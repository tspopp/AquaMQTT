#include "state/Optitronic2State.h"

using namespace aquamqtt::message::optitronic2;

namespace aquamqtt
{

Optitronic2State& Optitronic2State::getInstance()
{
    static Optitronic2State instance;
    return instance;
}

Optitronic2State::Optitronic2State()
    : mRegisters{}
    , mRegisterValid{}
    , mNotify(nullptr)
    , mMutex(xSemaphoreCreateMutex())
    , mChangeFlags(0)
    , mStats{ 0, 0, 0, 0 }
{
    memset(mRegisterValid, 0, sizeof(mRegisterValid));
}

void Optitronic2State::setListener(const TaskHandle_t handle)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }
    mNotify = handle;
    xSemaphoreGive(mMutex);
}

bool Optitronic2State::storeRegisters(uint16_t startReg, const uint16_t* values, uint16_t count)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return false;
    }

    bool anyChanged = false;
    uint32_t changeFlag = 0;

    for (uint16_t i = 0; i < count; i++)
    {
        uint16_t addr = startReg + i;
        if (addr >= MAX_REGISTER_ADDR)
        {
            break;
        }

        if (!mRegisterValid[addr] || mRegisters[addr] != values[i])
        {
            mRegisters[addr]      = values[i];
            mRegisterValid[addr]  = true;
            anyChanged            = true;
        }
    }

    if (anyChanged)
    {
        // Determine which change category this belongs to
        if (startReg == REG_BLOCK2_START)
        {
            changeFlag = CHANGE_SENSORS;
        }
        else if (startReg == REG_BLOCK1_START)
        {
            changeFlag = CHANGE_SETTINGS | CHANGE_STATE;
        }
        else if (startReg == REG_INSTALLER_BLK1_START || startReg == REG_INSTALLER_BLK2_START)
        {
            changeFlag = CHANGE_INSTALLER;
        }
        else if (startReg == REG_SCHEDULE_DHW_START || startReg == REG_SCHEDULE_VENT_START)
        {
            changeFlag = CHANGE_SCHEDULE;
        }
        else
        {
            changeFlag = CHANGE_STATE;
        }

        mChangeFlags |= changeFlag;
        mStats.registerUpdates++;
        mStats.lastUpdateMs = millis();
    }

    xSemaphoreGive(mMutex);

    if (anyChanged)
    {
        notifyListener(changeFlag);
    }

    return anyChanged;
}

bool Optitronic2State::storeSingleRegister(uint16_t reg, uint16_t value)
{
    uint16_t val = value;
    return storeRegisters(reg, &val, 1);
}

bool Optitronic2State::getRegister(uint16_t reg, uint16_t& value) const
{
    if (reg >= MAX_REGISTER_ADDR)
    {
        return false;
    }
    if (!mRegisterValid[reg])
    {
        return false;
    }
    value = mRegisters[reg];
    return true;
}

bool Optitronic2State::hasBlock(uint16_t startReg) const
{
    if (startReg >= MAX_REGISTER_ADDR)
    {
        return false;
    }
    return mRegisterValid[startReg];
}

// --- Typed accessors ---

float Optitronic2State::getWaterTemp() const
{
    uint16_t raw;
    if (getRegister(REG_WATER_TEMP, raw))
    {
        return regToTemp(raw);
    }
    return 0.0f;
}

float Optitronic2State::getAmbientTemp() const
{
    uint16_t raw;
    if (getRegister(REG_AMBIENT_TEMP, raw))
    {
        return regToTemp(raw);
    }
    return 0.0f;
}

float Optitronic2State::getEvaporatorTemp() const
{
    uint16_t raw;
    if (getRegister(REG_EVAPORATOR_TEMP, raw))
    {
        return regToTemp(raw);
    }
    return 0.0f;
}

float Optitronic2State::getDhwSetpoint() const
{
    uint16_t raw;
    if (getRegister(REG_DHW_SETPOINT, raw))
    {
        return regToTemp(raw);
    }
    return 0.0f;
}

float Optitronic2State::getActiveSetpoint() const
{
    uint16_t raw;
    if (getRegister(REG_ACTIVE_SETPOINT, raw))
    {
        return regToTemp(raw);
    }
    return 0.0f;
}

float Optitronic2State::getEcoDeviation() const
{
    uint16_t raw;
    if (getRegister(REG_ECO_DEVIATION, raw))
    {
        return regToTemp(raw);
    }
    return 0.0f;
}

float Optitronic2State::getKomfortDeviation() const
{
    uint16_t raw;
    if (getRegister(REG_KOMFORT_DEVIATION, raw))
    {
        return regToTemp(raw);
    }
    return 0.0f;
}

