#include <Arduino.h>
#include <WiFi.h>
#include <esp_task_wdt.h>

#include "config/Configuration.h"
#include "handler/OTA.h"
#include "handler/RTC.h"
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

void loop()
{
    // watchdog
    esp_task_wdt_reset();

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
    esp_task_wdt_init(WATCHDOG_TIMEOUT_S, true);
    esp_task_wdt_add(nullptr);

    // connect to wifi
    WiFiClass::mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.begin(aquamqtt::config::ssid, aquamqtt::config::psk);

    // setup rtc module
    rtcHandler.setup();

    // setup ota module
    otaHandler.setup();

    // if listener mode is set in configuration, just read the DHW traffic from a single One-Wire USART instance
    if (OPERATION_MODE == LISTENER)
    {
        // reads 194, 193 and 67 message and notifies the mqtt task
        listenerTask.spawn();
    }
    // if man-in-the-middle mode is set in configuration, there are two physical One-Wire USART instances
    // and AquaMQTT forwards (modified) messages from one to another
    else
    {
        // reads 194 message from the hmi controller, writes 193 and 67 to the hmi controller
        hmiTask.spawn();

        // reads 193 and 67 from the main controller, writes 194 to the main controller
        controllerTask.spawn();
    }

    // provide the message information via mqtt and enables overrides via mqtt
   mqttTask.spawn();
}