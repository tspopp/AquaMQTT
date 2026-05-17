#ifndef AQUAMQTT_WIFI_H
#define AQUAMQTT_WIFI_H

#include <WiFi.h>

namespace aquamqtt
{
class WifiHandler
{
public:
    WifiHandler();

    virtual ~WifiHandler() = default;

    bool setup();

    void setupAP();

    void loop();

    static bool isConnected() { return mConnectedToWifiWithValidIpAddress; }

private:
    static void wifiCallback(WiFiEvent_t event);

    unsigned long mLastCheck;

    static bool mConnectedToWifiWithValidIpAddress;

};

}  // namespace aquamqtt



#endif  // AQUAMQTT_WIFI_H
