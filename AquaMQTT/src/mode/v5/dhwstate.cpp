#include "dhwstate.h"

#include <cstring>

#include <Arduino.h>

#include "constants.h"

namespace aquamqtt
{


char *device_id = nullptr;
const char* unique_device_id() {
    if (!device_id) {
        uint64_t chipid;
        chipid = ESP.getEfuseMac();
        size_t n = snprintf(nullptr, 0, "%08x", (uint32_t)chipid) + 1;
        device_id = new char[n];
        snprintf(device_id, n, "%08x", (uint32_t)chipid);
    }
    return device_id;
}


/*** DHWSTATE ***/

DhwState& DhwState::getInstance() {
    static DhwState instance(unique_device_id(), HA_DEVICE_NAME);
    return instance;
}

DhwState::DhwState(const char* _device_id, const char* _device_name)
    : device_id(_device_id)
    , device_name(_device_name)
{
    entities = {
        serial_number = new TextSensor(this, "model_type", "Model Type", true),
        controller_model = new TextSensor(this, "model_type", "Controller Model", true),
        power_board_version = new TextSensor(this, "model_type", "Power Board Firmware", true),
        hmi_version = new TextSensor(this, "model_type", "HMI Firmware", true),
        hmi_model = new TextSensor(this, "model_type", "HMI Model", true),

        water_temperature = new FilteredSensor(this, "water_temperature", "Water Temperature", false),
        water_temperature_min = new Sensor(this, "water_temperature_min", "Water Temperature (min)", true),
        water_temperature_max = new Sensor(this, "water_temperature_max", "Water Temperature (max)", true),

        compressor_outlet_temperature = new FilteredSensor(this, "compressor_outlet_temperature", "Compressor Outlet Temperature", true),
        compressor_outlet_temperature_min = new Sensor(this, "compressor_outlet_temperature_min", "Compressor Outlet Temperature (min)", true),
        compressor_outlet_temperature_max = new Sensor(this, "compressor_outlet_temperature_max", "Compressor Outlet Temperature (max)", true),

        air_inlet_temperature = new FilteredSensor(this, "air_inlet_temperature", "Air inlet Temperature", true),
        air_inlet_temperature_min = new Sensor(this, "air_inlet_temperature_min", "Air inlet Temperature (min)", true),
        air_inlet_temperature_max = new Sensor(this, "air_inlet_temperature_max", "Air inlet Temperature (max)", true),

        evaporator1_temperature = new FilteredSensor(this, "evaporator1_temperature", "Evaporator1 Temperature", true),
        evaporator2_temperature = new FilteredSensor(this, "evaporator2_temperature", "Evaporator2 Temperature", true),
        evaporator3_temperature = new FilteredSensor(this, "evaporator3_temperature", "Evaporator3 Temperature", true),

        setpoint = new Sensor(this, "setpoint", "Setpoint", false),

        input_i2 = new BinarySensor(this, "input_i2", "Input I2", true),
        input_i1 = new BinarySensor(this, "input_i1", "Input I1", true),
        heating_active = new BinarySensor(this, "heating_active", "Heating Active", false),

        cycle1_active = new BinarySensor(this, "cycle1_active", "Cycle1 Active", true),
        cycle2_active = new BinarySensor(this, "cycle2_active", "Cycle2 Active", true),
        cycle3_active = new BinarySensor(this, "cycle3_active", "Cycle3 Active", true),
        cycle4_active = new BinarySensor(this, "cycle4_active", "Cycle4 Active", true),
        cycle5_active = new BinarySensor(this, "cycle5_active", "Cycle5 Active", true),
        cycle6_active = new BinarySensor(this, "cycle6_active", "Cycle6 Active", true),

        cycle1_count = new Sensor(this, "cycle1_count", "Cycle1 Cycles", true, "%.0f"),
        cycle2_count = new Sensor(this, "cycle2_count", "Cycle2 Cycles", true, "%.0f"),
        cycle3_count = new Sensor(this, "cycle3_count", "Cycle3 Cycles", true, "%.0f"),
        cycle4_count = new Sensor(this, "cycle4_count", "Cycle4 Cycles", true, "%.0f"),
        cycle5_count = new Sensor(this, "cycle5_count", "Cycle5 Cycles", true, "%.0f"),
        cycle6_count = new Sensor(this, "cycle6_count", "Cycle6 Cycles", true, "%.0f"),

        operation_mode = new SelectEntity(this, "operation_mode", "Operation Mode", false),
    };

    // configure options for DHW unit operation mode
    operation_mode->addOption(OPERATION_MODE_USE_INPUT_STR); // this is the default
    if (config::OPERATION_MODE == config::EOperationMode::V5_MITM) {
        // in MITM mode, we can manipulate messages to the HMI
        operation_mode->addOption(OPERATION_MODE_NORMAL_STR);
        operation_mode->addOption(OPERATION_MODE_EAGER_STR);
        operation_mode->addOption(OPERATION_MODE_OFF_STR);
        operation_mode->addOption(OPERATION_MODE_BOOST_STR);
    } else {
        operation_mode->setEnabledByDefault(false);
    }
}


}  // namespace aquamqtt
