#include "handler/RTC.h"

#include <TimeLib.h>

#include "config/Configuration.h"

namespace aquamqtt
{

RTCHandler::RTCHandler() : mRTC(), mFoundRTC(false), mLastNTPUpdate(0)
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
    if (timeStatus() != timeSet || (millis() - mLastNTPUpdate) >= 3600000)
    {
        mLastNTPUpdate = millis();
        struct tm timeinfo
        {
        };
        if (getLocalTime(&timeinfo, 1000))
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

}  // namespace aquamqtt