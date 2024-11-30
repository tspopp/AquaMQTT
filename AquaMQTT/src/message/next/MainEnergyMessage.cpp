#include "message/next/MainEnergyMessage.h"

#include "message/MessageConstants.h"

namespace aquamqtt::message::next
{

MainEnergyMessage::MainEnergyMessage(uint8_t* data, uint8_t* previous)
    : mData(data)
    , mHasChangedU16()
    , mHasChangedU32()
    , mHasChangedU64()
    , mHasChangedI8()
{
    mCreatedWithoutPrevious = (previous == nullptr);
    compareWith(previous);
}

void MainEnergyMessage::compareWith(uint8_t* data)
{
    if (data == nullptr)
    {
        return;
    }

    uint8_t diffIndices[ENERGY_MESSAGE_LENGTH_NEXT] = { 0 };
    size_t  numDiffs                                = 0;
    compareBuffers(mData, data, ENERGY_MESSAGE_LENGTH_NEXT, diffIndices, &numDiffs);

    for (int i = 0; i < numDiffs; ++i)
    {
        auto indiceChanged = diffIndices[i];

        switch (indiceChanged)
        {
            case 9:
            case 10:
            case 11:
            case 12:
                mHasChangedU32.insert(ENERGY_ATTR_U32::TOTAL_HEATING_ELEMENT_HOURS);
                break;
            case 13:
            case 14:
            case 15:
            case 16:
                mHasChangedU32.insert(ENERGY_ATTR_U32::TOTAL_HEATPUMP_HOURS);
                break;
            case 17:
            case 18:
            case 19:
            case 20:
                mHasChangedU64.insert(ENERGY_ATTR_U64::TOTAL_ENERGY);
                break;
            case 21:
            case 22:
                mHasChangedU16.insert(ENERGY_ATTR_U16::POWER_HEATPUMP);
                break;
            case 38:
                mHasChangedI8.insert(ENERGY_ATTR_I8::DIAG_AIR_TEMP_MAX);
                break;
            case 37:
                mHasChangedI8.insert(ENERGY_ATTR_I8::DIAG_AIR_TEMP_MIN);
                break;
            case 36:
                mHasChangedI8.insert(ENERGY_ATTR_I8::DIAG_EVA_UPPER_AIR_TEMP_MAX);
                break;
            case 35:
                mHasChangedI8.insert(ENERGY_ATTR_I8::DIAG_EVA_UPPER_AIR_TEMP_MIN);
                break;
            case 34:
                mHasChangedI8.insert(ENERGY_ATTR_I8::DIAG_EVA_LOWER_AIR_TEMP_MAX);
                break;
            case 33:
                mHasChangedI8.insert(ENERGY_ATTR_I8::DIAG_EVA_LOWER_AIR_TEMP_MIN);
                break;
            case 32:
                mHasChangedI8.insert(ENERGY_ATTR_I8::DIAG_COMPRESSOR_TEMP_MAX);
                break;
            case 31:
                mHasChangedI8.insert(ENERGY_ATTR_I8::DIAG_COMPRESSOR_TEMP_MIN);
                break;
            case 30:
                mHasChangedI8.insert(ENERGY_ATTR_I8::DIAG_WATER_TEMP_MAX);
                break;
            case 29:
                mHasChangedI8.insert(ENERGY_ATTR_I8::DIAG_WATER_TEMP_MIN);
                break;
            default:
                break;
        }
    }
}

uint8_t MainEnergyMessage::getLength()
{
    return ENERGY_MESSAGE_LENGTH_NEXT;
}

uint64_t MainEnergyMessage::getAttr(ENERGY_ATTR_U64 attr)
{
    switch (attr)
    {
        case ENERGY_ATTR_U64::TOTAL_ENERGY:
            // NEXT protocol has this attribute stored as u32
            return ((uint32_t) mData[20] << 24) | ((uint32_t) mData[19] << 16) | ((uint32_t) mData[18] << 8)
                   | (uint32_t) mData[17];
    }
    return 0;
}

uint32_t MainEnergyMessage::getAttr(ENERGY_ATTR_U32 attr)
{
    switch (attr)
    {
        case ENERGY_ATTR_U32::TOTAL_HEATPUMP_HOURS:
            return ((uint32_t) mData[16] << 24) | ((uint32_t) mData[15] << 16) | ((uint32_t) mData[14] << 8)
                   | (uint32_t) mData[13];
        case ENERGY_ATTR_U32::TOTAL_HEATING_ELEMENT_HOURS:
            return ((uint32_t) mData[12] << 24) | ((uint32_t) mData[11] << 16) | ((uint32_t) mData[10] << 8)
                   | (uint32_t) mData[9];
        case ENERGY_ATTR_U32::TOTAL_HOURS:
            break;
    }
    return 0;
}

uint16_t MainEnergyMessage::getAttr(ENERGY_ATTR_U16 attr)
{
    switch (attr)
    {
        case ENERGY_ATTR_U16::POWER_HEATPUMP:
            return ((uint16_t) mData[22] << 8) | (uint16_t) mData[21];
        case ENERGY_ATTR_U16::POWER_HEATELEMENT:
        case ENERGY_ATTR_U16::POWER_TOTAL:
        case ENERGY_ATTR_U16::WATER_TOTAL:
            break;
    }
    return 0;
}

int8_t MainEnergyMessage::getAttr(ENERGY_ATTR_I8 attr)
{
    switch (attr)
    {
        case ENERGY_ATTR_I8::DIAG_AIR_TEMP_MAX:
            return static_cast<int8_t>(mData[38]);
        case ENERGY_ATTR_I8::DIAG_AIR_TEMP_MIN:
            return static_cast<int8_t>(mData[37]);
        case ENERGY_ATTR_I8::DIAG_EVA_UPPER_AIR_TEMP_MAX:
            return static_cast<int8_t>(mData[36]);
        case ENERGY_ATTR_I8::DIAG_EVA_UPPER_AIR_TEMP_MIN:
            return static_cast<int8_t>(mData[35]);
        case ENERGY_ATTR_I8::DIAG_EVA_LOWER_AIR_TEMP_MAX:
            return static_cast<int8_t>(mData[34]);
        case ENERGY_ATTR_I8::DIAG_EVA_LOWER_AIR_TEMP_MIN:
            return static_cast<int8_t>(mData[33]);
        case ENERGY_ATTR_I8::DIAG_COMPRESSOR_TEMP_MAX:
            return static_cast<int8_t>(mData[32]);
        case ENERGY_ATTR_I8::DIAG_COMPRESSOR_TEMP_MIN:
            return static_cast<int8_t>(mData[31]);
        case ENERGY_ATTR_I8::DIAG_WATER_TEMP_MAX:
            return static_cast<int8_t>(mData[30]);
        case ENERGY_ATTR_I8::DIAG_WATER_TEMP_MIN:
            return static_cast<int8_t>(mData[29]);
    }
    return 0;
}

bool MainEnergyMessage::hasAttr(ENERGY_ATTR_U64 attr) const
{
    switch (attr)
    {
        case ENERGY_ATTR_U64::TOTAL_ENERGY:
            return true;
    }
    return false;
}

bool MainEnergyMessage::hasAttr(ENERGY_ATTR_U32 attr) const
{
    switch (attr)
    {
        case ENERGY_ATTR_U32::TOTAL_HEATPUMP_HOURS:
        case ENERGY_ATTR_U32::TOTAL_HEATING_ELEMENT_HOURS:
            return true;
        case ENERGY_ATTR_U32::TOTAL_HOURS:
            break;
    }
    return false;
}

bool MainEnergyMessage::hasAttr(ENERGY_ATTR_U16 attr) const
{
    switch (attr)
    {
        case ENERGY_ATTR_U16::POWER_TOTAL:
            return true;
        case ENERGY_ATTR_U16::POWER_HEATPUMP:
        case ENERGY_ATTR_U16::POWER_HEATELEMENT:
        case ENERGY_ATTR_U16::WATER_TOTAL:
            break;
    }
    return false;
}

bool MainEnergyMessage::hasAttr(ENERGY_ATTR_I8 attr) const
{
    switch (attr)
    {
        case ENERGY_ATTR_I8::DIAG_AIR_TEMP_MAX:
        case ENERGY_ATTR_I8::DIAG_AIR_TEMP_MIN:
        case ENERGY_ATTR_I8::DIAG_EVA_UPPER_AIR_TEMP_MAX:
        case ENERGY_ATTR_I8::DIAG_EVA_UPPER_AIR_TEMP_MIN:
        case ENERGY_ATTR_I8::DIAG_EVA_LOWER_AIR_TEMP_MAX:
        case ENERGY_ATTR_I8::DIAG_EVA_LOWER_AIR_TEMP_MIN:
        case ENERGY_ATTR_I8::DIAG_COMPRESSOR_TEMP_MAX:
        case ENERGY_ATTR_I8::DIAG_COMPRESSOR_TEMP_MIN:
        case ENERGY_ATTR_I8::DIAG_WATER_TEMP_MAX:
        case ENERGY_ATTR_I8::DIAG_WATER_TEMP_MIN:
            return true;
    }
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
    return mCreatedWithoutPrevious || mHasChangedI8.find(attr) != mHasChangedI8.end();
}

}  // namespace aquamqtt::message::next
