#ifndef AQUAMQTT_OPTITRONIC2_REGISTER_MAP_H
#define AQUAMQTT_OPTITRONIC2_REGISTER_MAP_H

#include <cstdint>

namespace aquamqtt::message::optitronic2
{

/**
 * Optitronic 2 Modbus register map — reverse-engineered from Austria Email WPA 450 ECO.
 * Slave address: 0x01, Baud: 57600, 8N1.
 * HMI is master, main controller is slave.
 */

// --- Periodic read blocks (polled by HMI every ~600ms cycle) ---

// Block 1: Settings + Operating State (0x0000 + 25 regs)
constexpr uint16_t REG_BLOCK1_START = 0x0000;
constexpr uint16_t REG_BLOCK1_COUNT = 25;

constexpr uint16_t REG_ACTIVE_SETPOINT     = 0x0000;  // int16 ×0.1°C (effective target, e.g. 450=45.0°C or 700=70.0°C in PV mode)
constexpr uint16_t REG_OPERATING_STATE     = 0x0001;  // enum: 2=idle, 3=heating, 6=PV-boost
constexpr uint16_t REG_OPERATING_SUBSTATE  = 0x0002;  // enum: 0=init, 3=normal
constexpr uint16_t REG_HEAT_SOURCE_ACTIVE  = 0x0005;  // enum: 0=off, 2=heat_pump (1=electric?, 3=both?)
constexpr uint16_t REG_FAN_COMPRESSOR_LVL  = 0x0006;  // enum: 0=off, 3=high (1=low?, 2=med?)
constexpr uint16_t REG_DHW_SETPOINT        = 0x0009;  // int16 ×0.1°C (user setting, e.g. 550=55.0°C)
constexpr uint16_t REG_ECO_DEVIATION       = 0x000A;  // int16 ×0.1°C (signed, e.g. 0xFF9C = -10.0°C)
constexpr uint16_t REG_KOMFORT_DEVIATION   = 0x000B;  // int16 ×0.1°C (e.g. 0x0014 = +2.0°C)
constexpr uint16_t REG_PROGRAM             = 0x000C;  // enum: 1=NORMAL, 2=ECO, 3=KOMFORT, 4=KOMFORT_PLUS
constexpr uint16_t REG_AUX_HEAT_MODE       = 0x000D;  // enum: 1=electric, 2=external, 3=both
constexpr uint16_t REG_FORCE_HEATING       = 0x000F;  // bool: 1=start, 0=cancel (quick heat)
constexpr uint16_t REG_ANTI_LEGIONELLA     = 0x0010;  // bool: 1=trigger, self-clears
constexpr uint16_t REG_EXT_INPUT_FUNCTION  = 0x0011;  // enum: see EXT_INPUT_* below
constexpr uint16_t REG_UNKNOWN_12          = 0x0012;  // always 0x00C8 (200)
constexpr uint16_t REG_SETPOINT_MIRROR     = 0x0013;  // mirrors REG_DHW_SETPOINT
constexpr uint16_t REG_QUICK_HEAT_STATE    = 0x0016;  // int16: bit15=active, lower bits=target ×0.1°C

// Block 2: Sensor Data (0x00C8 + 10 regs)
constexpr uint16_t REG_BLOCK2_START = 0x00C8;
constexpr uint16_t REG_BLOCK2_COUNT = 10;

constexpr uint16_t REG_WATER_TEMP          = 0x00C8;  // int16 ×0.1°C (tank top)
constexpr uint16_t REG_SENSOR_UNKNOWN_C9   = 0x00C9;  // 0xF334 — pressure transducer?
constexpr uint16_t REG_AMBIENT_TEMP        = 0x00CA;  // int16 ×0.1°C (intake/ambient air)
constexpr uint16_t REG_EVAPORATOR_TEMP     = 0x00CB;  // int16 ×0.1°C (evaporator/collector)
constexpr uint16_t REG_COMPRESSOR_STATUS   = 0x00D1;  // enum: 0=none, 2=demand-triggered, 4=PV-signal-triggered (latched)

// Block 3: Status Flags (0x00E1 + 6 regs)
constexpr uint16_t REG_BLOCK3_START = 0x00E1;
constexpr uint16_t REG_BLOCK3_COUNT = 6;

// Block 4: Unknown (0x0197 + 1 reg)
constexpr uint16_t REG_BLOCK4_START = 0x0197;
constexpr uint16_t REG_BLOCK4_COUNT = 1;

// Block 5: Unknown (0x0320 + 1 reg)
constexpr uint16_t REG_BLOCK5_START = 0x0320;
constexpr uint16_t REG_BLOCK5_COUNT = 1;

// --- On-demand read blocks ---

// DHW Schedule (0x0032 + 42 regs) — triggered by schedule menu
constexpr uint16_t REG_SCHEDULE_DHW_START = 0x0032;
constexpr uint16_t REG_SCHEDULE_DHW_COUNT = 42;

// Installer Block 1 (0x015E + 25 regs) — triggered by installer PIN entry
constexpr uint16_t REG_INSTALLER_BLK1_START = 0x015E;
constexpr uint16_t REG_INSTALLER_BLK1_COUNT = 25;

// Installer Block 2 (0x0190 + 25 regs)
constexpr uint16_t REG_INSTALLER_BLK2_START = 0x0190;
constexpr uint16_t REG_INSTALLER_BLK2_COUNT = 25;

// Ventilation Schedule (0x01C2 + 42 regs) — installer only
constexpr uint16_t REG_SCHEDULE_VENT_START = 0x01C2;
constexpr uint16_t REG_SCHEDULE_VENT_COUNT = 42;

// --- Installer settings (writable via fn=0x06) ---

constexpr uint16_t REG_ANTI_LEGIO_INTERVAL   = 0x015E;  // days (0=OFF)
constexpr uint16_t REG_ANTI_LEGIO_HYSTERESIS = 0x015F;  // ×0.1°C
constexpr uint16_t REG_FROST_PROTECT_TEMP    = 0x0160;  // ×0.1°C (default 70 = 7.0°C)
constexpr uint16_t REG_EXT_SOURCE_PRIORITY   = 0x0163;  // enum: 0=device, 1=external
constexpr uint16_t REG_PV_TARGET_SETPOINT    = 0x019F;  // ×0.1°C (default 700 = 70.0°C)
constexpr uint16_t REG_BIVALENT_THRESHOLD    = 0x01A0;  // ×0.1°C (default 30 = 3.0°C)
constexpr uint16_t REG_EXT_SOURCE_MAX_TEMP   = 0x01A3;  // ×0.1°C (default 600 = 60.0°C)

// --- Modification log triplet (written with fn=0x10 to 0x00DE-0x00E0) ---

constexpr uint16_t REG_RTC_MINUTE_COUNTER = 0x00DE;  // monotonic, high byte 0x82xx
constexpr uint16_t REG_LOG_MAGIC          = 0x00DF;  // always 0x34AE
constexpr uint16_t REG_LOG_SEQUENCE       = 0x00E0;  // wraps at ~60

// --- Enums ---

enum Optitronic2Program : uint16_t
{
    PROGRAM_NORMAL       = 1,
    PROGRAM_ECO          = 2,
    PROGRAM_KOMFORT      = 3,
    PROGRAM_KOMFORT_PLUS = 4,
};

enum Optitronic2AuxHeatMode : uint16_t
{
    AUX_HEAT_ELECTRIC = 1,
    AUX_HEAT_EXTERNAL = 2,
    AUX_HEAT_BOTH     = 3,
};

enum Optitronic2ExtInputFunction : uint16_t
{
    EXT_INPUT_QUICK_HEAT    = 0,
    EXT_INPUT_OFF           = 1,
    EXT_INPUT_NORMAL        = 2,
    EXT_INPUT_ECO           = 3,
    EXT_INPUT_KOMFORT       = 4,
    EXT_INPUT_KOMFORT_PLUS  = 5,
    EXT_INPUT_PHOTOVOLTAIK  = 6,
    EXT_INPUT_RESERVE       = 7,
    EXT_INPUT_FUNCTION_1    = 8,
};

enum Optitronic2OperatingState : uint16_t
{
    STATE_OFF              = 0,
    STATE_STANDBY          = 1,
    STATE_IDLE             = 2,
    STATE_HEATING          = 3,
    STATE_HEATING_ELECTRIC = 4,
    STATE_HEATING_BOTH     = 5,
    STATE_PV_BOOST         = 6,
    STATE_DEFROST          = 7,
    STATE_ANTI_LEGIONELLA  = 8,
};

enum Optitronic2HeatSource : uint16_t
{
    HEAT_SRC_OFF       = 0,
    HEAT_SRC_ELECTRIC  = 1,
    HEAT_SRC_HEATPUMP  = 2,
    HEAT_SRC_BOTH      = 3,
};

enum Optitronic2CompressorStatus : uint16_t
{
    COMP_IDLE          = 0,
    COMP_RUNNING       = 2,
    COMP_PV_ACTIVE     = 4,
};

// Schedule encoding: bits 15:13 = type (001=start, 111=end), bits 6:0 = quarter-hours (0-95 = 00:00-23:45)
// 0xFFFF = disabled slot. 7 days × 6 regs (3 start/end pairs per day).
constexpr uint16_t SCHED_FLAG_START = 0x2000;
constexpr uint16_t SCHED_FLAG_END   = 0xE000;
constexpr uint16_t SCHED_TIME_MASK  = 0x007F;
constexpr uint16_t SCHED_DISABLED   = 0xFFFF;

// Quick-heat register bit manipulation
constexpr uint16_t QUICK_HEAT_ACTIVATE_BIT = 0x8000;

inline bool isQuickHeatActive(uint16_t regValue)
{
    return (regValue & QUICK_HEAT_ACTIVATE_BIT) != 0;
}

inline uint16_t getQuickHeatTarget(uint16_t regValue)
{
    return regValue & 0x7FFF;
}

inline uint16_t makeQuickHeatActivate(uint16_t targetRaw)
{
    return targetRaw | QUICK_HEAT_ACTIVATE_BIT;
}

// Temperature encoding helpers
inline float regToTemp(uint16_t raw)
{
    return static_cast<int16_t>(raw) * 0.1f;
}

inline uint16_t tempToReg(float temp)
{
    return static_cast<uint16_t>(static_cast<int16_t>(temp * 10.0f));
}

}  // namespace aquamqtt::message::optitronic2

#endif  // AQUAMQTT_OPTITRONIC2_REGISTER_MAP_H
