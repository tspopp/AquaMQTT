#include "ESPLink.h"

#include <TimeLib.h>

#include "MQTTDefinitions.h"

ESPLink::ESPLink() : mElClient(nullptr), mELClientCmd(nullptr), mELClientMqtt(nullptr), mCallback(nullptr)
{
}

void ESPLink::init(Stream* serial)
{

    mSerial = serial;
    mElClient     = new ELClient(serial);
    mELClientCmd  = new ELClientCmd(mElClient);
    mELClientMqtt = new ELClientMqtt(mElClient);
}

bool ESPLink::setup()
{
    mElClient->wifiCb.attach(wifiCb);
    int retry = 0;
    while (!mElClient->Sync())
    {
        retry++;
        if (retry >= 5)
        {
            mSerial->println(F("esp:nosync"));
            return false;
        }
    }
    mSerial->println(F("esp:sync"));

    mELClientMqtt->connectedCb.attach(mqttConnected);
    mELClientMqtt->disconnectedCb.attach(mqttDisconnected);
    mELClientMqtt->dataCb.attach(mqttData);
    mELClientMqtt->setup();

    return true;
}

void ESPLink::wifiCb(void* response)
{
    ELClientResponse* res = (ELClientResponse*) response;
    if (res->argc() == 1)
    {
        uint8_t status;
        res->popArg(&status, 1);
        // bool wifiConnected = (status == STATION_GOT_IP);
    }
}

// Callback when MQTT is connected
void ESPLink::mqttConnected(void* response)
{
    char topic[40];
    sprintf(topic, "%S%s", CONTROL_TOPIC, "#");
    ESPLink::getInstance().mELClientMqtt->subscribe(topic);
}

// Callback when MQTT is disconnected
void ESPLink::mqttDisconnected(void* response)
{
}

// Callback when an MQTT message arrives for one of our subscriptions
void ESPLink::mqttData(void* response)
{
    if (getInstance().mCallback == nullptr)
    {
        return;
    }

    const int max_topic_length = 40;
    char      topic[max_topic_length];
    const int max_value_length = 10;
    char      value[max_value_length];

    ELClientResponse* res = (ELClientResponse*) response;
    if ((res->argLen() + 1) > max_topic_length)
    {
        return;
    }
    res->popChar(topic);

    if ((res->argLen() + 1) > max_value_length)
    {
        return;
    }
    res->popChar(value);

    if (strstr_P(topic, OPERATION_MODE) != nullptr)
    {
        if (strstr_P(value, ENUM_OPERATION_MODE_ABSENCE) != nullptr)
        {
            getInstance().mCallback->onOperationModeChanged(HMIOperationMode::ABSENCE);
        }

        else if (strstr_P(value, ENUM_OPERATION_MODE_BOOST) != nullptr)
        {
            getInstance().mCallback->onOperationModeChanged(HMIOperationMode::BOOST);
        }

        else if (strstr_P(value, ENUM_OPERATION_MODE_AUTO) != nullptr)
        {
            getInstance().mCallback->onOperationModeChanged(HMIOperationMode::AUTO);
        }

        else if (strstr_P(value, ENUM_OPERATION_MODE_ECO_ON) != nullptr)
        {
            getInstance().mCallback->onOperationModeChanged(HMIOperationMode::ECO_ACTIVE);
        }

        else if (strstr_P(value, ENUM_OPERATION_MODE_ECO_OFF) != nullptr)
        {
            getInstance().mCallback->onOperationModeChanged(HMIOperationMode::ECO_INACTIVE);
        }
    }

    else if (strstr_P(topic, HOT_WATER_TEMP_TARGET) != nullptr)
    {
        getInstance().mCallback->onWaterTempTargetChanged(atof(value));
    }
}

void ESPLink::loop()
{
    updateInternalClock();

    mElClient->Process();
}

void ESPLink::updateInternalClock()
{
    unsigned long now = millis();
    if (timeStatus() != timeSet || (now - mLastNTPSync) > 120000)
    {
        uint32_t time = mELClientCmd->GetTime();
        if (time != 0)
        {
            setTime(time);
            mLastNTPSync = now;
        }
    }
}
