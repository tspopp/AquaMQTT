#include "task/ModbusListenerTask.h"

#include <esp_task_wdt.h>

#include "config/Configuration.h"
#include "state/Optitronic2State.h"

using namespace aquamqtt::message::optitronic2;

namespace aquamqtt
{

ModbusListenerTask::ModbusListenerTask()
    : mFrameBuffer{}
    , mFrameLength(0)
    , mLastByteTime(0)
    , mFrameInProgress(false)
    , mPendingReadRequest(false)
    , mPendingReadStartReg(0)
    , mPendingReadQuantity(0)
    , mFramesReceived(0)
    , mCrcErrors(0)
    , mRawBytesReceived(0)
    , mRawBytesHmi(0)
    , mLastStatsUpdate(0)
{
}

void ModbusListenerTask::spawn()
{
    TaskHandle_t handle;
    xTaskCreatePinnedToCore(ModbusListenerTask::innerTask, "modbusListenerLoop", 4096, this, 4, &handle, 1);
    esp_task_wdt_add(handle);
}

[[noreturn]] void ModbusListenerTask::innerTask(void* pvParameters)
{
    auto* self = static_cast<ModbusListenerTask*>(pvParameters);

    self->setup();

    while (true)
    {
        esp_task_wdt_reset();
        self->loop();
    }
}

void ModbusListenerTask::setup()
{
    Serial2.begin(
            MODBUS_BAUD_RATE,
            SERIAL_8N1,
            config::GPIO_MAIN_RX,
            config::GPIO_MAIN_TX);
}

void ModbusListenerTask::loop()
{
    unsigned long now = millis();

    // Check for inter-frame silence (frame boundary detection)
    if (mFrameInProgress && (now - mLastByteTime) >= MODBUS_FRAME_SILENCE_MS)
    {
        // Frame complete — process it
        if (mFrameLength >= 5)  // minimum valid Modbus frame
        {
            processFrame();
        }
        mFrameLength    = 0;
        mFrameInProgress = false;
    }

    // Read available bytes from serial
    while (Serial2.available())
    {
        uint8_t byte = Serial2.read();
        mRawBytesReceived++;
        now          = millis();

        // If we were accumulating a frame and there was a gap, process the previous frame first
        if (mFrameInProgress && (now - mLastByteTime) >= MODBUS_FRAME_SILENCE_MS)
        {
            if (mFrameLength >= 5)
            {
                processFrame();
            }
            mFrameLength    = 0;
            mFrameInProgress = false;
        }

        // Start or continue accumulating a frame
        if (mFrameLength < MODBUS_MAX_FRAME_SIZE)
        {
            mFrameBuffer[mFrameLength++] = byte;
        }
        mLastByteTime    = now;
        mFrameInProgress = true;
    }

    // Periodic stats update
    if ((now - mLastStatsUpdate) >= 5000)
    {
        Optitronic2State::getInstance().updateStats(mFramesReceived, mCrcErrors);

        Serial.print("[modbus]: frames=");
        Serial.print(mFramesReceived);
        Serial.print(", crcErr=");
        Serial.println(mCrcErrors);

        mLastStatsUpdate = now;
    }

    // Small delay to prevent starving other tasks
    vTaskDelay(pdMS_TO_TICKS(1));
}

void ModbusListenerTask::processFrame()
{
    ModbusFrame frame;

    if (!parseModbusFrame(mFrameBuffer, mFrameLength, frame))
    {
        mCrcErrors++;
        return;
    }

    mFramesReceived++;

    // Only process frames for our slave address
    if (frame.slaveAddress != MODBUS_SLAVE_ADDRESS)
    {
        return;
    }

    switch (frame.functionCode)
    {
        case FC_READ_HOLDING_REGISTERS:
        {
            // Determine if this is a request (4 data bytes) or response (has byte count)
            if (frame.dataLength == 4)
            {
                // This is a master READ REQUEST: [startHi, startLo, qtyHi, qtyLo]
                ModbusReadRequest request;
                if (parseReadRequest(frame, request))
                {
                    mPendingReadRequest  = true;
                    mPendingReadStartReg = request.startRegister;
                    mPendingReadQuantity = request.quantity;
                }
            }
            else if (frame.dataLength >= 3 && mPendingReadRequest)
            {
                // This is a slave READ RESPONSE: [byteCount, data...]
                ModbusReadResponse response;
                if (parseReadResponse(frame, response))
                {
                    // Validate response matches our pending request
                    if (response.registerCount == mPendingReadQuantity)
                    {
                        // Store register values in state
                        Optitronic2State::getInstance().storeRegisters(
                                mPendingReadStartReg, response.registers, response.registerCount);
                    }
                }
                mPendingReadRequest = false;
            }
            break;
        }

        case FC_WRITE_SINGLE_REGISTER:
        {
            // Both request and response have same format: [regHi, regLo, valHi, valLo]
            ModbusWriteSingle write;
            if (parseWriteSingle(frame, write))
            {
                // Store the written value
                Optitronic2State::getInstance().storeSingleRegister(write.registerAddress, write.value);
            }
            // A write clears any pending read request context
            mPendingReadRequest = false;
            break;
        }

        case FC_WRITE_MULTIPLE_REGISTERS:
        {
            // Master request: [startHi, startLo, qtyHi, qtyLo, byteCount, data...]
            if (frame.dataLength > 5)
            {
                ModbusWriteMultiple write;
                if (parseWriteMultiple(frame, write))
                {
                    Optitronic2State::getInstance().storeRegisters(
                            write.startRegister, write.values, write.quantity);
                }
            }
            // Response for fn=0x10 is just [startHi, startLo, qtyHi, qtyLo] (4 bytes) — ignore
            mPendingReadRequest = false;
            break;
        }

        default:
            // Unknown function code — ignore
            mPendingReadRequest = false;
            break;
    }
}

}  // namespace aquamqtt
