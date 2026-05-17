#include <Arduino.h>
#include <LittleFS.h>
#include <esp_task_wdt.h>

#include "config/Configuration.h"
#include "config/WebConfig.h"
#include "handler/OTA.h"
#include "handler/RTC.h"
#include "handler/Web.h"
#include "handler/Wifi.h"
#include "task/ControllerTask.h"
#include "task/HMITask.h"
#include "task/ListenerTask.h"
#include "task/ModbusListenerTask.h"
#include "task/ModbusRelayTask.h"
#include "task/MQTTTask.h"
#include "task/Optitronic2MQTTTask.h"

using namespace aquamqtt;
using namespace aquamqtt::config;

HMITask              hmiTask;
ControllerTask       controllerTask;
ListenerTask         listenerTask;
MQTTTask             mqttTask;
ModbusListenerTask   modbusListenerTask;
ModbusRelayTask      modbusRelayTask;
Optitronic2MQTTTask  optitronic2MqttTask;
OTAHandler           otaHandler;
RTCHandler           rtcHandler;
WifiHandler          wifiHandler;
WebHandler           webHandler;

// Raw serial test counters (read from main loop, reported via /api/diag)


esp_task_wdt_config_t twdt_config = {
    .timeout_ms     = WATCHDOG_TIMEOUT_MS,
    .idle_core_mask = (1 << configNUM_CORES) - 1,
    .trigger_panic  = true,
};

void loop()
{
    // watchdog
    esp_task_wdt_reset();

    // handle wifi events
    wifiHandler.loop();

    // handle over-the-air module in main thread
    otaHandler.loop();

    // handle real-time-clock module in main thread
    rtcHandler.loop();

    // handle web server
    webHandler.loop();

    // Drive relay directly from main loop (FreeRTOS task can't read Serial1)
    modbusRelayTask.loop();
}

void setup()
{
    // limited serial output for debuggability
    Serial.begin(9600);
    Serial.println("REBOOT");

    // mount LittleFS filesystem (true = format on first use)
    if (!LittleFS.begin(true))
    {
        Serial.println("[fs] LittleFS mount failed even after format");
    }

    // load configuration from filesystem
    loadMqttConfig();
    loadAquaMqttConfig();

    // initialize watchdog EARLY (tasks need it for esp_task_wdt_add)
    esp_task_wdt_deinit();
    esp_task_wdt_init(&twdt_config);
    esp_task_wdt_add(nullptr);

    // determine operation mode
    EOperationMode opMode = static_cast<EOperationMode>(aquaMqttConfig.operationMode);

    // Open serial ports for Optitronic2 modes (must be in main context to work)
    if (opMode == OPTITRONIC2_MITM || opMode == OPTITRONIC2_LISTENER)
    {
        Serial1.begin(57600, SERIAL_8N1, config::GPIO_HMI_RX, config::GPIO_HMI_TX);
        Serial2.begin(57600, SERIAL_8N1, config::GPIO_MAIN_RX, config::GPIO_MAIN_TX);
        modbusRelayTask.setup();
        Serial.println("[setup] Serial1+Serial2 opened, relay initialized");
    }

    // Now setup WiFi (may take several seconds)
    if (!loadWifiConfig())
    {
        Serial.println("[setup] WiFi config not found, starting AP mode");
        wifiHandler.setupAP();
    }
    else
    {
        wifiHandler.setup();
    }

    // setup rtc module
    rtcHandler.setup();

    // setup ota module
    otaHandler.setup();

    // setup web server
    webHandler.setup();

    // spawn remaining tasks that depend on WiFi/MQTT
    if (opMode == LISTENER)
    {
        listenerTask.spawn();
    }
    else if (opMode == MITM)
    {
        hmiTask.spawn();
        controllerTask.spawn();
    }

    // Optitronic2 MQTT task (needs WiFi but relay already running)
    if (opMode == OPTITRONIC2_LISTENER || opMode == OPTITRONIC2_MITM)
    {
        optitronic2MqttTask.spawn();
    }

    // provide the message information via mqtt and enables overrides via mqtt
    // (only for Atlantic protocol modes)
    if (opMode == LISTENER || opMode == MITM)
    {
        mqttTask.spawn();
    }
}