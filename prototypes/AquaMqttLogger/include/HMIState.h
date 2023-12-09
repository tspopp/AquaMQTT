#ifndef UNTITLED1_STATEHMI_H
#define UNTITLED1_STATEHMI_H

#include <Arduino.h>

#include "MQTTDefinitions.h"

class HMIState : public IMQTTCallback
{
public:
    HMIState();

    virtual ~HMIState() = default;

    bool updateMessage();

    uint8_t* getMessage()
    {
        return mMessage;
    }

    void onOperationModeChanged(HMIOperationMode value) override;

    void onWaterTempTargetChanged(float value) override;

private:
    float            mTargetTemperature;
    HMIOperationMode mOperationMode;
    bool             mTimerModeEnabled;

    uint8_t mMessage[35];
};

#endif  // UNTITLED1_STATEHMI_H
