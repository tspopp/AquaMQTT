#include <Arduino.h>
#include <MQTT.h>
#include <RingBuf.h>
#include <WiFiClient.h>
#include <esp_task_wdt.h>

#include "config/Configuration.h"
#include "handler/OTA.h"
#include "handler/Wifi.h"
#include "mqtt/MQTTDefinitions.h"

using namespace aquamqtt;
using namespace aquamqtt::config;
using namespace aquamqtt::mqtt;

constexpr uint16_t BUFFER_SIZE_BYTES = 256;
constexpr uint16_t BUFFER_SIZE_STR   = BUFFER_SIZE_BYTES * 2 + 1;

WiFiClient                      mWiFiClient;
MQTTClient                      mMQTTClient(BUFFER_SIZE_STR);
RingBuf<int, BUFFER_SIZE_BYTES> mBuffer;
OTAHandler                      otaHandler;
WifiHandler                     wifiHandler;
uint8_t                         mTopicBufferStr[80];
uint8_t                         mPayloadBufferStr[BUFFER_SIZE_STR];
uint8_t                         mTempBuffer[BUFFER_SIZE_BYTES];
esp_task_wdt_config_t           twdt_config = {
              .timeout_ms     = WATCHDOG_TIMEOUT_MS,
              .idle_core_mask = (1 << configNUM_CORES) - 1,
              .trigger_panic  = true,
};

void toHexStr(uint8_t* data, char* buffer)
{
    for (size_t i = 0; i < BUFFER_SIZE_BYTES; i++)
    {
        sprintf(&buffer[i * 2], "%02X", data[i]);
    }
    buffer[BUFFER_SIZE_BYTES * 2] = '\0';
}

void loop()
{
    // watchdog
    esp_task_wdt_reset();
    delay(1);

    // handle wifi events
    wifiHandler.loop();

    // handle over-the-air module in main thread
    otaHandler.loop();

    // connect mqtt if not connected
    if (!mMQTTClient.connected())
    {
        mMQTTClient.connect(
                brokerClientId,
                strlen(brokerUser) == 0 ? nullptr : brokerUser,
                strlen(brokerPassword) == 0 ? nullptr : brokerPassword);
    }
    else
    {
        mMQTTClient.loop();
    }

    // read serial into buffer
    while (Serial2.available())
    {
        // push new element to the back
        mBuffer.push(Serial2.read());

        // if buffer is full, we emit everything to mqtt and clear the buffer
        if (mBuffer.isFull())
        {
            for (int i = 0; i < BUFFER_SIZE_BYTES; i++)
            {
                int retVal;
                mBuffer.pop(retVal);
                mTempBuffer[i] = retVal;
            }
            sprintf(reinterpret_cast<char*>(mTopicBufferStr), "%s%s%s", mqttPrefix, BASE_TOPIC, DEBUG);
            toHexStr(mTempBuffer, reinterpret_cast<char*>(mPayloadBufferStr));
            mMQTTClient.publish(reinterpret_cast<char*>(mTopicBufferStr), reinterpret_cast<char*>(mPayloadBufferStr));
        }
    }
}

void setup()
{
    // limited serial output for debuggability
    Serial.begin(9600);
    Serial.println("REBOOT");

    // initialize watchdog
    esp_task_wdt_deinit();
    esp_task_wdt_init(&twdt_config);
    esp_task_wdt_add(nullptr);

    // setup wifi
    wifiHandler.setup();

    // setup ota module
    otaHandler.setup();

    // setup mqtt client
    mMQTTClient.begin(brokerAddr, brokerPort, mWiFiClient);

    // setup serial port
    Serial2.begin(9550, SERIAL_8N2, GPIO_MAIN_RX, GPIO_MAIN_TX);
}