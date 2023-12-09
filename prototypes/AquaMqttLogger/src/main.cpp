#include <Arduino.h>
#include <SoftwareSerial.h>
#include <avr/wdt.h>

#include "ESPLink.h"
#include "FrameBuffer.h"
#include "FrameHandler.h"
#include "HMIState.h"

ESPLink&     espLink = ESPLink::getInstance();
FrameHandler handler(&espLink);
FrameBuffer  buffer(&handler);
HMIState     hmiState;
FastCRC16    mCRC;

/**
 * In case no HMI is connected, this application will act as headless HMI which is controlled using MQTT
 */
const bool hmiConnected = false;

void setup()
{
    wdt_disable();

    wdt_enable(WDTO_2S);

    Serial.begin(115200);
    Serial.println(F("REBOOT"));

    // heatpump serial is only started, if esp8266 is in sync
    if (espLink.setup())
    {
        Serial.println("connected to esp!");
        Serial1.begin(9550, SERIAL_8N2);
    }

    espLink.setMqttCallback(&hmiState);

    Serial.println("SETUP OK");
}

void loop()
{
    wdt_reset();

    espLink.loop();

    while (Serial1.available())
    {
        int val = Serial1.read();
        buffer.pushByte(val);

        if (hmiConnected)
        {
            // TODO: this will cause any magic byte 194 to emit a message
            // TODO: assert sequence before emitting an hmi message
            if (val == 194 && hmiState.updateMessage())
            {
                uint16_t actualCRC = mCRC.ccitt(hmiState.getMessage(), 35);
                Serial1.write(hmiState.getMessage(), 35);
                Serial1.write((uint8_t) (actualCRC >> 8));    // extract the high byte
                Serial1.write((uint8_t) (actualCRC & 0xFF));  // extract the low byte

                // provide the messages we wer writing to our own buffer as well (updating any mqtt topics)
                for (int i = 0; i < 35; ++i)
                {
                    buffer.pushByte(hmiState.getMessage()[i]);
                }
                buffer.pushByte((uint8_t) (actualCRC >> 8));
                buffer.pushByte((uint8_t) (actualCRC & 0xFF));
            }
        }
    }
}