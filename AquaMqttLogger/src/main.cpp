#include <Arduino.h>
#include <avr/wdt.h>
#include <SoftwareSerial.h>

#include "ESPLink.h"
#include "FrameBuffer.h"
#include "FrameHandler.h"
#include "HMIState.h"

ESPLink&             espLink = ESPLink::getInstance();
FrameHandler         handler(&espLink);
FrameBuffer          buffer(&handler);
HMIState             hmiState;

FastCRC16 mCRC;

void setup()
{
    wdt_disable();

    wdt_enable(WDTO_2S);

    Serial.begin(115200);
    espLink.init(&Serial);
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

        // TODO, this will cause any magic byte 194 to emit a message, this is truly bad.
        //  we need something better.... for our poc this will be sufficient.
        if (val != 194)
        {
            buffer.pushByte(val);
        }
        else if (hmiState.updateMessage())
        {
            uint16_t actualCRC = mCRC.ccitt(hmiState.getMessage(), 35);
            Serial1.write(hmiState.getMessage(), 35);
            Serial1.write((uint8_t) (actualCRC >> 8));    // extract the high byte
            Serial1.write((uint8_t) (actualCRC & 0xFF));  // extract the low byte

            // eat our own dogfood, we are not reading when writing to the one-wire bus ....
            buffer.pushByte(val);

            for (int i = 0; i < 35; ++i)
            {
                buffer.pushByte(hmiState.getMessage()[i]);
            }
            buffer.pushByte((uint8_t) (actualCRC >> 8));
            buffer.pushByte((uint8_t) (actualCRC & 0xFF));
        }
    }
}