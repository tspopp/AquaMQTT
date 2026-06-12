#include "mode/v5/SerialRelayTask.h"

#include <esp_task_wdt.h>
#include <driver/gpio.h>
#include <esp_rom_gpio.h>
#include <soc/uart_periph.h>

#include "config/Configuration.h"

#include "mode/v5/constants.h"
#include "mode/v5/Frame.h"


// TODO: should depend on baud rate?
#define FRAME_SILENCE_MS 4


namespace aquamqtt
{

SerialRelayTask& SerialRelayTask::getInstance()
{
    static SerialRelayTask instance(38400, MAX_FRAME_SIZE);
    return instance;
}

SerialRelayTask::SerialRelayTask(int baud_rate, int max_frame_size)
    : Task("relay")
    , baudRate(baud_rate)
    , maxFrameSize(max_frame_size)
{
    mHmiFrameBuffer = new uint8_t[maxFrameSize];
    mMainFrameBuffer = new uint8_t[maxFrameSize];
}

SerialRelayTask::~SerialRelayTask() {
    delete mHmiFrameBuffer;
    delete mMainFrameBuffer;
}

void SerialRelayTask::setup()
{
    Task::setup();

    Serial1.begin(baudRate, SERIAL_8N1, config::GPIO_HMI_RX, config::GPIO_HMI_TX);
    Serial2.begin(baudRate, SERIAL_8N1, config::GPIO_MAIN_RX, config::GPIO_MAIN_TX);

        // Serials already opened from main setup()
    // TX enable pins: HIGH=transmit, LOW=receive
    pinMode(config::GPIO_ENABLE_TX_HMI, OUTPUT);
    pinMode(config::GPIO_ENABLE_TX_MAIN, OUTPUT);
    digitalWrite(config::GPIO_ENABLE_TX_HMI, LOW);    // receive from HMI
    digitalWrite(config::GPIO_ENABLE_TX_MAIN, LOW);    // receive from Main

    // === ONE-WIRE UART: Fully disconnect TX pins during receive ===
    // The SN74LVC2T45 in receive mode (DIR=LOW) makes B-side inputs, A-side outputs.
    // The translator drives A1 (our TX pin) with bus data. If ESP's UART TX also drives
    // this pin, there's bus contention. Fix: disconnect UART TX output from pin entirely.
    // Route to SIG_GPIO_OUT_IDX (constant HIGH, pin not driven by peripheral)
    // Actually: just set pin as input and disconnect output signal
    gpio_set_direction((gpio_num_t)config::GPIO_MAIN_TX, GPIO_MODE_INPUT);
    gpio_set_direction((gpio_num_t)config::GPIO_HMI_TX, GPIO_MODE_INPUT);
    // Disconnect any output signal from these pins
    esp_rom_gpio_connect_out_signal(config::GPIO_MAIN_TX, SIG_GPIO_OUT_IDX, false, false);
    esp_rom_gpio_connect_out_signal(config::GPIO_HMI_TX, SIG_GPIO_OUT_IDX, false, false);

    // Also disconnect RX pins from output (they should only be inputs)
    // The UART RX input is still connected via gpio_connect_in_signal from Serial.begin()
    gpio_set_direction((gpio_num_t)config::GPIO_MAIN_RX, GPIO_MODE_INPUT);
    gpio_set_direction((gpio_num_t)config::GPIO_HMI_RX, GPIO_MODE_INPUT);
}

void SerialRelayTask::loop()
{
    Task::loop();

    unsigned long now = millis();

    // ===== HMI side: receive frames from HMI controller =====
    if (mHmiFrameInProgress) {
        processAndForwardIfReady(message::FrameBufferChannel::CH_HMI, mHmiFrameBuffer, mHmiFrameLength, Serial2, now - mHmiLastByteTime);
    }

    while (Serial1.available())
    {
        uint8_t byte = Serial1.read();
        mHmiBytesIn++;
        now = millis();

        processAndForwardIfReady(message::FrameBufferChannel::CH_HMI, mHmiFrameBuffer, mHmiFrameLength, Serial2, now - mHmiLastByteTime);

        if (mHmiFrameLength < maxFrameSize)
        {
            mHmiFrameBuffer[mHmiFrameLength++] = byte;
        }
        mHmiLastByteTime    = now;
        mHmiFrameInProgress = true;
    }

    // ===== Main controller side: receive frames from Main controller =====
    if (mMainFrameInProgress) {
        processAndForwardIfReady(message::FrameBufferChannel::CH_MAIN, mMainFrameBuffer, mMainFrameLength, Serial1, now - mMainLastByteTime);
    }

    while (Serial2.available())
    {
        uint8_t byte = Serial2.read();
        mMainBytesIn++;
        now = millis();

        processAndForwardIfReady(message::FrameBufferChannel::CH_MAIN, mMainFrameBuffer, mMainFrameLength, Serial1, now - mMainLastByteTime);

        if (mMainFrameLength < maxFrameSize)
        {
            mMainFrameBuffer[mMainFrameLength++] = byte;
        }
        mMainLastByteTime    = now;
        mMainFrameInProgress = true;
    }
}

void SerialRelayTask::processAndForwardIfReady(message::FrameBufferChannel channel, uint8_t* buffer, uint8_t length, HardwareSerial& destination, long delay)
{
    if (delay >= FRAME_SILENCE_MS || protocol_callback->frameIsReady(channel, mHmiFrameBuffer, mHmiFrameLength)) {
        processAndForward(channel, buffer, length, destination);
        switch (channel) {
        case message::FrameBufferChannel::CH_HMI:
            mHmiFrameLength     = 0;
            mHmiFrameInProgress = false;
            break;
        case message::FrameBufferChannel::CH_MAIN:
            mMainFrameLength     = 0;
            mMainFrameInProgress = false;
            break;
        default:
            break;
        }
    }
}

void SerialRelayTask::processAndForward(message::FrameBufferChannel channel, uint8_t* buffer, uint8_t length, HardwareSerial& destination)
{
    // Count per-side idncoming frames
    if (channel == message::FrameBufferChannel::CH_HMI) {
        mHmiFramesIn++;
    } else {
        mMainFramesIn++;
    }

    // Parse the frame for state extraction
    bool success = protocol_callback->processFrame(channel, buffer, length, err_message, ERROR_MESSAGE_SIZE);
    if (!success) {
        log_line(err_message);
        return; // abort
    }

    // Determine pins and UART number for the destination side
    uint8_t txEnablePin;
    uint8_t rxPin;
    int     uartNum;

    if (channel == message::FrameBufferChannel::CH_HMI)
    {
        // Forwarding toward Main (Serial2 = UART2)
        txEnablePin = config::GPIO_ENABLE_TX_MAIN;
        rxPin       = config::GPIO_MAIN_RX;  // GPIO 5, connected to SN74LVC2T45 A2
        uartNum     = 2;
    }
    else
    {
        // Forwarding toward HMI (Serial1 = UART1)
        txEnablePin = config::GPIO_ENABLE_TX_HMI;
        rxPin       = config::GPIO_HMI_RX;   // GPIO 7, connected to SN74LVC2T45 A2
        uartNum     = 1;
    }

    // === ONE-WIRE UART FIX ===
    // The SN74LVC2T45 has BOTH B1 and B2 connected to the same one-wire bus.
    // When DIR=HIGH (transmit mode, A→B): A1 drives B1, A2 drives B2.
    // When DIR=LOW (receive mode, B→A): B drives through to A1 and A2.
    //
    // Problem 1: During TX, if A2 (RX pin) isn't driven with TX signal, B1≠B2 = contention
    // Problem 2: During RX, if UART TX output still drives A1, it fights the translator
    //
    // Solution: 
    //   TX mode: connect UART TX signal to BOTH A1 (txPin) and A2 (rxPin)
    //   RX mode: disconnect all output signals, pins are pure inputs
    int txSignal = uart_periph_signal[uartNum].pins[SOC_UART_TX_PIN_IDX].signal;
    int rxSignal = uart_periph_signal[uartNum].pins[SOC_UART_RX_PIN_IDX].signal;
    uint8_t txPin = channel == message::FrameBufferChannel::CH_HMI ? config::GPIO_MAIN_TX : config::GPIO_HMI_TX;

    // Connect UART TX output to TX pin (A1)
    gpio_set_direction((gpio_num_t)txPin, GPIO_MODE_OUTPUT);
    esp_rom_gpio_connect_out_signal(txPin, txSignal, false, false);

    // Connect UART TX output to RX pin (A2) — both pins carry same data
    gpio_set_direction((gpio_num_t)rxPin, GPIO_MODE_INPUT_OUTPUT);
    esp_rom_gpio_connect_out_signal(rxPin, txSignal, false, false);

    // Small delay for GPIO matrix and translator to settle
    delayMicroseconds(10);

    // Enable transmit direction on level translator (DIR=HIGH → A→B)
    digitalWrite(txEnablePin, HIGH);

    // Send frame
    size_t written = destination.write(buffer, length);
    destination.flush();
    mTxBytesWritten += written;

    // Back to receive mode (DIR=LOW → B→A)
    digitalWrite(txEnablePin, LOW);

    // Fully disconnect TX pin from UART output (prevent fighting translator)
    esp_rom_gpio_connect_out_signal(txPin, SIG_GPIO_OUT_IDX, false, false);
    gpio_set_direction((gpio_num_t)txPin, GPIO_MODE_INPUT);

    // Restore RX pin as UART RX input
    esp_rom_gpio_connect_out_signal(rxPin, SIG_GPIO_OUT_IDX, false, false);
    gpio_set_direction((gpio_num_t)rxPin, GPIO_MODE_INPUT);
    esp_rom_gpio_connect_in_signal(rxPin, rxSignal, false);

    // Clear any echo bytes that appeared in RX buffer during transmit
    delayMicroseconds(200);
    while (destination.available()) { destination.read(); mEchoBytes++; }

    mFramesRelayed++;
}

void SerialRelayTask::periodicUpdate()
{
    Task::periodicUpdate();

    LOG.print("[");
    LOG.print(taskName);
    LOG.print("] frames received: ");
    LOG.print(mMainFramesIn);
    LOG.println();
}


}  // namespace aquamqtt
