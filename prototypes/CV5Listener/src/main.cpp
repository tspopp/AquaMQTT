#include <Arduino.h>
#include <WiFiClient.h>
#include <buffer/ModbusBuffer.h>
#include <esp_task_wdt.h>

#include "config/Configuration.h"
#include "handler/OTA.h"
#include "handler/Wifi.h"
#include "mqtt/MQTTDefinitions.h"

using namespace aquamqtt;
using namespace aquamqtt::config;
using namespace aquamqtt::mqtt;

WiFiClient                mWiFiClient;
MQTTClient                mMQTTClient(256);
modbus::ModbusBuffer      mModbus(&mMQTTClient);
OTAHandler                otaHandler;
WifiHandler               wifiHandler;

esp_task_wdt_config_t twdt_config = {
    .timeout_ms     = WATCHDOG_TIMEOUT_MS,
    .idle_core_mask = (1 << configNUM_CORES) - 1,
    .trigger_panic  = true,
};

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

        delay(1000);
    }
    else
    {
        mMQTTClient.loop();
    }

    // read serial into buffer
    while (Serial2.available())
    {
        // tell modbus buffer about incoming stuff
        mModbus.pushByte(Serial2.read());
    }

    mMQTTClient.loop();
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
    Serial2.begin(DEFAULT_SERIAL_BAUD, DEFAULT_SERIAL_CONFIGURATION, GPIO_MAIN_RX, GPIO_MAIN_TX);
}