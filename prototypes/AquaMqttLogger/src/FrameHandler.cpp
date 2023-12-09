#include "FrameHandler.h"


#include "Arduino.h"
#include "MQTTDefinitions.h"

FrameHandler::FrameHandler(ESPLink* mqttClient) : mMqttClient(mqttClient)
{
}

void FrameHandler::handleFrame(uint8_t frameId, uint8_t payloadLength, uint8_t* payload)
{
    if (DEBUG_AQUAMQTT)
    {
        logDebug(frameId, payloadLength, payload);
    }

    if (frameId == 193)
    {
        parse193(payloadLength, payload);
    }
    else if (frameId == 194)
    {
        parse194(payloadLength, payload);
    }
    else if (frameId == 67)
    {
        parse67(payloadLength, payload);
    }

    delete[] payload;
}

void FrameHandler::parse193(uint8_t payloadLength, const uint8_t* payload)
{
    if (payloadLength != 37)
    {
        return;
    }
    char topic[40];
    char payloadStr[10];

    float hotWaterTemp = (float) (((int16_t) payload[2] << 8) | payload[1]) / 10.0;
    dtostrf(hotWaterTemp, 3, 1, payloadStr);
    sprintf(topic, "%S%S", BASE_TOPIC, HOT_WATER_TEMP);
    mMqttClient->getMqttClient()->publish(topic, payloadStr);

    float airTemp = (float) (((int16_t) payload[4] << 8) | payload[3]) / 10.0;
    dtostrf(airTemp, 3, 1, payloadStr);
    sprintf(topic, "%S%S", BASE_TOPIC, SUPPLY_AIR_TEMP);
    mMqttClient->getMqttClient()->publish(topic, payloadStr);

    float evaporatorLowerAirTemp = (float) (((int16_t) payload[6] << 8) | payload[5]) / 10.0;
    dtostrf(evaporatorLowerAirTemp, 3, 1, payloadStr);
    sprintf(topic, "%S%S", BASE_TOPIC, EVAPORATOR_AIR_TEMP_LOWER);
    mMqttClient->getMqttClient()->publish(topic, payloadStr);

    float evaporatorUpperAirTemp = (float) (((int16_t) payload[8] << 8) | payload[7]) / 10.0;
    dtostrf(evaporatorUpperAirTemp, 3, 1, payloadStr);
    sprintf(topic, "%S%S", BASE_TOPIC, EVAPORATOR_AIR_TEMP_UPPER);
    mMqttClient->getMqttClient()->publish(topic, payloadStr);

    uint16_t fanSpeed = ((uint16_t) payload[19] << 8) | (uint16_t) payload[18];
    ultoa(fanSpeed, payloadStr, 10);
    sprintf(topic, "%S%S", BASE_TOPIC, FAN_SPEED);
    mMqttClient->getMqttClient()->publish(topic, payloadStr);

    bool stateHeatingElement = payload[17] & 0x01;
    itoa(stateHeatingElement, payloadStr, 10);
    sprintf(topic, "%S%S", BASE_TOPIC, STATE_HEAT_ELEMENT);
    mMqttClient->getMqttClient()->publish(topic, payloadStr);

    bool stateHeatpump = payload[17] & 0x02;
    itoa(stateHeatpump, payloadStr, 10);
    sprintf(topic, "%S%S", BASE_TOPIC, STATE_HEATPUMP);
    mMqttClient->getMqttClient()->publish(topic, payloadStr);

    bool stateBoilerBackup = payload[17] & 0x04;
    itoa(stateBoilerBackup, payloadStr, 10);
    sprintf(topic, "%S%S", BASE_TOPIC, STATE_EXT_BOILER);
    mMqttClient->getMqttClient()->publish(topic, payloadStr);

    bool stateFan = payload[17] & 0x08;
    itoa(stateFan, payloadStr, 10);
    sprintf(topic, "%S%S", BASE_TOPIC, STATE_FAN);
    mMqttClient->getMqttClient()->publish(topic, payloadStr);

    bool stateDefrost = payload[17] & 0x20;
    itoa(stateDefrost, payloadStr, 10);
    sprintf(topic, "%S%S", BASE_TOPIC, STATE_DEFROST);
    mMqttClient->getMqttClient()->publish(topic, payloadStr);
}

