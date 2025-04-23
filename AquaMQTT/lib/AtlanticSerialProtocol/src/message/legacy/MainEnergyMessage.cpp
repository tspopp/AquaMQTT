#include "message/legacy/MainEnergyMessage.h"

namespace aquamqtt::message::legacy
{

MainEnergyMessage::MainEnergyMessage(uint8_t* data, uint8_t* previous)
    : mData(data)
    , mHasChangedU16()
    , mHasChangedU32()
    , mHasChangedU64()
{
    mCreatedWithoutPrevious = previous == nullptr;
    compareWith(previous);
}

void MainEnergyMessage::compareWith(uint8_t* data)
{
    if (data == nullptr)
    {
        return;
    }

    uint8_t diffIndices[ENERGY_MESSAGE_LENGTH_LEGACY] = { 0 };
    size_t  numDiffs                                  = 0;
    compareBuffers(mData, data, ENERGY_MESSAGE_LENGTH_LEGACY, diffIndices, &numDiffs);

    for (int i = 0; i < numDiffs; ++i)
    {
        auto indiceChanged = diffIndices[i];

        switch (indiceChanged)
        {
            case 1:
            case 2:
                mHasChangedU16.insert(ENERGY_ATTR_U16::POWER_HEATPUMP);
                break;
            case 3:
            case 4:
                mHasChangedU16.insert(ENERGY_ATTR_U16::POWER_HEATELEMENT);
                break;
            case 7:
            case 8:
                mHasChangedU16.insert(ENERGY_ATTR_U16::POWER_TOTAL);
                break;
            case 9:
            case 10:
                mHasChangedU16.insert(ENERGY_ATTR_U16::WATER_TOTAL);
                break;
            case 11:
            case 12:
            case 13:
            case 14:
                mHasChangedU32.insert(ENERGY_ATTR_U32::TOTAL_HEATPUMP_HOURS);
                break;
            case 15:
            case 16:
            case 17:
            case 18:
                mHasChangedU32.insert(ENERGY_ATTR_U32::TOTAL_HEATING_ELEMENT_HOURS);
                break;
            case 19:
            case 20:
            case 21:
            case 22:
                mHasChangedU32.insert(ENERGY_ATTR_U32::TOTAL_HOURS);
                break;
            case 23:
            case 24:
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
            case 30:
                mHasChangedU64.insert(ENERGY_ATTR_U64::TOTAL_ENERGY);
                break;
            default:
                break;
        }
    }
}

uint8_t MainEnergyMessage::getLength()
{
    return ENERGY_MESSAGE_LENGTH_LEGACY;
}

uint64_t MainEnergyMessage::getAttr(ENERGY_ATTR_U64 attr)
{
    switch (attr)
    {
        case ENERGY_ATTR_U64::TOTAL_ENERGY:
            return ((uint64_t) mData[30] << 56) | ((uint64_t) mData[29] << 48) | ((uint64_t) mData[28] << 40)
                   | ((uint64_t) mData[27] << 32) | ((uint64_t) mData[26] << 24) | ((uint64_t) mData[25] << 16)
                   | ((uint64_t) mData[24] << 8) | (uint64_t) mData[23];
    }
    return 0;
}

uint32_t MainEnergyMessage::getAttr(ENERGY_ATTR_U32 attr)
{
    switch (attr)
    {
        case ENERGY_ATTR_U32::TOTAL_HEATPUMP_HOURS:
            return ((uint32_t) mData[14] << 24) | ((uint32_t) mData[13] << 16) | ((uint32_t) mData[12] << 8)
                   | (uint32_t) mData[11];
        case ENERGY_ATTR_U32::TOTAL_HEATING_ELEMENT_HOURS:
            return ((uint32_t) mData[18] << 24) | ((uint32_t) mData[17] << 16) | ((uint32_t) mData[16] << 8)
                   | (uint32_t) mData[15];
        case ENERGY_ATTR_U32::TOTAL_HOURS:
            return ((uint32_t) mData[22] << 24) | ((uint32_t) mData[21] << 16) | ((uint32_t) mData[20] << 8)
                   | (uint32_t) mData[19];
    }
    return 0;
}

uint16_t MainEnergyMessage::getAttr(ENERGY_ATTR_U16 attr)
{
    switch (attr)
    {
        case ENERGY_ATTR_U16::POWER_HEATPUMP:
            return ((uint16_t) mData[2] << 8) | (uint16_t) mData[1];
        case ENERGY_ATTR_U16::POWER_HEATELEMENT:
            return ((uint16_t) mData[4] << 8) | (uint16_t) mData[3];
        case ENERGY_ATTR_U16::POWER_TOTAL:
            return ((uint16_t) mData[8] << 8) | (uint16_t) mData[7];
        case ENERGY_ATTR_U16::WATER_TOTAL:
            return ((uint16_t) mData[10] << 8) | (uint16_t) mData[9];
    }
    return 0;
}

int8_t MainEnergyMessage::getAttr(ENERGY_ATTR_I8 attr)
{
    return 0;
}



bool MainEnergyMessage::hasAttr(ENERGY_ATTR_U64 attr) const
{
    switch (attr)
    {
        case ENERGY_ATTR_U64::TOTAL_ENERGY:
            return true;
        default:
            return false;
    }
}

bool MainEnergyMessage::hasAttr(ENERGY_ATTR_U32 attr) const
{
    switch (attr)
    {
        case ENERGY_ATTR_U32::TOTAL_HEATPUMP_HOURS:
        case ENERGY_ATTR_U32::TOTAL_HEATING_ELEMENT_HOURS:
        case ENERGY_ATTR_U32::TOTAL_HOURS:
            return true;
        default:
            return false;
    }
}

bool MainEnergyMessage::hasAttr(ENERGY_ATTR_U16 attr) const
{
    switch (attr)
    {
        case ENERGY_ATTR_U16::POWER_HEATPUMP:
        case ENERGY_ATTR_U16::POWER_HEATELEMENT:
        case ENERGY_ATTR_U16::POWER_TOTAL:
        case ENERGY_ATTR_U16::WATER_TOTAL:
            return true;
        default:
            return false;
    }
}

bool MainEnergyMessage::hasAttr(ENERGY_ATTR_I8 attr) const
{
    return false;
}

bool MainEnergyMessage::hasChanged(ENERGY_ATTR_U64 attr) const
{
    return mCreatedWithoutPrevious || mHasChangedU64.find(attr) != mHasChangedU64.end();
}

bool MainEnergyMessage::hasChanged(ENERGY_ATTR_U32 attr) const
{
    return mCreatedWithoutPrevious || mHasChangedU32.find(attr) != mHasChangedU32.end();
}

bool MainEnergyMessage::hasChanged(ENERGY_ATTR_U16 attr) const
{
    return mCreatedWithoutPrevious || mHasChangedU16.find(attr) != mHasChangedU16.end();
}

bool MainEnergyMessage::hasChanged(ENERGY_ATTR_I8 attr) const
{
    return false;
}

}  // namespace aquamqtt::message::legacy
