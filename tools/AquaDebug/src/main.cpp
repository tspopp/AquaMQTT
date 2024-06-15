#include <Arduino.h>
#include <MQTT.h>
#include <RingBuf.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <esp_task_wdt.h>

#include "config/Configuration.h"
#include "handler/OTA.h"
#include "mqtt/MQTTDefinitions.h"

using namespace aquamqtt;
using namespace aquamqtt::config;
using namespace aquamqtt::mqtt;

constexpr uint16_t BUFFER_SIZE = 40;

WiFiClient                mWiFiClient;
MQTTClient                mMQTTClient(256);
RingBuf<int, BUFFER_SIZE> mBuffer;
OTAHandler                otaHandler;
uint8_t                   mTopicBuffer[config::MQTT_MAX_TOPIC_SIZE];
uint8_t                   mPayloadBuffer[MQTT_MAX_PAYLOAD_SIZE];
uint8_t                   mTempBuffer[BUFFER_SIZE];

void toHexStr(uint8_t* data, uint8_t data_len, char* buffer)
{
    const size_t num_bytes = data_len / sizeof(uint8_t);
    // char         hex_str[num_bytes * 2 + 1];
    for (size_t i = 0; i < num_bytes; i++)
    {
        sprintf(&buffer[i * 2], "%02X", data[i]);
    }
    buffer[num_bytes * 2] = '\0';
}

void wifiCallback(WiFiEvent_t event)
{
    switch (event)
    {
        case WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED:
            Serial.println(F("ARDUINO_EVENT_WIFI_STA_CONNECTED"));
            break;
        case WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println(F("ARDUINO_EVENT_WIFI_STA_DISCONNECTED"));
            WiFi.setAutoReconnect(true);
            break;
        default:
            break;
    }
}

void loop()
{
    // watchdog
    esp_task_wdt_reset();

    // handle over-the-air module in main thread
    otaHandler.loop();

    // connect mqtt if not connected
    if (!mMQTTClient.connected())
    {
        mMQTTClient.connect(
                aquamqtt::config::brokerClientId,
                strlen(aquamqtt::config::brokerUser) == 0 ? nullptr : aquamqtt::config::brokerUser,
                strlen(aquamqtt::config::brokerPassword) == 0 ? nullptr : aquamqtt::config::brokerPassword);
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
            for (int i = 0; i < BUFFER_SIZE; i++)
            {
                int retVal;
                mBuffer.pop(retVal);
                mTempBuffer[i] = retVal;
            }
            sprintf(reinterpret_cast<char*>(mTopicBuffer), "%s%s%s", config::mqttPrefix, BASE_TOPIC, DEBUG);
            toHexStr(mTempBuffer, BUFFER_SIZE, reinterpret_cast<char*>(mPayloadBuffer));
            mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
        }
    }
}

void setup()
{
    // limited serial output for debuggability
    Serial.begin(9600);
    Serial.println("REBOOT");

    // initialize watchdog
    esp_task_wdt_init(WATCHDOG_TIMEOUT_S, true);
    esp_task_wdt_add(nullptr);

    // connect to Wi-Fi
    WiFiClass::mode(WIFI_STA);
    WiFi.onEvent(wifiCallback);
    WiFi.begin(aquamqtt::config::ssid, aquamqtt::config::psk);

    // setup ota module
    otaHandler.setup();

    // setup mqtt client
    mMQTTClient.begin(aquamqtt::config::brokerAddr, aquamqtt::config::brokerPort, mWiFiClient);

    // setup serial port
    Serial2.begin(9550, SERIAL_8N2, aquamqtt::config::GPIO_MAIN_RX, aquamqtt::config::GPIO_MAIN_TX);
}