void FrameHandler::parse194(uint8_t payloadLength, const uint8_t* payload)
{
    if (payloadLength != 35)
    {
        return;
    }
    char topic[40];
    char payloadStr[20];

    float temperature = (float) (((int16_t) payload[2] << 8) | payload[1]) / 10.0;
    dtostrf(temperature, 3, 1, payloadStr);
    sprintf(topic, "%S%S", BASE_TOPIC, HOT_WATER_TEMP_TARGET);
    mMqttClient->getMqttClient()->publish(topic, payloadStr);

    uint8_t operationMode = payload[3] & 0x0F;
    sprintf(topic, "%S%S", BASE_TOPIC, OPERATION_MODE);
    switch (operationMode)
    {
        case 0:
            sprintf(payloadStr, "%S", ENUM_OPERATION_MODE_ABSENCE);
            break;
        case 1:
            sprintf(payloadStr, "%S", ENUM_OPERATION_MODE_ECO_ON);
            break;
        case 2:
            sprintf(payloadStr, "%S", ENUM_OPERATION_MODE_ECO_OFF);
            break;
        case 3:
            sprintf(payloadStr, "%S", ENUM_OPERATION_MODE_BOOST);
            break;
        case 4:
            sprintf(payloadStr, "%S", ENUM_OPERATION_MODE_AUTO);
            break;
        default:
            sprintf(payloadStr, "%S", ENUM_UNKNOWN);
            break;
    }
    mMqttClient->getMqttClient()->publish(topic, payloadStr);

    bool timerMode = payload[3] & 0x40;
    sprintf(topic, "%S%S", BASE_TOPIC, OPERATION_TYPE);
    if (timerMode)
    {
        sprintf(payloadStr, "%S", ENUM_OPERATION_TYPE_TIMER);
    }
    else
    {
        sprintf(payloadStr, "%S", ENUM_OPERATION_TYPE_ALWAYS_ON);
    }
    mMqttClient->getMqttClient()->publish(topic, payloadStr);

    sprintf(payloadStr, "%02d:%02d:%02d", payload[21], payload[20], payload[17]);
    sprintf(topic, "%S%S", BASE_TOPIC, TIME);
    mMqttClient->getMqttClient()->publish(topic, payloadStr);

    uint16_t year  = 2000 + (payload[19] / 2);
    uint8_t  month = 1 + (payload[18] >> 5) + ((payload[19] % 2) * 7);
    uint8_t  day   = payload[18] & 0x1F;
    sprintf(payloadStr, "%d.%d.%d", day, month, year);
    sprintf(topic, "%S%S", BASE_TOPIC, DATE);
    mMqttClient->getMqttClient()->publish(topic, payloadStr);
}

void FrameHandler::parse67(uint8_t payloadLength, const uint8_t* payload)
{
    if (payloadLength != 31)
    {
        return;
    }
    char topic[40];
    char payloadStr[20];

    uint32_t totalHeatpumpHours = ((uint32_t) payload[14] << 24) | ((uint32_t) payload[13] << 16)
                                  | ((uint32_t) payload[12] << 8) | (uint32_t) payload[11];
    ultoa(totalHeatpumpHours, payloadStr, 10);
    sprintf(topic, "%S%S", BASE_TOPIC, TOTAL_HEATPUMP_HOURS);
    mMqttClient->getMqttClient()->publish(topic, payloadStr);

    uint32_t totalHeatingElemHours = ((uint32_t) payload[18] << 24) | ((uint32_t) payload[17] << 16)
                                     | ((uint32_t) payload[16] << 8) | (uint32_t) payload[15];
    ultoa(totalHeatingElemHours, payloadStr, 10);
    sprintf(topic, "%S%S", BASE_TOPIC, TOTAL_HEATING_ELEM_HOURS);
    mMqttClient->getMqttClient()->publish(topic, payloadStr);

    uint32_t totalHours = ((uint32_t) payload[22] << 24) | ((uint32_t) payload[21] << 16)
                          | ((uint32_t) payload[20] << 8) | (uint32_t) payload[19];
    ultoa(totalHours, payloadStr, 10);
    sprintf(topic, "%S%S", BASE_TOPIC, TOTAL_HOURS);
    mMqttClient->getMqttClient()->publish(topic, payloadStr);

    uint64_t totalEnergyCounter = ((uint64_t) payload[30] << 56) | ((uint64_t) payload[29] << 48)
                                  | ((uint64_t) payload[28] << 40) | ((uint64_t) payload[27] << 32)
                                  | ((uint64_t) payload[26] << 24) | ((uint64_t) payload[25] << 16)
                                  | ((uint64_t) payload[24] << 8) | (uint64_t) payload[23];
    ultoa(totalEnergyCounter, payloadStr, 10);
    sprintf(topic, "%S%S", BASE_TOPIC, TOTAL_ENERGY_WH);
    mMqttClient->getMqttClient()->publish(topic, payloadStr);

    uint16_t powerHeatpump = ((uint16_t) payload[2] << 8) | (uint16_t) payload[1];
    ultoa(powerHeatpump, payloadStr, 10);
    sprintf(topic, "%S%S", BASE_TOPIC, POWER_HEATPUMP);
    mMqttClient->getMqttClient()->publish(topic, payloadStr);

    uint16_t powerHeatElement = ((uint16_t) payload[4] << 8) | (uint16_t) payload[3];
    ultoa(powerHeatElement, payloadStr, 10);
    sprintf(topic, "%S%S", BASE_TOPIC, POWER_HEAT_ELEMENT);
    mMqttClient->getMqttClient()->publish(topic, payloadStr);

    uint16_t powerOverall = ((uint16_t) payload[8] << 8) | (uint16_t) payload[7];
    ultoa(powerOverall, payloadStr, 10);
    sprintf(topic, "%S%S", BASE_TOPIC, POWER_TOTAL);
    mMqttClient->getMqttClient()->publish(topic, payloadStr);;
}

void FrameHandler::logDebug(uint8_t frameId, uint8_t payloadLength, uint8_t* payload)
{
    char topic[40];
    sprintf(topic, "%S%d", DEBUG_TOPIC, frameId);

    const size_t num_bytes = payloadLength / sizeof(uint8_t);
    char         hex_str[num_bytes * 2 + 1];
    for (size_t i = 0; i < num_bytes; i++)
    {
        sprintf(&hex_str[i * 2], "%02X", payload[i]);
    }
    hex_str[num_bytes * 2] = '\0';

    mMqttClient->getMqttClient()->publish(topic, hex_str);
}
