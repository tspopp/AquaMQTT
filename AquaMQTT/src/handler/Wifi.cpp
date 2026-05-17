#include "handler/Wifi.h"

#include "config/Configuration.h"
#include "config/WebConfig.h"

namespace aquamqtt
{

bool WifiHandler::mConnectedToWifiWithValidIpAddress = false;

WifiHandler::WifiHandler() : mLastCheck(0)
{
}

void WifiHandler::setupAP()
{
    WiFiClass::mode(WIFI_AP);
    WiFi.disconnect();
    String macAddress = WiFi.softAPmacAddress();
    macAddress.replace(":", "");
    String apName = "aquamqtt-" + macAddress.substring(0, 4);
    WiFi.softAP(apName.c_str());
    Serial.print("[wifi] AP started: ");
    Serial.println(apName);
    Serial.print("[wifi] AP IP: ");
    Serial.println(WiFi.softAPIP().toString().c_str());
}

bool WifiHandler::setup()
{
    WiFiClass::mode(WIFI_STA);

    WiFi.setAutoReconnect(false);
    WiFi.onEvent(wifiCallback);

    Serial.print("[wifi] connecting to SSID: ");
    Serial.println(wifiConfig.ssid.c_str());

    WiFi.begin(wifiConfig.ssid.c_str(), wifiConfig.password.c_str());

    mLastCheck = millis();
    return true;
}

void WifiHandler::loop()
{
    if ((millis() - mLastCheck) >= (config::WIFI_RECONNECT_CYCLE_S * 1000))
    {
        mLastCheck = millis();

        // we don't trust WiFi.isConnected() or WiFi.status() == WL_CONNECTED, since it is suspected to be unreliable
        if (!mConnectedToWifiWithValidIpAddress)
        {
            Serial.println("[wifi] attempting reconnect");
            WiFi.disconnect();
            WiFi.reconnect();
        }
    }
}

void WifiHandler::wifiCallback(WiFiEvent_t event)
{
    Serial.print("[wifi] event: ");
    Serial.println(WiFi.eventName(event));

    switch (event)
    {
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        case ARDUINO_EVENT_WIFI_STA_LOST_IP:
            // if we lost connection or ip address, we wil enforce a reconnect within the next cycle
            mConnectedToWifiWithValidIpAddress = false;
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
            // if we got connection and therefore a valid ip address we have a valid connection
            Serial.print("[wifi] ip address: ");
            Serial.println(WiFi.localIP().toString().c_str());
            mConnectedToWifiWithValidIpAddress = true;
            break;
        default:
            break;
    }
}

}  // namespace aquamqtt