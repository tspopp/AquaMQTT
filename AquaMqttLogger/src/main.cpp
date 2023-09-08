#include <Arduino.h>
#include <SoftwareSerial.h>
#include <avr/wdt.h>

#include "FrameBuffer.h"
#include "FrameHandler.h"

SoftwareSerial heatpumpSerial(8, 9);
FrameHandler   handler;
FrameBuffer    buffer(&handler);

void setup()
{
    wdt_disable();

    wdt_enable(WDTO_4S);

    Serial.begin(115200);
    Serial.println(F("REBOOT"));

    // heatpump serial is only started, if esp8266 is in sync
    if (handler.setup())
    {
        heatpumpSerial.begin(9600);
    }
}

void loop()
{
    wdt_reset();

    handler.loop();

    while (heatpumpSerial.available())
    {
        buffer.pushByte(heatpumpSerial.read());
    }
}