#ifndef AQUAMQTT_HMIMESSAGE_H
#define AQUAMQTT_HMIMESSAGE_H

#include <cstring>

namespace aquamqtt
{
namespace message
{
enum HMIOperationMode : uint8_t
{
    ABSENCE      = 0,
    ECO_ACTIVE   = 1,
    ECO_INACTIVE = 2,
    BOOST        = 3,
    AUTO         = 4
};

// 0 == Umluft
// 16 == 1 Kanal Luftanschluss
// 32 == 2 Kanal Luftamschluss
enum HMIAirDuctConfig : uint8_t
{
    IN_IN   = 0,
    EXT_IN  = 16,
    EXT_EXT = 32
};

enum HMITestMode : uint8_t
{
    OFF          = 0,
    ENTERED      = 1,
    HEAT_PUMP    = 2,
    HEAT_ELEMENT = 3,
    FAN_LOW      = 4,
    FAN_HIGH     = 5
};

class HMIMessage
{
public:
    explicit HMIMessage(uint8_t* data) : mData(data)
    {
    }

    ~HMIMessage() = default;

    float waterTempTarget()
    {
        return (float) (((int16_t) mData[2] << 8) | mData[1]) / 10.0;
    }

    // TODO: sanity we should not allow more than 62°C
    void setWaterTempTarget(float targetTemperature)
    {
        mData[1] = (int) targetTemperature * 10 & 0xFF;
        mData[2] = ((int) targetTemperature * 10 >> 8) & 0xFF;
    }

    HMIOperationMode operationMode()
    {
        return HMIOperationMode(mData[3] & 0x0F);
    }

    void setOperationMode(HMIOperationMode operationMode)
    {
        mData[3] = (mData[3] & 0xF0) | (operationMode & 0x0F);
    }

    bool isTimerModeEnabled()
    {
        return mData[3] & 0x40;
    }

    void setTimerModeEnabled(boolean enabled)
    {
        mData[3] = (mData[3] & ~(1 << 6)) | (enabled << 6);
    }

    // TODO: sanity you cannot active emergency mode if heating element is disabled
    bool isEmergencyModeEnabled()
    {
        return mData[6] & 0x01;
    }

    // TODO: you cannot disable heating elment if emergency mode is activated
    bool isHeatingElementEnabled()
    {
        return mData[9] & 0x04;
    }

    // TODO: implement
    bool setupModeA()
    {
        // Fresh-Reset = 164 / 1010 0100? (nothing set)
        return mData[9] & 0x80;
    };

    // TODO: implement
    bool setupModeB()
    {
        // Then 36             0010 0100 (umluftbetrieb set, rest missing
        return mData[9] & 0x20;
    }

    uint8_t antiLegionellaModePerMonth()
    {
        return (uint8_t) (mData[5] & 0x0F);
    };

    // TODO: enum, naming
    uint8_t airFlow()
    {
        // 0 == Umluft
        // 16 == 1 Kanal Luftanschluss
        // 32 == 2 Kanal Luftamschluss
        return (uint8_t) (mData[5] & 0xF0);
    };

    // TODO: enum, naming
    uint16_t installationOptions()
    {

        //| WT, WP-Only:                        0000 0000,
        // WP+Heizkessel-Prio-Wärmepumpe:       0000 0001,
        // Wp+Heizkessel-Opt-Wärmepumpe: 17 /   0001 0001,
        // Wp+Heizkelsse-Opt-Heizkessel: 33 /   0010 0001,
        // Wp+Heizkessel-Prio-Heizkessel: 49 // 0011 0001,
        // Wp+Solarthermie: 50 //               0011 0010|

        return mData[17];
    }

    // TODO: enum, naming
    uint16_t testMode()
    {
        // HMI Left TestMode = 0
        // HMI Entered TestMode = 1
        // WP TestMode Running = 2;
        // Heating-Element = 3
        // Fan TestMode Running (slow) = 4
        // Fan TestMode Running (fast) = 5
        // Defrost = 6,
        return mData[22];
    }

    // TODO: sanity, timer window cannot exceed 14 hours (both a and b)
    // 1st range duration: 4 hours < time < 14 hours;
    // Total duration of the 2 ranges: 8 hours minimum and 14 hours maximum.
    // FIXME: do not return string
    std::string timerWindowStr(bool firstWindow)
    {
        uint16_t start    = firstWindow ? timerWindowAStart() : timerWindowBStart();
        uint16_t duration = firstWindow ? timerWindowALength() : timerWindowBLength();

        char          beginStr[13];
        const uint8_t beginHours    = (start * 15) / 60;
        const uint8_t beginMinutes  = (start * 15) % 60;
        const uint8_t lengthHours   = (duration * 15) / 60;
        const uint8_t lengthMinutes = (duration * 15) % 60;
        const uint8_t endMinutes    = (beginMinutes + lengthMinutes) % 60;
        const uint8_t hourOverlap   = (beginMinutes + lengthMinutes) / 60;
        const uint8_t endHours      = (beginHours + lengthHours) % 24 + hourOverlap;
        sprintf(beginStr, "%02d:%02d-%02d:%02d", beginHours, beginMinutes, endHours, endMinutes);
        return beginStr;
    }

    uint16_t timerWindowAStart()
    {
        return mData[10];
    };

    uint16_t timerWindowALength()
    {
        return mData[11];
    };

    uint16_t timerWindowBStart()
    {
        return mData[12];
    };

    uint16_t timerWindowBLength()
    {
        return mData[13];
    };

    uint8_t timeHours()
    {
        return mData[21];
    }

    void setTimeHours(uint8_t hour)
    {
        mData[21] = hour;
    }

    uint8_t timeMinutes()
    {
        return mData[20];
    }

    void setTimeMinutes(uint8_t minute)
    {
        mData[20] = minute;
    }

    uint8_t timeSeconds()
    {
        return mData[17];
    }

    void setTimeSeconds(uint8_t second)
    {
        mData[17] = second;
    }

    uint16_t dateYear()
    {
        return 2000 + (mData[19] / 2);
    }

    void setDateMonthAndYear(uint8_t month, uint16_t year)
    {
        int pastJuly   = month > 7 ? 1 : 0;
        mData[19]      = ((year - 2000) * 2) + pastJuly;
        int monthValue = (pastJuly ? month - 8 : month) << 5;
        mData[18]      = (mData[18] & 0x1F) | (monthValue & 0xE0);
    }

    uint8_t dateMonth()
    {
        return (mData[18] >> 5) + ((mData[19] % 2) * 8);
    }

    uint8_t dateDay()
    {
        return mData[18] & 0x1F;
    }

    void setDateDay(uint8_t day)
    {
        mData[18] = (mData[18] & 0xE0) | (day & 0x1F);
    };

private:
    uint8_t* mData;
};

}  // namespace message
}  // namespace aquamqtt

#endif  // AQUAMQTT_HMIMESSAGE_H
