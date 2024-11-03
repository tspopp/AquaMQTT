#include <Arduino.h>
#include <esp_task_wdt.h>

#include "config/Configuration.h"
#include "handler/OTA.h"
#include "handler/RTC.h"
#include "handler/Wifi.h"
#include "task/ControllerTask.h"
#include "task/HMITask.h"
#include "task/ListenerTask.h"
#include "task/MQTTTask.h"

using namespace aquamqtt;
using namespace aquamqtt::config;

HMITask        hmiTask;
ControllerTask controllerTask;
ListenerTask   listenerTask;
MQTTTask       mqttTask;
OTAHandler     otaHandler;
RTCHandler     rtcHandler;
WifiHandler    wifiHandler;

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
}

void setup()
{
    // limited serial output for debuggability
    Serial.begin(9600);
    Serial.println("REBOOT");

    // initialize watchdog
    esp_task_wdt_init(&twdt_config);
    esp_task_wdt_add(nullptr);

    // setup wifi
    wifiHandler.setup();

    // setup rtc module
    rtcHandler.setup();

    // setup ota module
    otaHandler.setup();

//    // if listener mode is set in configuration, just read the DHW traffic from a single One-Wire USART instance
//    if (OPERATION_MODE == LISTENER)
//    {
        // reads 194, 193, 67 and 74 message and notifies the mqtt task
        listenerTask.spawn();
//    }
//    // if man-in-the-middle mode is set in configuration, there are two physical One-Wire USART instances
//    // and AquaMQTT forwards (modified) messages from one to another
//    else
//    {
//        // reads 194 message from the hmi controller, writes 193, 67 and 74 to the hmi controller
//        hmiTask.spawn();
//
//        // reads 193, 67 and 74 from the main controller, writes 194 to the main controller
//        controllerTask.spawn();
//    }

    // provide the message information via mqtt and enables overrides via mqtt
    mqttTask.spawn();
}