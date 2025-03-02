#ifndef AQUAMQTT_WIFI_H
#define AQUAMQTT_WIFI_H

#include <WiFi.h>
#include "config/config.h"
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

private:
    static void wifiCallback(WiFiEvent_t event);

    unsigned long mLastCheck;

    static bool mConnectedToWifiWithValidIpAddress;
};

}  // namespace aquamqtt

#endif  // AQUAMQTT_WIFI_H
