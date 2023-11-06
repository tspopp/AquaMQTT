#ifndef AQUAMQTT_ESPLINK_H
#define AQUAMQTT_ESPLINK_H

#include <ELClient.h>
#include <ELClientCmd.h>
#include <ELClientMqtt.h>

#include "MQTTDefinitions.h"

class ESPLink
{
public:
    static ESPLink& getInstance()
    {
        static ESPLink instance;
        return instance;
    }

    virtual ~ESPLink() = default;

    void init(Stream* serial);

    bool setup();

    void loop();

    ELClientMqtt* getMqttClient()
    {
        return mELClientMqtt;
    }

    void setMqttCallback(IMQTTCallback* callback)
    {
        mCallback = callback;
    }

private:
    ESPLink();
    ESPLink(ESPLink const&);         // Don't Implement.
    void operator=(ESPLink const&);  // Don't implement

    uint32_t mLastNTPSync;
    void     updateInternalClock();

    static void wifiCb(void* response);
    static void mqttConnected(void* response);
    static void mqttDisconnected(void* response);
    static void mqttData(void* response);

    Stream*        mSerial;
    ELClient*      mElClient;
    ELClientCmd*   mELClientCmd;
    ELClientMqtt*  mELClientMqtt;
    IMQTTCallback* mCallback;
};

#endif  // AQUAMQTT_ESPLINK_H
