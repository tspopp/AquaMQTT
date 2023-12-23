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
    if (timeStatus() != timeSet)
    {
        if ((millis() - mLastNTPUpdate) >= 3600000)
        {
            mLastNTPUpdate = millis();
            struct tm timeinfo
            {
            };
            if (getLocalTime(&timeinfo, 5000))
            {
                setTime(timeinfo.tm_hour,
                        timeinfo.tm_min,
                        timeinfo.tm_sec,
                        timeinfo.tm_mday,
                        timeinfo.tm_mon + 1,
                        timeinfo.tm_year + 1900);
                if (mFoundRTC)
                {
                    mRTC.adjust(DateTime(now()));
                }
            }
            else if (mFoundRTC)
            {
                setTime(mRTC.now().unixtime());
            }
        }
    }
    else if (config::OVERRIDE_TIME_AND_DATE_IN_MITM && (millis() - mLastStateUpdate) > 500)
    {
        mLastStateUpdate = now();
        aquamqtt::HMIStateProxy::getInstance().updateTime(
                second(mLastStateUpdate),
                minute(mLastStateUpdate),
                hour(mLastStateUpdate),
                day(mLastStateUpdate),
                month(mLastStateUpdate),
                year(mLastStateUpdate));
    }
}

}  // namespace aquamqtt