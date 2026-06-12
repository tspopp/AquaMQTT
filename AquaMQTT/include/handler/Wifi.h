#ifndef AQUAMQTT_WIFI_H
#define AQUAMQTT_WIFI_H

#include <WiFi.h>

namespace aquamqtt
{
class WifiHandler final
{
public:
    static WifiHandler& getInstance();

    bool connected() { return mConnectedToWifiWithValidIpAddress; };
    void setup();
    void loop();

private:
    WifiHandler();
    virtual ~WifiHandler() = default;

private:
    static void wifiCallback(WiFiEvent_t event);

    unsigned long mLastCheck;

    static bool mConnectedToWifiWithValidIpAddress;

};

}  // namespace aquamqtt



#endif  // AQUAMQTT_WIFI_H
