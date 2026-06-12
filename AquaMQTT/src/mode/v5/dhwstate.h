#ifndef AQUAMQTT_DHWSTATE_H
#define AQUAMQTT_DHWSTATE_H

#include <string>
#include <map>
#include <vector>

#include "entity.h"

/**
 * Operation mode governs the I1/I2 boolean input signals.
 *
 * Via the HMI, the unit can be configured to use one of three modes:
 * - I2/I1 disabled
 * - PV installation connected, this enables the use of I1
 * - smart grid installation connected, this enables the use of I1 and I2
 *
 * In Home Assistant, AquaMQTT can be configured to manipulate frames as to
 * modify the I1 and I2 values to set the unit operation mode instead of using
 * the actual I1 and I2 values.
 *
 * Operation mode options:
 * - use input: do not manipulate the frames and use the actual input signals
 * - normal: normal operation (I1 off, I2 off)
 * - eager: recommended heating (I1 on, I2 off)
 * - off: disable heating (I1 off, I2 on)
 * - boost: full power (I1 on, I2 on)
 */
#define OPERATION_MODE_USE_INPUT 0
#define OPERATION_MODE_NORMAL 1
#define OPERATION_MODE_EAGER 2
#define OPERATION_MODE_OFF 3
#define OPERATION_MODE_BOOST 4

#define OPERATION_MODE_USE_INPUT_STR "use input"
#define OPERATION_MODE_NORMAL_STR "normal"
#define OPERATION_MODE_EAGER_STR "eager"
#define OPERATION_MODE_OFF_STR "off"
#define OPERATION_MODE_BOOST_STR "boost"

namespace aquamqtt
{

/**
 * Device state of the DHW unit.
 */
class DhwState final
{
private:
    std::string device_id;
    std::string device_name;

public:
    static DhwState& getInstance();

    DhwState(DhwState const&) = delete;
    void operator=(DhwState const&) = delete;

    inline const char* getDeviceId() const { return device_id.c_str(); };
    inline const char* getDeviceName() const { return device_name.c_str(); };

    TextSensor* serial_number;
    TextSensor* controller_model;
    TextSensor* power_board_version;
    TextSensor* hmi_version;
    TextSensor* hmi_model;

    Sensor *water_temperature, *water_temperature_min, *water_temperature_max;
    Sensor *compressor_outlet_temperature, *compressor_outlet_temperature_min, *compressor_outlet_temperature_max;
    Sensor *air_inlet_temperature, *air_inlet_temperature_min, *air_inlet_temperature_max;
    Sensor *evaporator1_temperature;
    Sensor *evaporator2_temperature;
    Sensor *evaporator3_temperature;

    Sensor *setpoint;

    BinarySensor* input_i2;
    BinarySensor* input_i1;
    BinarySensor* heating_active;

    BinarySensor* cycle1_active;
    BinarySensor* cycle2_active;
    BinarySensor* cycle3_active;
    BinarySensor* cycle4_active;
    BinarySensor* cycle5_active;
    BinarySensor* cycle6_active;

    Sensor* cycle1_count;
    Sensor* cycle2_count;
    Sensor* cycle3_count;
    Sensor* cycle4_count;
    Sensor* cycle5_count;
    Sensor* cycle6_count;

    SelectEntity* operation_mode;

    std::vector<Entity*> entities;

private:
    DhwState(const char* device_id, const char* device_name);
    ~DhwState() = default;
};

}  // namespace aquamqtt

#endif  // AQUAMQTT_DHWSTATE_H
