#include "HMIState.h"

#include <TimeLib.h>

HMIState::HMIState()
    : IMQTTCallback()
    , mTargetTemperature(53.0f)
    , mOperationMode(ABSENCE)
    , mTimerModeEnabled(false)
    , mMessage{
        35,                                                          // LENGTH
        18,  2,                                                      // TARGET TEMP
        66,                                                          // OPERATION MODE
        252, 0,   240, 17, 240, 6,  16, 56,                          // TIMER MODE 1
        0,   0,                                                      // TIMER MODE 2
        255, 255, 0,                                                 // UNKNOWN
        33,                                                          // SECONDS
        147,                                                         // CURRENT DAY, CURRENT MONTH IN HALF YEAR
        46,                                                          // CURRENT YEAR, HALF-YEAR
        11,                                                          // CURRENT_TIME-MINUTES
        11,                                                          // CURRENT_TIME_HOUR
        0,   0,   255, 0,  9,   66, 0,  0,  255, 255, 255, 255, 255  // UNKNOWN
    }
{
}

bool HMIState::updateMessage()
{
    if (timeStatus() != timeSet)
    {
        Serial.println(F("waiting for ntp time"));
        return false;
    }

    // if operation mode is boost, we always go for 62°C!
    float targetTemperature = mOperationMode == BOOST ? 62.0 : mTargetTemperature;
    mMessage[1] = (int) targetTemperature * 10 & 0xFF;
    mMessage[2] = ((int) targetTemperature * 10 >> 8) & 0xFF;

    mMessage[3] = (mMessage[3] & 0xF0) | (mOperationMode & 0x0F);
    mMessage[3] = (mMessage[3] & ~(1 << 6)) | (mTimerModeEnabled << 6);

    // TODO: TimeWindow

    // TODO: Update Docs, this is not halfyear, it should be called past july
    int pastJuly = month() > 7 ? 1 : 0;

    // year
    mMessage[19] = ((year() - 2000) * 2) + pastJuly;
    // day
    mMessage[18] = (mMessage[18] & 0xE0) | (day() & 0x1F);
    // month
    int monthValue = (pastJuly ? month() - 8 : month() - 1) << 5;
    mMessage[18]   = (mMessage[18] & 0x1F) | (monthValue & 0xE0);

    mMessage[17] = second();
    mMessage[20] = minute();
    mMessage[21] = hour();
    return true;
}
void HMIState::onOperationModeChanged(HMIOperationMode value)
{
    mOperationMode = value;
}
void HMIState::onWaterTempTargetChanged(float value)
{
    mTargetTemperature = value;
}