uint16_t Optitronic2State::getProgram() const
{
    uint16_t raw;
    if (getRegister(REG_PROGRAM, raw))
    {
        return raw;
    }
    return 0;
}

uint16_t Optitronic2State::getAuxHeatMode() const
{
    uint16_t raw;
    if (getRegister(REG_AUX_HEAT_MODE, raw))
    {
        return raw;
    }
    return 0;
}

uint16_t Optitronic2State::getExtInputFunction() const
{
    uint16_t raw;
    if (getRegister(REG_EXT_INPUT_FUNCTION, raw))
    {
        return raw;
    }
    return 0;
}

uint16_t Optitronic2State::getOperatingState() const
{
    uint16_t raw;
    if (getRegister(REG_OPERATING_STATE, raw))
    {
        return raw;
    }
    return 0;
}

bool Optitronic2State::isPvActive() const
{
    uint16_t raw;
    if (getRegister(REG_COMPRESSOR_STATUS, raw))
    {
        return raw == COMP_PV_ACTIVE;
    }
    return false;
}

uint16_t Optitronic2State::getHeatSource() const
{
    uint16_t raw;
    if (getRegister(REG_HEAT_SOURCE_ACTIVE, raw))
    {
        return raw;
    }
    return 0;
}

uint16_t Optitronic2State::getCompressorStatus() const
{
    uint16_t raw;
    if (getRegister(REG_COMPRESSOR_STATUS, raw))
    {
        return raw;
    }
    return 0;
}

bool Optitronic2State::isQuickHeatActive() const
{
    uint16_t raw;
    if (getRegister(REG_QUICK_HEAT_STATE, raw))
    {
        return message::optitronic2::isQuickHeatActive(raw);
    }
    return false;
}

float Optitronic2State::getQuickHeatTarget() const
{
    uint16_t raw;
    if (getRegister(REG_QUICK_HEAT_STATE, raw))
    {
        return regToTemp(message::optitronic2::getQuickHeatTarget(raw));
    }
    return 0.0f;
}

bool Optitronic2State::isForceHeating() const
{
    uint16_t raw;
    if (getRegister(REG_FORCE_HEATING, raw))
    {
        return raw != 0;
    }
    return false;
}

float Optitronic2State::getFrostProtectTemp() const
{
    uint16_t raw;
    if (getRegister(REG_FROST_PROTECT_TEMP, raw))
    {
        return regToTemp(raw);
    }
    return 0.0f;
}

float Optitronic2State::getBivalentThreshold() const
{
    uint16_t raw;
    if (getRegister(REG_BIVALENT_THRESHOLD, raw))
    {
        return regToTemp(raw);
    }
    return 0.0f;
}

float Optitronic2State::getPvTargetSetpoint() const
{
    uint16_t raw;
    if (getRegister(REG_PV_TARGET_SETPOINT, raw))
    {
        return regToTemp(raw);
    }
    return 0.0f;
}

float Optitronic2State::getExtSourceMaxTemp() const
{
    uint16_t raw;
    if (getRegister(REG_EXT_SOURCE_MAX_TEMP, raw))
    {
        return regToTemp(raw);
    }
    return 0.0f;
}

uint16_t Optitronic2State::getAntiLegioInterval() const
{
    uint16_t raw;
    if (getRegister(REG_ANTI_LEGIO_INTERVAL, raw))
    {
        return raw;
    }
    return 0;
}

uint16_t Optitronic2State::getExtSourcePriority() const
{
    uint16_t raw;
    if (getRegister(REG_EXT_SOURCE_PRIORITY, raw))
    {
        return raw;
    }
    return 0;
}

// --- Statistics ---

void Optitronic2State::updateStats(uint32_t framesRx, uint32_t crcErr)
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }
    mStats.framesReceived = framesRx;
    mStats.crcErrors      = crcErr;
    xSemaphoreGive(mMutex);
}

Optitronic2State::Stats Optitronic2State::getStats() const
{
    return mStats;
}

// --- Change tracking ---

bool Optitronic2State::hasChanges() const
{
    return mChangeFlags != 0;
}

void Optitronic2State::clearChanges()
{
    if (!xSemaphoreTake(mMutex, portMAX_DELAY))
    {
        return;
    }
    mChangeFlags = 0;
    xSemaphoreGive(mMutex);
}

uint32_t Optitronic2State::getChangeFlags() const
{
    return mChangeFlags;
}

void Optitronic2State::notifyListener(uint32_t changeFlag)
{
    if (mNotify != nullptr)
    {
        xTaskNotifyIndexed(mNotify, 0, changeFlag, eSetBits);
    }
}

}  // namespace aquamqtt
