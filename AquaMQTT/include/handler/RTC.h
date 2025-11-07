#ifndef AQUAMQTT_RTC_H
#define AQUAMQTT_RTC_H

#include "RTClib.h"

namespace aquamqtt
{
class RTCHandler
{
public:
    RTCHandler();

    virtual ~RTCHandler() = default;

    void setup();

    void loop();

private:
    unsigned long mLastNTPUpdate;
    unsigned long mLastStateUpdate;
    bool          mFoundRTC;
    RTC_DS3231    mRTC;
};
}  // namespace aquamqtt

#endif  // AQUAMQTT_RTC_H
