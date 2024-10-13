#include "handler/RTC.h"

#include <TimeLib.h>

#include "config/Configuration.h"
#include "state/HMIStateProxy.h"

namespace aquamqtt
{

RTCHandler::RTCHandler() : mRTC(), mFoundRTC(false), mLastNTPUpdate(0), mLastStateUpdate(0)
{
}

void RTCHandler::setup()
{
    configTzTime(aquamqtt::config::timeZoneInfo, aquamqtt::config::ntpServer);
    Wire.begin(aquamqtt::config::GPIO_SDA_RTC, aquamqtt::config::GPIO_SCL_RTC);
    mFoundRTC = mRTC.begin(&Wire);
}

void RTCHandler::loop()
{
    // do a ntp update if case time is not set or last ntp update is an hour ago
    if (timeStatus() != timeSet || (millis() - mLastNTPUpdate) >= 3600000)
    {
        mLastNTPUpdate = millis();
        struct tm timeinfo
        {
        };

        // success getting time via ntp
        if (getLocalTime(&timeinfo, 5000))
        {
            // set local time in lib (PaulStoffregen/Time)
            setTime(timeinfo.tm_hour,
                    timeinfo.tm_min,
                    timeinfo.tm_sec,
                    timeinfo.tm_mday,
                    timeinfo.tm_mon + 1,
                    timeinfo.tm_year + 1900);

            // store the new time to the rtc
            if (mFoundRTC)
            {
                mRTC.adjust(DateTime(now()));
            }
        }
        // ntp update failed, but we have a hardware rtc
        else if (mFoundRTC)
        {
            // set local time in lib from RTC
            setTime(mRTC.now().unixtime());
        }
    }

    // if time is known (either from RTC or NTP), then update the state
    if (timeStatus() == timeSet && config::OVERRIDE_TIME_AND_DATE_IN_MITM && ((millis() - mLastStateUpdate) > 1000))
    {
        mLastStateUpdate  = millis();
        time_t timeUpdate = now();
        aquamqtt::HMIStateProxy::getInstance().updateTime(
                second(timeUpdate),
                minute(timeUpdate),
                hour(timeUpdate),
                day(timeUpdate),
                month(timeUpdate),
                year(timeUpdate));
    }
}

}  // namespace aquamqtt