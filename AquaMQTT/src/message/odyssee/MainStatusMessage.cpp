#include "message/odyssee/MainStatusMessage.h"

namespace aquamqtt::message::odyssee
{
    MainStatusMessage::MainStatusMessage(uint8_t* data, const uint8_t* previous)
        : mData(data)
          , mHasChangedFloat()
          , mHasChangedBool()
          , mHasChangedU8()
    {
        mCreatedWithoutPrevious = previous == nullptr;
        compareWith(previous);
    }

    void MainStatusMessage::compareWith(const uint8_t* data)
    {
        if (data == nullptr)
        {
            return;
        }

        uint8_t diffIndices[MAIN_MESSAGE_LENGTH_NEXT] = {0};
        size_t numDiffs = 0;
        compareBuffers(mData, data, MAIN_MESSAGE_LENGTH_NEXT, diffIndices, &numDiffs);

        for (int i = 0; i < numDiffs; ++i)
        {
            switch (diffIndices[i])
            {
            case 1:
                mHasChangedFloat.insert(MAIN_ATTR_FLOAT::WATER_TEMPERATURE);
                break;
            case 3:
                mHasChangedFloat.insert(MAIN_ATTR_FLOAT::COMPRESSOR_OUTLET_TEMPERATURE);
                break;
            case 5:
                mHasChangedFloat.insert(MAIN_ATTR_FLOAT::EVAPORATOR_UPPER_TEMPERATURE);
                break;
            case 7:
                mHasChangedFloat.insert(MAIN_ATTR_FLOAT::EVAPORATOR_LOWER_TEMPERATURE);
                break;
            case 9:
                mHasChangedFloat.insert(MAIN_ATTR_FLOAT::AIR_TEMPERATURE);
                break;
            case 13:
                mHasChangedFloat.insert(MAIN_ATTR_FLOAT::WATER_UPPER_TEMPERATURE);
                break;
            case 15:
                mHasChangedFloat.insert(MAIN_ATTR_FLOAT::WATER_LOWER_TEMPERATURE);
                break;
            case 18:
                mHasChangedFloat.insert(MAIN_ATTR_FLOAT::FAN_SPEED_PWM);
                break;
            default:
                break;
            }
        }
    }

    float MainStatusMessage::getAttr(const MAIN_ATTR_FLOAT attr)
    {
        switch (attr)
        {
        case MAIN_ATTR_FLOAT::WATER_TEMPERATURE:
            return static_cast<int8_t>(mData[1]);
        case MAIN_ATTR_FLOAT::AIR_TEMPERATURE:
            return static_cast<int8_t>(mData[9]);
        case MAIN_ATTR_FLOAT::EVAPORATOR_UPPER_TEMPERATURE:
            return static_cast<int8_t>(mData[5]);
        case MAIN_ATTR_FLOAT::EVAPORATOR_LOWER_TEMPERATURE:
            return static_cast<int8_t>(mData[7]);
        case MAIN_ATTR_FLOAT::COMPRESSOR_OUTLET_TEMPERATURE:
            return static_cast<int8_t>(mData[3]);
        case MAIN_ATTR_FLOAT::WATER_UPPER_TEMPERATURE:
            return static_cast<int8_t>(mData[13]);
        case MAIN_ATTR_FLOAT::WATER_LOWER_TEMPERATURE:
            return static_cast<int8_t>(mData[15]);
        case MAIN_ATTR_FLOAT::FAN_SPEED_PWM:
            return mData[18];
        default:
            return 0;
        }
    }

    bool MainStatusMessage::getAttr(MAIN_ATTR_BOOL attr)
    {
        return false;
    }

    uint8_t MainStatusMessage::getAttr(MAIN_ATTR_U8 attr)
    {
        return 0;
    }

    uint16_t MainStatusMessage::getAttr(MAIN_ATTR_U16 attr)
    {
        return 0;
    }

    void MainStatusMessage::setAttr(MAIN_ATTR_FLOAT attr, float value)
    {
    }

    void MainStatusMessage::setAttr(MAIN_ATTR_BOOL attr, bool value)
    {
    }

    void MainStatusMessage::setAttr(MAIN_ATTR_U8 attr, uint8_t value)
    {
    }

    void MainStatusMessage::setAttr(MAIN_ATTR_U16 attr, uint16_t value)
    {
    }

    bool MainStatusMessage::hasAttr(const MAIN_ATTR_FLOAT attr) const
    {
        switch (attr)
        {
        case MAIN_ATTR_FLOAT::WATER_TEMPERATURE:
        case MAIN_ATTR_FLOAT::AIR_TEMPERATURE:
        case MAIN_ATTR_FLOAT::EVAPORATOR_UPPER_TEMPERATURE:
        case MAIN_ATTR_FLOAT::EVAPORATOR_LOWER_TEMPERATURE:
        case MAIN_ATTR_FLOAT::COMPRESSOR_OUTLET_TEMPERATURE:
        case MAIN_ATTR_FLOAT::FAN_SPEED_PWM:
        case MAIN_ATTR_FLOAT::WATER_LOWER_TEMPERATURE:
        case MAIN_ATTR_FLOAT::WATER_UPPER_TEMPERATURE:
            return true;
        default:
            return false;
        }
    }

    bool MainStatusMessage::hasAttr(const MAIN_ATTR_BOOL attr) const
    {
        return false;
    }

    bool MainStatusMessage::hasAttr(const MAIN_ATTR_U8 attr) const
    {
        return false;
    }

    bool MainStatusMessage::hasAttr(const MAIN_ATTR_U16 attr) const
    {
        return false;
    }

    bool MainStatusMessage::hasChanged(const MAIN_ATTR_FLOAT attr) const
    {
        return mCreatedWithoutPrevious || mHasChangedFloat.contains(attr);
    }

    bool MainStatusMessage::hasChanged(const MAIN_ATTR_BOOL attr) const
    {
        return mCreatedWithoutPrevious || mHasChangedBool.contains(attr);
    }

    bool MainStatusMessage::hasChanged(const MAIN_ATTR_U8 attr) const
    {
        return mCreatedWithoutPrevious || mHasChangedU8.contains(attr);
    }

    bool MainStatusMessage::hasChanged(const MAIN_ATTR_U16 attr) const
    {
        return mCreatedWithoutPrevious || mHasChangedU16.contains(attr);
    }

    uint8_t MainStatusMessage::getLength()
    {
        return MAIN_MESSAGE_LENGTH_ODYSSEE;
    }
} // namespace aquamqtt::message::odyssee
