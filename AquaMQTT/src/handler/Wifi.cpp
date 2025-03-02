#include "handler/Wifi.h"

#include "config/Configuration.h"
#include "config/config.h"

extern struct aquamqtt::WifiConfigStruct wifiSettings;

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
    char AP_Name[] = "aquamqtt";
    WiFi.softAP(AP_Name);
}

bool WifiHandler::setup()
{
    WiFiClass::mode(WIFI_STA);

    // we don't trust the auto reconnect routine, as it seems there are edge cases where it does not work
    WiFi.setAutoReconnect(false);

    // we trust the wifi callbacks to determine if we are properly connected or disconnected
    WiFi.onEvent(wifiCallback);

    // begin a single wifi session
    // WiFi.begin(aquamqtt::config::ssid, aquamqtt::config::psk); //FIXME to be removed
    Serial.println("****** SSID is");
    Serial.println(String(wifiSettings.ssid));  // FIXME Not correctly formatted...
    // WiFi.begin(wifiSettings.ssid, wifiSettings.password);
    WiFi.begin("Livebox-8CAE", "julie0807");  // FiXME chagng with the previous line.
    // perform the next wifi check in config::WIFI_RECONNECT_CYCLE_S
    mLastCheck = millis();
    return true;
}

void WifiHandler::loop()
{
    if ((millis() - mLastCheck) >= (config::WIFI_RECONNECT_CYCLE_S * 1000))
    {
        mLastCheck = millis();
        Serial.println("Wifi RSSI is :");
        Serial.println(WiFi.RSSI());
        // we don't trust WiFi.isConnected() or WiFi.status() == WL_CONNECTED, since it is suspected to be unreliable
        if (!mConnectedToWifiWithValidIpAddress)
        {
            Serial.println("[wifi] attempting reconnect");
            // Serial.println("SSID");
            // Serial.println(wifiSettings.ssid);
            // Serial.println("Pwd");
            // Serial.println(wifiSettings.password);
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
        case ARDUINO_EVENT_WIFI_AP_START:
            Serial.println("WiFi access point started");
            break;
        case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
            Serial.println("Assigned IP address to client");
            break;
        default:
            break;
    }
}

}  // namespace aquamqtt