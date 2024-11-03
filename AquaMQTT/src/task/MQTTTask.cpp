#include "task/MQTTTask.h"

#include <ArduinoJson.h>
#include <WiFi.h>
#include <esp_task_wdt.h>

#include "FastCRC.h"
#include "config/Configuration.h"
#include "message/IEnergyMessage.h"
#include "message/IErrorMessage.h"
#include "message/IHMIMessage.h"
#include "message/legacy/ErrorMessage.h"
#include "message/legacy/HMIMessage.h"
#include "message/legacy/MainEnergyMessage.h"
#include "message/legacy/MainStatusMessage.h"
#include "message/next/ErrorMessage.h"
#include "message/next/HMIMessage.h"
#include "message/next/MainEnergyMessage.h"
#include "message/next/MainStatusMessage.h"
#include "mqtt/MQTTDefinitions.h"
#include "mqtt/MQTTDiscovery.h"
#include "state/HMIStateProxy.h"
#include "state/MainStateProxy.h"

namespace aquamqtt
{

using namespace mqtt;
using namespace message;

MQTTTask::MQTTTask()
    : mLastStatsUpdate(0)
    , mLastFullUpdate(0)
    , mMQTTClient(256)
    , mTransferBuffer{ 0 }
    , mTaskHandle(nullptr)
    , mTopicBuffer{ 0 }
    , mPayloadBuffer{ 0 }
    , mLastProcessedHMIMessage(nullptr)
    , mLastProcessedEnergyMessage(nullptr)
    , mLastProcessedMainMessage(nullptr)
    , mHotWaterTempFilter(config::KALMAN_MEA_E, config::KALMAN_EST_E, config::KALMAN_Q)
    , mHotWaterTempFiltered(0.0)
    , mAirTempFilter(config::KALMAN_MEA_E, config::KALMAN_EST_E, config::KALMAN_Q)
    , mAirTempFiltered(0.0)
    , mEvaporatorLowerAirTempFilter(config::KALMAN_MEA_E, config::KALMAN_EST_E, config::KALMAN_Q)
    , mEvaporatorLowerAirTempFiltered(0.0)
    , mEvaporatorUpperAirTempFilter(config::KALMAN_MEA_E, config::KALMAN_EST_E, config::KALMAN_Q)
    , mEvaporatorUpperAirTempFiltered(0.0)
    , mCompressorTempFilter(config::KALMAN_MEA_E, config::KALMAN_EST_E, config::KALMAN_Q)
    , mCompressorTempFiltered(0.0)
    , mPublishedDiscovery(false)
{
}

MQTTTask::~MQTTTask()
{
    delete mLastProcessedHMIMessage;
    delete mLastProcessedEnergyMessage;
    delete mLastProcessedMainMessage;
}

void MQTTTask::messageReceived(String& topic, String& payload)
{
    Serial.println("incoming: " + topic + " - " + payload);

    // Note: Do not use the client in the callback to publish, subscribe or
    // unsubscribe as it may cause deadlocks when other things arrive while
    // sending and receiving acknowledgments. Instead, change a global variable,
    // or push to a queue and handle it in the loop after calling `client.loop()`.

    if (strstr_P(topic.c_str(), HMI_OPERATION_MODE) != nullptr)
    {
        if (strstr_P(payload.c_str(), ENUM_OPERATION_MODE_ABSENCE) != nullptr)
        {
            HMIStateProxy::getInstance().onOperationModeChanged(std::make_unique<HMIOperationMode>(OM_ABSENCE));
        }

        else if (strstr_P(payload.c_str(), ENUM_OPERATION_MODE_BOOST) != nullptr)
        {
            HMIStateProxy::getInstance().onOperationModeChanged(std::make_unique<HMIOperationMode>(OM_BOOST));
        }

        else if (strstr_P(payload.c_str(), ENUM_OPERATION_MODE_AUTO) != nullptr)
        {
            HMIStateProxy::getInstance().onOperationModeChanged(std::make_unique<HMIOperationMode>(OM_AUTO));
        }

        else if (strstr_P(payload.c_str(), ENUM_OPERATION_MODE_ECO_ON) != nullptr)
        {
            HMIStateProxy::getInstance().onOperationModeChanged(std::make_unique<HMIOperationMode>(OM_ECO_ACTIVE));
        }

        else if (strstr_P(payload.c_str(), ENUM_OPERATION_MODE_ECO_OFF) != nullptr)
        {
            HMIStateProxy::getInstance().onOperationModeChanged(std::make_unique<HMIOperationMode>(OM_ECO_INACTIVE));
        }
        else
        {
            HMIStateProxy::getInstance().onOperationModeChanged(std::unique_ptr<HMIOperationMode>(nullptr));
        }
    }
    else if (strstr_P(topic.c_str(), HMI_OPERATION_TYPE) != nullptr)
    {
        if (strstr_P(payload.c_str(), ENUM_OPERATION_TYPE_TIMER) != nullptr)
        {
            HMIStateProxy::getInstance().onOperationTypeChanged(std::make_unique<HMIOperationType>(TIMER));
        }
        else if (strstr_P(payload.c_str(), ENUM_OPERATION_TYPE_ALWAYS_ON) != nullptr)
        {
            HMIStateProxy::getInstance().onOperationTypeChanged(std::make_unique<HMIOperationType>(ALWAYS_ON));
        }
        else
        {
            HMIStateProxy::getInstance().onOperationTypeChanged(std::unique_ptr<HMIOperationType>(nullptr));
        }
    }

    else if (strstr_P(topic.c_str(), HMI_EMERGENCY_MODE) != nullptr)
    {
        if (payload.isEmpty())
        {
            HMIStateProxy::getInstance().onEmergencyModeEnabledChanged(std::unique_ptr<bool>(nullptr));
        }
        else
        {
            bool result = (strcmp(payload.c_str(), "1") == 0);
            HMIStateProxy::getInstance().onEmergencyModeEnabledChanged(std::make_unique<bool>(result));
        }
    }
    else if (strstr_P(topic.c_str(), HMI_HEATING_ELEMENT_ENABLED) != nullptr)
    {
        if (payload.isEmpty())
        {
            HMIStateProxy::getInstance().onHeatingElementEnabledChanged(std::unique_ptr<bool>(nullptr));
        }
        else
        {
            bool result = (strcmp(payload.c_str(), "1") == 0);
            HMIStateProxy::getInstance().onHeatingElementEnabledChanged(std::make_unique<bool>(result));
        }
    }
    else if (strstr_P(topic.c_str(), HMI_HOT_WATER_TEMP_TARGET) != nullptr)
    {
        if (payload.isEmpty())
        {
            HMIStateProxy::getInstance().onWaterTempTargetChanged(std::unique_ptr<float>(nullptr));
        }
        else
        {
            HMIStateProxy::getInstance().onWaterTempTargetChanged(std::make_unique<float>(atof(payload.c_str())));
        }
    }
    else if (strstr_P(topic.c_str(), HMI_INSTALLATION_CONFIG) != nullptr)
    {
        if (strstr_P(payload.c_str(), ENUM_INSTALLATION_THERMODYNAMICS_ONLY) != nullptr)
        {
            HMIStateProxy::getInstance().onInstallationModeChanged(std::make_unique<HMIInstallation>(INST_HP_ONLY));
        }
        else if (strstr_P(payload.c_str(), ENUM_INSTALLATION_SOLAR_BACKUP) != nullptr)
        {
            HMIStateProxy::getInstance().onInstallationModeChanged(
                    std::make_unique<HMIInstallation>(INST_HP_AND_SOLAR));
        }

        else if (strstr_P(payload.c_str(), ENUM_INSTALLATION_BOILER_BACKUP_EXT_OPT) != nullptr)
        {
            HMIStateProxy::getInstance().onInstallationModeChanged(
                    std::make_unique<HMIInstallation>(INST_HP_AND_EXT_OPT_EXT));
        }

        else if (strstr_P(payload.c_str(), ENUM_INSTALLATION_BOILER_BACKUP_EXT_PRIO) != nullptr)
        {
            HMIStateProxy::getInstance().onInstallationModeChanged(
                    std::make_unique<HMIInstallation>(INST_HP_AND_EXT_PRIO_EXT));
        }

        else if (strstr_P(payload.c_str(), ENUM_INSTALLATION_BOILER_BACKUP_HP_OPT) != nullptr)
        {
            HMIStateProxy::getInstance().onInstallationModeChanged(
                    std::make_unique<HMIInstallation>(INST_HP_AND_EXT_OPT_HP));
        }

        else if (strstr_P(payload.c_str(), ENUM_INSTALLATION_BOILER_BACKUP_HP_PRIO) != nullptr)
        {
            HMIStateProxy::getInstance().onInstallationModeChanged(
                    std::make_unique<HMIInstallation>(INST_HP_AND_EXT_PRIO_HP));
        }
        else
        {
            HMIStateProxy::getInstance().onInstallationModeChanged(std::unique_ptr<HMIInstallation>(nullptr));
        }
    }
    else if (strstr_P(topic.c_str(), HMI_FAN_EXHAUST_CONFIG) != nullptr)
    {
        if (strstr_P(payload.c_str(), ENUM_CONFIG_EXHAUST_FAN_STOP) != nullptr)
        {
            HMIStateProxy::getInstance().onFanExhaustModeChanged(std::make_unique<HMIFanExhaust>(EXHAUST_STOP));
        }
        else if (strstr_P(payload.c_str(), ENUM_CONFIG_EXHAUST_FAN_LOW_SPEED) != nullptr)
        {
            HMIStateProxy::getInstance().onFanExhaustModeChanged(std::make_unique<HMIFanExhaust>(EXHAUST_LOW_SPEED));
        }
        else if (strstr_P(payload.c_str(), ENUM_CONFIG_EXHAUST_FAN_HIGH_SPEED) != nullptr)
        {
            HMIStateProxy::getInstance().onFanExhaustModeChanged(std::make_unique<HMIFanExhaust>(EXHAUST_HIGH_SPEED));
        }
        else
        {
            HMIStateProxy::getInstance().onFanExhaustModeChanged(std::unique_ptr<HMIFanExhaust>(nullptr));
        }
    }

    else if (strstr_P(topic.c_str(), HMI_AIR_DUCT_CONFIG) != nullptr)
    {
        if (strstr_P(payload.c_str(), ENUM_AIR_DUCT_IN_IN) != nullptr)
        {
            HMIStateProxy::getInstance().onAirductConfigChanged(std::make_unique<HMIAirDuctConfig>(AD_INT_INT));
        }
        else if (strstr_P(payload.c_str(), ENUM_AIR_DUCT_INT_EXT) != nullptr)
        {
            HMIStateProxy::getInstance().onAirductConfigChanged(std::make_unique<HMIAirDuctConfig>(AD_INT_EXT));
        }
        else if (strstr_P(payload.c_str(), ENUM_AIR_DUCT_EXT_EXT) != nullptr)
        {
            HMIStateProxy::getInstance().onAirductConfigChanged(std::make_unique<HMIAirDuctConfig>(AD_EXT_EXT));
        }
        else
        {
            HMIStateProxy::getInstance().onAirductConfigChanged(std::unique_ptr<HMIAirDuctConfig>(nullptr));
        }
    }

    else if (
            (strlen(optionalSubscribeTopicSetPvHeatPumpFlag) != 0)
            && strstr_P(topic.c_str(), optionalSubscribeTopicSetPvHeatPumpFlag) != nullptr)
    {
        bool enabled = strcmp(payload.c_str(), "1") == 0;
        HMIStateProxy::getInstance().onPVModeHeatpumpEnabled(enabled);
        MainStateProxy::getInstance().onPVModeHeatpumpEnabled(enabled);
    }
    else if (strstr_P(topic.c_str(), STATS_ENABLE_FLAG_PV_HEATPUMP) != nullptr)
    {
        bool enabled = strcmp(payload.c_str(), "1") == 0;
        HMIStateProxy::getInstance().onPVModeHeatpumpEnabled(enabled);
        MainStateProxy::getInstance().onPVModeHeatpumpEnabled(enabled);
    }
    else if (
            (strlen(optionalSubscribeTopicSetPvHeatElementFlag) != 0)
            && strstr_P(topic.c_str(), optionalSubscribeTopicSetPvHeatElementFlag) != nullptr)
    {
        bool enabled = strcmp(payload.c_str(), "1") == 0;
        HMIStateProxy::getInstance().onPVModeHeatElementEnabled(enabled);
        MainStateProxy::getInstance().onPVModeHeatElementEnabled(enabled);
    }
    else if (strstr_P(topic.c_str(), STATS_ENABLE_FLAG_PV_HEATELEMENT) != nullptr)
    {
        bool enabled = strcmp(payload.c_str(), "1") == 0;
        HMIStateProxy::getInstance().onPVModeHeatElementEnabled(enabled);
        MainStateProxy::getInstance().onPVModeHeatElementEnabled(enabled);
    }
    else if (strstr_P(topic.c_str(), AQUAMQTT_RESET_OVERRIDES) != nullptr)
    {
        HMIStateProxy::getInstance().onResetOverrides();
    }
}

void MQTTTask::spawn()
{
    xTaskCreatePinnedToCore(MQTTTask::innerTask, "mqttTaskLoop", 9000, this, 3, &mTaskHandle, 0);
    esp_task_wdt_add(mTaskHandle);
    DHWState::getInstance().setListener(mTaskHandle);
    HMIStateProxy::getInstance().setListener(mTaskHandle);
    MainStateProxy::getInstance().setListener(mTaskHandle);
}

[[noreturn]] void MQTTTask::innerTask(void* pvParameters)
{
    auto* self = (MQTTTask*) pvParameters;

    self->setup();
    while (true)
    {
        esp_task_wdt_reset();
        self->loop();
    }
}

void MQTTTask::setup()
{
    mMQTTClient.begin(aquamqtt::config::brokerAddr, aquamqtt::config::brokerPort, mWiFiClient);
    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%s%s%s",
            config::mqttPrefix,
            BASE_TOPIC,
            STATS_SUBTOPIC,
            STATS_AQUAMQTT_LAST_WILL);
    sprintf(reinterpret_cast<char*>(mPayloadBuffer), "%s", ENUM_LAST_WILL_OFFLINE);

    mMQTTClient.setWill(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer), true, 0);
    mMQTTClient.onMessage(MQTTTask::messageReceived);
}

void MQTTTask::check_mqtt_connection()
{
    if (mMQTTClient.connected())
    {
        return;
    }

    Serial.println("[mqtt] is disconnected... trying to connect to mqtt broker");

    while (!mMQTTClient.connect(
            config::brokerClientId,
            strlen(config::brokerUser) == 0 ? nullptr : config::brokerUser,
            strlen(config::brokerPassword) == 0 ? nullptr : config::brokerPassword))
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        esp_task_wdt_reset();
    }
    mPublishedDiscovery = false;

    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%s%s%s",
            config::mqttPrefix,
            BASE_TOPIC,
            STATS_SUBTOPIC,
            STATS_AQUAMQTT_LAST_WILL);

    sprintf(reinterpret_cast<char*>(mPayloadBuffer), "%s", ENUM_LAST_WILL_ONLINE);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer), true, 0);

    sprintf(reinterpret_cast<char*>(mTopicBuffer), "%s%s", config::mqttPrefix, CONTROL_TOPIC);
    mMQTTClient.subscribe(reinterpret_cast<char*>(mTopicBuffer));

    if (strlen(optionalSubscribeTopicSetPvHeatPumpFlag) != 0)
    {
        sprintf(reinterpret_cast<char*>(mTopicBuffer), "%s", optionalSubscribeTopicSetPvHeatPumpFlag);
        mMQTTClient.subscribe(reinterpret_cast<char*>(mTopicBuffer));
    }

    if (strlen(optionalSubscribeTopicSetPvHeatElementFlag) != 0)
    {
        sprintf(reinterpret_cast<char*>(mTopicBuffer), "%s", optionalSubscribeTopicSetPvHeatElementFlag);
        mMQTTClient.subscribe(reinterpret_cast<char*>(mTopicBuffer));
    }

    Serial.println("[mqtt] is now connected");
}

void MQTTTask::loop()
{
    check_mqtt_connection();

    auto mqttCycle = pdMS_TO_TICKS(5);

    sendHomeassistantDiscovery();

    bool fullUpdate = (millis() - mLastFullUpdate) >= config::MQTT_FULL_UPDATE_MS;

    bool statsUpdate = (millis() - mLastStatsUpdate) >= config::MQTT_STATS_UPDATE_MS;

    auto notify = ulTaskNotifyTake(pdTRUE, mqttCycle);

    if ((notify & 1 << 8) != 0 || fullUpdate)
    {
        message::ProtocolVersion version = message::PROTOCOL_UNKNOWN;
        size_t length = HMIStateProxy::getInstance().copyFrame(HMI_MESSAGE_IDENTIFIER, mTransferBuffer, version);
        if (length > 0)
        {
            updateHMIStatus(fullUpdate, version);

            if (mLastProcessedHMIMessage == nullptr)
            {
                mLastProcessedHMIMessage = new uint8_t[length];
            }
            memcpy(mLastProcessedHMIMessage, mTransferBuffer, length);
        }
    }

    if ((notify & 1 << 7) != 0 || fullUpdate)
    {
        message::ProtocolVersion version = message::PROTOCOL_UNKNOWN;
        size_t length = MainStateProxy::getInstance().copyFrame(MAIN_MESSAGE_IDENTIFIER, mTransferBuffer, version);
        if (length > 0)
        {
            updateMainStatus(fullUpdate, version);

            if (mLastProcessedMainMessage == nullptr)
            {
                mLastProcessedMainMessage = new uint8_t[length];
            }
            memcpy(mLastProcessedMainMessage, mTransferBuffer, length);
        }
    }

    if ((notify & 1 << 6) != 0 || fullUpdate)
    {
        message::ProtocolVersion version = message::PROTOCOL_UNKNOWN;
        size_t length = MainStateProxy::getInstance().copyFrame(ENERGY_MESSAGE_IDENTIFIER, mTransferBuffer, version);
        if (length > 0)
        {
            updateEnergyStats(fullUpdate, version);

            if (mLastProcessedEnergyMessage == nullptr)
            {
                mLastProcessedEnergyMessage = new uint8_t[length];
            }
            memcpy(mLastProcessedEnergyMessage, mTransferBuffer, length);
        }
    }

    if ((notify & 1 << 5) != 0)
    {
        message::ProtocolVersion version = message::PROTOCOL_UNKNOWN;
        if (MainStateProxy::getInstance().copyFrame(ERROR_MESSAGE_IDENTIFIER, mTransferBuffer, version))
        {
            updateErrorStatus(version);
        }
    }

    if (statsUpdate)
    {
        updateStats();
    }

    if (statsUpdate)
    {
        Serial.println("[mqtt] stat update");
        mLastStatsUpdate = millis();
        Serial.print("[mqtt]: stack size (words)");
        Serial.println(uxTaskGetStackHighWaterMark(nullptr));
    }

    if (fullUpdate)
    {
        mLastFullUpdate = millis();
    }

    mMQTTClient.loop();
}

void MQTTTask::updateStats()
{
    publishString(
            STATS_SUBTOPIC,
            STATS_AQUAMQTT_MODE,
            config::OPERATION_MODE == config::EOperationMode::LISTENER ? ENUM_AQUAMQTT_MODE_LISTENER
                                                                       : ENUM_AQUAMQTT_MODE_MITM);

    publishString(STATS_SUBTOPIC, STATS_AQUAMQTT_ADDR, WiFi.localIP().toString().c_str());

    publishi(STATS_SUBTOPIC, STATS_AQUAMQTT_RSSI, WiFi.RSSI());

    publishString(STATS_SUBTOPIC, STATS_AQUAMQTT_PROTOCOL, protocolVersionStr(DHWState::getInstance().getVersion()));

    if (config::OPERATION_MODE == config::EOperationMode::LISTENER)
    {
        if (config::MQTT_PUBLISH_SERIAL_STATISTICS)
        {
            auto listenerStats = DHWState::getInstance().getFrameBufferStatistics(0);
            publishul(STATS_SUBTOPIC, STATS_MSG_HANDLED, listenerStats.msgHandled);
            publishul(STATS_SUBTOPIC, STATS_MSG_UNHANDLED, listenerStats.msgUnhandled);
            publishul(STATS_SUBTOPIC, STATS_MSG_CRC_NOK, listenerStats.msgCRCFail);
            publishul(STATS_SUBTOPIC, STATS_DROPPED_BYTES, listenerStats.droppedBytes);
        }
    }
    else
    {
        if (config::MQTT_PUBLISH_SERIAL_STATISTICS)
        {
            auto hmiStats = DHWState::getInstance().getFrameBufferStatistics(1);
            publishul(STATS_SUBTOPIC, HMI_SUBTOPIC, STATS_MSG_HANDLED, hmiStats.msgHandled);
            publishul(STATS_SUBTOPIC, HMI_SUBTOPIC, STATS_MSG_UNHANDLED, hmiStats.msgUnhandled);
            publishul(STATS_SUBTOPIC, HMI_SUBTOPIC, STATS_MSG_CRC_NOK, hmiStats.msgCRCFail);
            publishul(STATS_SUBTOPIC, HMI_SUBTOPIC, STATS_DROPPED_BYTES, hmiStats.droppedBytes);
            publishul(STATS_SUBTOPIC, HMI_SUBTOPIC, STATS_MSG_SENT, hmiStats.msgSent);

            auto mainStats = DHWState::getInstance().getFrameBufferStatistics(2);
            publishul(STATS_SUBTOPIC, MAIN_SUBTOPIC, STATS_MSG_HANDLED, mainStats.msgHandled);
            publishul(STATS_SUBTOPIC, MAIN_SUBTOPIC, STATS_MSG_UNHANDLED, mainStats.msgUnhandled);
            publishul(STATS_SUBTOPIC, MAIN_SUBTOPIC, STATS_MSG_CRC_NOK, mainStats.msgCRCFail);
            publishul(STATS_SUBTOPIC, MAIN_SUBTOPIC, STATS_DROPPED_BYTES, mainStats.droppedBytes);
            publishul(STATS_SUBTOPIC, MAIN_SUBTOPIC, STATS_MSG_SENT, mainStats.msgSent);
        }

        // TODO: these should be updated a soon as something changes, and not by the stats timer
        auto         overrides = HMIStateProxy::getInstance().getOverrides();
        JsonDocument overrideJson;
        overrideJson[HMI_OPERATION_MODE]          = overrides.operationMode ? "1" : "0";
        overrideJson[HMI_OPERATION_TYPE]          = overrides.operationType ? "1" : "0";
        overrideJson[HMI_HOT_WATER_TEMP_TARGET]   = overrides.waterTempTarget ? "1" : "0";
        overrideJson[HMI_HEATING_ELEMENT_ENABLED] = overrides.heatingElementEnabled ? "1" : "0";
        overrideJson[HMI_EMERGENCY_MODE]          = overrides.emergencyModeEnabled ? "1" : "0";
        overrideJson[HMI_INSTALLATION_CONFIG]     = overrides.installationMode ? "1" : "0";
        overrideJson[HMI_FAN_EXHAUST_CONFIG]      = overrides.exhaustFanMode ? "1" : "0";
        overrideJson[HMI_AIR_DUCT_CONFIG]         = overrides.airductConfig ? "1" : "0";
        overrideJson[HMI_TIME_AND_DATE]           = (aquamqtt::config::OVERRIDE_TIME_AND_DATE_IN_MITM
                                           && aquamqtt::config::OPERATION_MODE
                                                      != aquamqtt::config::EOperationMode::LISTENER)
                                                            ? "1"
                                                            : "0";
        serializeJson(overrideJson, mPayloadBuffer, config::MQTT_MAX_PAYLOAD_SIZE);

        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%s%s%s",
                config::mqttPrefix,
                BASE_TOPIC,
                STATS_SUBTOPIC,
                STATS_ACTIVE_OVERRIDES);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

        auto         mainOverrides = MainStateProxy::getInstance().getOverrides();
        JsonDocument mainOverrideJson;
        mainOverrideJson[MAIN_STATE_PV]    = mainOverrides.pvState ? "1" : "0";
        mainOverrideJson[MAIN_STATE_SOLAR] = mainOverrides.solarState ? "1" : "0";

        serializeJson(mainOverrideJson, mPayloadBuffer, config::MQTT_MAX_PAYLOAD_SIZE);

        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%s%s%s",
                config::mqttPrefix,
                BASE_TOPIC,
                STATS_SUBTOPIC,
                STATS_ACTIVE_OVERRIDES_MAIN);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

        publishString(
                STATS_SUBTOPIC,
                STATS_AQUAMQTT_OVERRIDE_MODE,
                aquamqttOverrideStr(HMIStateProxy::getInstance().getOverrideMode()));
        publishi(
                STATS_SUBTOPIC,
                STATS_ENABLE_FLAG_PV_HEATPUMP,
                HMIStateProxy::getInstance().isPVModeHeatPumpEnabled());
        publishi(
                STATS_SUBTOPIC,
                STATS_ENABLE_FLAG_PV_HEATELEMENT,
                HMIStateProxy::getInstance().isPVModeHeatElementEnabled());
    }
}

void MQTTTask::updateMainStatus(bool fullUpdate, message::ProtocolVersion& version)
{
    std::unique_ptr<message::IMainMessage> message;
    if (version == ProtocolVersion::PROTOCOL_NEXT)
    {
        message = std::make_unique<message::next::MainStatusMessage>(
                mTransferBuffer,
                fullUpdate ? nullptr : mLastProcessedMainMessage);
    }
    else
    {
        message = std::make_unique<message::legacy::MainStatusMessage>(
                mTransferBuffer,
                fullUpdate ? nullptr : mLastProcessedMainMessage);
    }

    publishFiltered(
            message,
            MAIN_ATTR_FLOAT::WATER_TEMPERATURE,
            mHotWaterTempFilter,
            mHotWaterTempFiltered,
            MAIN_HOT_WATER_TEMP,
            fullUpdate);

    publishFiltered(
            message,
            MAIN_ATTR_FLOAT::AIR_TEMPERATURE,
            mAirTempFilter,
            mAirTempFiltered,
            MAIN_SUPPLY_AIR_TEMP,
            fullUpdate);

    publishFiltered(
            message,
            MAIN_ATTR_FLOAT::EVAPORATOR_LOWER_TEMPERATURE,
            mEvaporatorLowerAirTempFilter,
            mEvaporatorLowerAirTempFiltered,
            MAIN_EVAPORATOR_AIR_TEMP_LOWER,
            fullUpdate);

    publishFiltered(
            message,
            MAIN_ATTR_FLOAT::EVAPORATOR_UPPER_TEMPERATURE,
            mEvaporatorUpperAirTempFilter,
            mEvaporatorUpperAirTempFiltered,
            MAIN_EVAPORATOR_AIR_TEMP_UPPER,
            fullUpdate);

    publishFiltered(
            message,
            MAIN_ATTR_FLOAT::COMPRESSOR_OUTLET_TEMPERATURE,
            mCompressorTempFilter,
            mCompressorTempFiltered,
            MAIN_COMPRESSOR_OUTLET_TEMP,
            fullUpdate);

    if (message->hasAttr(MAIN_ATTR_FLOAT::FAN_SPEED_PWM))
    {
        if (fullUpdate || message->hasChanged(MAIN_ATTR_FLOAT::FAN_SPEED_PWM))
        {
            publishFloat(MAIN_SUBTOPIC, MAIN_FAN_PWM, message->getAttr(MAIN_ATTR_FLOAT::FAN_SPEED_PWM));
        }
    }

    if (message->hasAttr(MAIN_ATTR_BOOL::STATE_HEATING_ELEMENT))
    {
        if (fullUpdate || message->hasChanged(MAIN_ATTR_BOOL::STATE_HEATING_ELEMENT))
        {
            publishi(MAIN_SUBTOPIC, MAIN_STATE_HEAT_ELEMENT, message->getAttr(MAIN_ATTR_BOOL::STATE_HEATING_ELEMENT));
        }
    }

    if (message->hasAttr(MAIN_ATTR_BOOL::STATE_HEATPUMP))
    {
        if (fullUpdate || message->hasChanged(MAIN_ATTR_BOOL::STATE_HEATPUMP))
        {
            publishi(MAIN_SUBTOPIC, MAIN_STATE_HEATPUMP, message->getAttr(MAIN_ATTR_BOOL::STATE_HEATPUMP));
        }
    }

    if (message->hasAttr(MAIN_ATTR_BOOL::STATE_BOILER_BACKUP))
    {
        if (fullUpdate || message->hasChanged(MAIN_ATTR_BOOL::STATE_BOILER_BACKUP))
        {
            publishi(MAIN_SUBTOPIC, MAIN_STATE_EXT_BOILER, message->getAttr(MAIN_ATTR_BOOL::STATE_BOILER_BACKUP));
        }
    }

    if (message->hasAttr(MAIN_ATTR_BOOL::STATE_FAN))
    {
        if (fullUpdate || message->hasChanged(MAIN_ATTR_BOOL::STATE_FAN))
        {
            publishi(MAIN_SUBTOPIC, MAIN_STATE_FAN, message->getAttr(MAIN_ATTR_BOOL::STATE_FAN));
        }
    }

    if (message->hasAttr(MAIN_ATTR_BOOL::STATE_DEFROST))
    {
        if (fullUpdate || message->hasChanged(MAIN_ATTR_BOOL::STATE_DEFROST))
        {
            publishi(MAIN_SUBTOPIC, MAIN_STATE_DEFROST, message->getAttr(MAIN_ATTR_BOOL::STATE_DEFROST));
        }
    }

    if (message->hasAttr(MAIN_ATTR_BOOL::STATE_PV))
    {
        if (fullUpdate || message->hasChanged(MAIN_ATTR_BOOL::STATE_PV))
        {
            publishi(MAIN_SUBTOPIC, MAIN_STATE_PV, message->getAttr(MAIN_ATTR_BOOL::STATE_PV));
        }
    }

    if (message->hasAttr(MAIN_ATTR_BOOL::STATE_SOLAR))
    {
        if (fullUpdate || message->hasChanged(MAIN_ATTR_BOOL::STATE_SOLAR))
        {
            publishi(MAIN_SUBTOPIC, MAIN_STATE_SOLAR, message->getAttr(MAIN_ATTR_BOOL::STATE_SOLAR));
        }
    }

    if (message->hasAttr(MAIN_ATTR_U8::SETTING_FAN_PWM_FIRST))
    {
        if (fullUpdate || message->hasChanged(MAIN_ATTR_U8::SETTING_FAN_PWM_FIRST))
        {
            publishi(MAIN_SUBTOPIC, MAIN_SETTING_PWM_01, message->getAttr(MAIN_ATTR_U8::SETTING_FAN_PWM_FIRST));
        }
    }

    if (message->hasAttr(MAIN_ATTR_U8::SETTING_FAN_PWM_SECOND))
    {
        if (fullUpdate || message->hasChanged(MAIN_ATTR_U8::SETTING_FAN_PWM_SECOND))
        {
            publishi(MAIN_SUBTOPIC, MAIN_SETTING_PWM_02, message->getAttr(MAIN_ATTR_U8::SETTING_FAN_PWM_SECOND));
        }
    }

    if (message->hasAttr(MAIN_ATTR_U8::SETTING_FAN_PWM_THIRD))
    {
        if (fullUpdate || message->hasChanged(MAIN_ATTR_U8::SETTING_FAN_PWM_THIRD))
        {
            publishi(MAIN_SUBTOPIC, MAIN_SETTING_PWM_03, message->getAttr(MAIN_ATTR_U8::SETTING_FAN_PWM_THIRD));
        }
    }

    if (message->hasAttr(MAIN_ATTR_U8::SETTING_MIN_TARGET_WATER_TEMPERATURE))
    {
        if (fullUpdate || message->hasChanged(MAIN_ATTR_U8::SETTING_MIN_TARGET_WATER_TEMPERATURE))
        {
            publishi(
                    MAIN_SUBTOPIC,
                    MAIN_SETTING_MIN_TEMP_TARGET,
                    message->getAttr(MAIN_ATTR_U8::SETTING_MIN_TARGET_WATER_TEMPERATURE));
        }
    }

    if (message->hasAttr(MAIN_ATTR_U8::SETTING_LEGIONELLA_TARGET_WATER_TEMPERATURE))
    {
        if (fullUpdate || message->hasChanged(MAIN_ATTR_U8::SETTING_LEGIONELLA_TARGET_WATER_TEMPERATURE))
        {
            publishi(
                    MAIN_SUBTOPIC,
                    MAIN_SETTING_MIN_TEMP_LEGIONELLA,
                    message->getAttr(MAIN_ATTR_U8::SETTING_LEGIONELLA_TARGET_WATER_TEMPERATURE));
        }
    }

    if (message->hasAttr(MAIN_ATTR_U16::SETTING_HEAT_ELEMENT_WATTAGE))
    {
        if (fullUpdate || message->hasChanged(MAIN_ATTR_U16::SETTING_HEAT_ELEMENT_WATTAGE))
        {
            publishi(
                    MAIN_SUBTOPIC,
                    MAIN_SETTING_PWR_HEATELEM,
                    message->getAttr(MAIN_ATTR_U16::SETTING_HEAT_ELEMENT_WATTAGE));
        }
    }

    if (message->hasAttr(MAIN_ATTR_U16::SETTING_BOILER_CAPACITY))
    {
        if (fullUpdate || message->hasChanged(MAIN_ATTR_U16::SETTING_BOILER_CAPACITY))
        {
            publishi(MAIN_SUBTOPIC, MAIN_SETTING_BOILER_CAP, message->getAttr(MAIN_ATTR_U16::SETTING_BOILER_CAPACITY));
        }
    }

    if (message->hasAttr(MAIN_ATTR_U8::SETTING_BRAND))
    {
        if (fullUpdate || message->hasChanged(MAIN_ATTR_U8::SETTING_BRAND))
        {
            publishString(
                    MAIN_SUBTOPIC,
                    MAIN_SETTING_BOILER_BRAND,
                    brandStr(static_cast<MAINBrands>(message->getAttr(MAIN_ATTR_U16::SETTING_BOILER_CAPACITY))));
        }
    }

    if (message->hasAttr(MAIN_ATTR_BOOL::CAPABILITY_HAS_ANTI_DRY_HEATING))
    {
        if (fullUpdate || message->hasChanged(MAIN_ATTR_BOOL::CAPABILITY_HAS_ANTI_DRY_HEATING))
        {
            publishi(
                    MAIN_SUBTOPIC,
                    MAIN_CAPABILITY_DRY_HEATING,
                    message->getAttr(MAIN_ATTR_BOOL::CAPABILITY_HAS_ANTI_DRY_HEATING));
        }
    }

    if (message->hasAttr(MAIN_ATTR_BOOL::CAPABILITY_HAS_CIRCULATION))
    {
        if (fullUpdate || message->hasChanged(MAIN_ATTR_BOOL::CAPABILITY_HAS_CIRCULATION))
        {
            publishi(
                    MAIN_SUBTOPIC,
                    MAIN_CAPABILITY_CIRCULATION,
                    message->getAttr(MAIN_ATTR_BOOL::CAPABILITY_HAS_CIRCULATION));
        }
    }

    if (message->hasAttr(MAIN_ATTR_BOOL::CAPABILITY_HAS_PV_INPUT))
    {
        if (fullUpdate || message->hasChanged(MAIN_ATTR_BOOL::CAPABILITY_HAS_PV_INPUT))
        {
            publishi(
                    MAIN_SUBTOPIC,
                    MAIN_CAPABILITY_PV_INPUT,
                    message->getAttr(MAIN_ATTR_BOOL::CAPABILITY_HAS_PV_INPUT));
        }
    }

    if (message->hasAttr(MAIN_ATTR_BOOL::CAPABILITY_HAS_COMMUNICATION))
    {
        if (fullUpdate || message->hasChanged(MAIN_ATTR_BOOL::CAPABILITY_HAS_COMMUNICATION))
        {
            publishi(
                    MAIN_SUBTOPIC,
                    MAIN_CAPABILITY_EXT_COMM,
                    message->getAttr(MAIN_ATTR_BOOL::CAPABILITY_HAS_COMMUNICATION));
        }
    }

    if (message->hasAttr(MAIN_ATTR_BOOL::CAPABILITY_HAS_HEAT_EXCHANGER))
    {
        if (fullUpdate || message->hasChanged(MAIN_ATTR_BOOL::CAPABILITY_HAS_HEAT_EXCHANGER))
        {
            publishi(
                    MAIN_SUBTOPIC,
                    MAIN_CAPABILITY_HEAT_EXC,
                    message->getAttr(MAIN_ATTR_BOOL::CAPABILITY_HAS_HEAT_EXCHANGER));
        }
    }

    if (message->hasAttr(MAIN_ATTR_U8::ERROR_CODE))
    {
        if (fullUpdate || message->hasChanged(MAIN_ATTR_U8::ERROR_CODE))
        {
            publishi(MAIN_SUBTOPIC, MAIN_ERROR_CODE, message->getAttr(MAIN_ATTR_U8::ERROR_CODE));
        }
    }

    if (config::DEBUG_RAW_SERIAL_MESSAGES)
    {
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%s%s%s",
                config::mqttPrefix,
                BASE_TOPIC,
                MAIN_SUBTOPIC,
                DEBUG);

        toHexStr(mTransferBuffer, message->getLength(), reinterpret_cast<char*>(mPayloadBuffer));
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
    }
}

void MQTTTask::updateHMIStatus(bool fullUpdate, message::ProtocolVersion& version)
{
    std::unique_ptr<message::IHMIMessage> message;
    if (version == ProtocolVersion::PROTOCOL_NEXT)
    {
        message = std::make_unique<message::next::HMIMessage>(
                mTransferBuffer,
                fullUpdate ? nullptr : mLastProcessedHMIMessage);
    }
    else
    {
        message = std::make_unique<message::legacy::HMIMessage>(
                mTransferBuffer,
                fullUpdate ? nullptr : mLastProcessedHMIMessage);
    }

    if (message->hasAttr(HMI_ATTR_FLOAT::WATER_TARGET_TEMPERATURE))
    {
        if (fullUpdate || message->hasChanged(HMI_ATTR_FLOAT::WATER_TARGET_TEMPERATURE))
        {
            publishFloat(
                    HMI_SUBTOPIC,
                    HMI_HOT_WATER_TEMP_TARGET,
                    message->getAttr(HMI_ATTR_FLOAT::WATER_TARGET_TEMPERATURE));
        }
    }

    if (message->hasAttr(HMI_ATTR_U8::OPERATION_MODE))
    {
        if (fullUpdate || message->hasChanged(HMI_ATTR_U8::OPERATION_MODE))
        {
            publishString(
                    HMI_SUBTOPIC,
                    HMI_OPERATION_MODE,
                    operationModeStr(static_cast<HMIOperationMode>(message->getAttr(HMI_ATTR_U8::OPERATION_MODE))));
        }
    }
    if (message->hasAttr(HMI_ATTR_U8::OPERATION_TYPE))
    {
        if (fullUpdate || message->hasChanged(HMI_ATTR_U8::OPERATION_TYPE))
        {
            publishString(
                    HMI_SUBTOPIC,
                    HMI_OPERATION_TYPE,
                    operationTypeStr(static_cast<HMIOperationType>(message->getAttr(HMI_ATTR_U8::OPERATION_TYPE))));
        }
    }

    if (config::MQTT_PUBLISH_HEATPUMP_TIME_AND_DATE)
    {
        if (message->hasAttr(HMI_ATTR_U8::TIME_SECONDS) && message->hasAttr(HMI_ATTR_U8::TIME_MINUTES)
            && message->hasAttr(HMI_ATTR_U8::TIME_SECONDS))
        {
            if (fullUpdate || message->hasChanged(HMI_ATTR_U8::TIME_SECONDS)
                || message->hasChanged(HMI_ATTR_U8::TIME_MINUTES) || message->hasChanged(HMI_ATTR_U8::TIME_HOURS))
            {
                sprintf(reinterpret_cast<char*>(mPayloadBuffer),
                        "%02d:%02d:%02d",
                        message->getAttr(HMI_ATTR_U8::TIME_HOURS),
                        message->getAttr(HMI_ATTR_U8::TIME_MINUTES),
                        message->getAttr(HMI_ATTR_U8::TIME_SECONDS));
                sprintf(reinterpret_cast<char*>(mTopicBuffer),
                        "%s%s%s%s",
                        config::mqttPrefix,
                        BASE_TOPIC,
                        HMI_SUBTOPIC,
                        HMI_TIME);
                mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
            }
        }

        if (message->hasAttr(HMI_ATTR_U16::DATE_YEAR) && message->hasAttr(HMI_ATTR_U8::DATE_MONTH)
            && message->hasAttr(HMI_ATTR_U8::DATE_DAY))
        {
            if (fullUpdate || message->hasChanged(HMI_ATTR_U16::DATE_YEAR)
                || message->hasChanged(HMI_ATTR_U8::DATE_MONTH) || message->hasChanged(HMI_ATTR_U8::DATE_DAY))
            {
                sprintf(reinterpret_cast<char*>(mPayloadBuffer),
                        "%d.%d.%d",
                        message->getAttr(HMI_ATTR_U8::DATE_DAY),
                        message->getAttr(HMI_ATTR_U8::DATE_MONTH),
                        message->getAttr(HMI_ATTR_U16::DATE_YEAR));
                sprintf(reinterpret_cast<char*>(mTopicBuffer),
                        "%s%s%s%s",
                        config::mqttPrefix,
                        BASE_TOPIC,
                        HMI_SUBTOPIC,
                        HMI_DATE);
                mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
            }
        }
    }

    if (message->hasAttr(HMI_ATTR_BOOL::EMERGENCY_MODE_ENABLED))
    {
        if (fullUpdate || message->hasChanged(HMI_ATTR_BOOL::EMERGENCY_MODE_ENABLED))
        {
            publishi(HMI_SUBTOPIC, HMI_EMERGENCY_MODE, message->getAttr(HMI_ATTR_BOOL::EMERGENCY_MODE_ENABLED));
        }
    }

    if (message->hasAttr(HMI_ATTR_BOOL::HEATING_ELEMENT_ALLOWED))
    {
        if (fullUpdate || message->hasChanged(HMI_ATTR_BOOL::HEATING_ELEMENT_ALLOWED))
        {
            publishi(
                    HMI_SUBTOPIC,
                    HMI_HEATING_ELEMENT_ENABLED,
                    message->getAttr(HMI_ATTR_BOOL::HEATING_ELEMENT_ALLOWED));
        }
    }

    if (message->hasAttr(HMI_ATTR_BOOL::PV_INPUT_ALLOWED))
    {
        if (fullUpdate || message->hasChanged(HMI_ATTR_BOOL::PV_INPUT_ALLOWED))
        {
            publishi(HMI_SUBTOPIC, HMM_PV_INPUT_ACTIVATED, message->getAttr(HMI_ATTR_BOOL::PV_INPUT_ALLOWED));
        }
    }

    if (message->hasAttr(HMI_ATTR_U8::STATE_SETUP))
    {
        if (fullUpdate || message->hasChanged(HMI_ATTR_U8::STATE_SETUP))
        {
            publishString(
                    HMI_SUBTOPIC,
                    HMI_SETUP_STATE,
                    setupStr(static_cast<HMISetup>(message->getAttr(HMI_ATTR_U8::STATE_SETUP))));
        }
    }

    if (message->hasAttr(HMI_ATTR_U8::ANTI_LEGIONELLA_CYCLES))
    {
        if (fullUpdate || message->hasChanged(HMI_ATTR_U8::ANTI_LEGIONELLA_CYCLES))
        {
            publishi(HMI_SUBTOPIC, HMI_LEGIONELLA, message->getAttr(HMI_ATTR_U8::ANTI_LEGIONELLA_CYCLES));
        }
    }

    if (message->hasAttr(HMI_ATTR_U8::CONFIG_AIRDUCT))
    {
        if (fullUpdate || message->hasChanged(HMI_ATTR_U8::CONFIG_AIRDUCT))
        {
            publishString(
                    HMI_SUBTOPIC,
                    HMI_AIR_DUCT_CONFIG,
                    airDuctConfigStr(static_cast<HMIAirDuctConfig>(message->getAttr(HMI_ATTR_U8::CONFIG_AIRDUCT))));
        }
    }

    if (message->hasAttr(HMI_ATTR_U8::STATE_TEST))
    {
        if (fullUpdate || message->hasChanged(HMI_ATTR_U8::STATE_TEST))
        {
            publishString(
                    HMI_SUBTOPIC,
                    HMI_TEST_MODE,
                    testModeStr(static_cast<HMITestMode>(message->getAttr(HMI_ATTR_U8::STATE_TEST))));
        }
    }

    if (message->hasAttr(HMI_ATTR_U8::STATE_INSTALLATION_MODE))
    {
        if (fullUpdate || message->hasChanged(HMI_ATTR_U8::STATE_INSTALLATION_MODE))
        {
            publishString(
                    HMI_SUBTOPIC,
                    HMI_INSTALLATION_CONFIG,
                    installationModeStr(
                            static_cast<HMIInstallation>(message->getAttr(HMI_ATTR_U8::STATE_INSTALLATION_MODE))));
        }
    }

    if (message->hasAttr(HMI_ATTR_U8::CONFIG_FAN_EXHAUST))
    {
        if (fullUpdate || message->hasChanged(HMI_ATTR_U8::CONFIG_FAN_EXHAUST))
        {
            publishString(
                    HMI_SUBTOPIC,
                    HMI_FAN_EXHAUST_CONFIG,
                    exhaustModeStr(static_cast<HMIFanExhaust>(message->getAttr(HMI_ATTR_U8::CONFIG_FAN_EXHAUST))));
        }
    }

    if (message->hasAttr(HMI_ATTR_STR::TIMER_WINDOW_A))
    {
        if (fullUpdate || message->hasChanged(HMI_ATTR_STR::TIMER_WINDOW_A))
        {
            sprintf(reinterpret_cast<char*>(mTopicBuffer),
                    "%s%s%s%s",
                    config::mqttPrefix,
                    BASE_TOPIC,
                    HMI_SUBTOPIC,
                    HMI_TIMER_WINDOW_A);
            message->getAttr(HMI_ATTR_STR::TIMER_WINDOW_A, reinterpret_cast<char*>(mPayloadBuffer));
            mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
        }
    }

    if (message->hasAttr(HMI_ATTR_STR::TIMER_WINDOW_B))
    {
        if (fullUpdate || message->hasChanged(HMI_ATTR_STR::TIMER_WINDOW_B))
        {
            sprintf(reinterpret_cast<char*>(mTopicBuffer),
                    "%s%s%s%s",
                    config::mqttPrefix,
                    BASE_TOPIC,
                    HMI_SUBTOPIC,
                    HMI_TIMER_WINDOW_B);
            message->getAttr(HMI_ATTR_STR::TIMER_WINDOW_B, reinterpret_cast<char*>(mPayloadBuffer));
            mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
        }
    }

    // publish the error number to the request id
    if (message->hasAttr(HMI_ATTR_U8::HMI_ERROR_ID_REQUESTED) && message->hasAttr(HMI_ATTR_U8::HMI_ERROR_NO_REQUESTED))
    {
        if (message->hasChanged(HMI_ATTR_U8::HMI_ERROR_ID_REQUESTED)
            && message->getAttr(HMI_ATTR_U8::HMI_ERROR_ID_REQUESTED) != 0)
        {
            sprintf(reinterpret_cast<char*>(mTopicBuffer),
                    "%s%s%s%u/%s",
                    config::mqttPrefix,
                    BASE_TOPIC,
                    ERROR_SUBTOPIC,
                    message->getAttr(HMI_ATTR_U8::HMI_ERROR_ID_REQUESTED),
                    ERROR_ERROR_NUMBER);
            itoa(message->getAttr(HMI_ATTR_U8::HMI_ERROR_NO_REQUESTED), reinterpret_cast<char*>(mPayloadBuffer), 10);
            mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
        }
    }

    if (config::DEBUG_RAW_SERIAL_MESSAGES)
    {
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%s%s%s",
                config::mqttPrefix,
                BASE_TOPIC,
                HMI_SUBTOPIC,
                DEBUG);

        toHexStr(mTransferBuffer, message->getLength(), reinterpret_cast<char*>(mPayloadBuffer));
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
    }
}

void MQTTTask::updateEnergyStats(bool fullUpdate, message::ProtocolVersion& version)
{
    std::unique_ptr<message::IEnergyMessage> message;
    if (version == ProtocolVersion::PROTOCOL_NEXT)
    {
        message = std::make_unique<message::next::MainEnergyMessage>(
                mTransferBuffer,
                fullUpdate ? nullptr : mLastProcessedHMIMessage);
    }
    else
    {
        message = std::make_unique<message::legacy::MainEnergyMessage>(
                mTransferBuffer,
                fullUpdate ? nullptr : mLastProcessedHMIMessage);
    }

    if (message->hasAttr(ENERGY_ATTR_U32::TOTAL_HEATPUMP_HOURS))
    {
        if (fullUpdate || message->hasChanged(ENERGY_ATTR_U32::TOTAL_HEATPUMP_HOURS))
        {
            publishul(
                    ENERGY_SUBTOPIC,
                    ENERGY_TOTAL_HEATPUMP_HOURS,
                    message->getAttr(ENERGY_ATTR_U32::TOTAL_HEATPUMP_HOURS),
                    true);
        }
    }

    if (message->hasAttr(ENERGY_ATTR_U32::TOTAL_HEATING_ELEMENT_HOURS))
    {
        if (fullUpdate || message->hasChanged(ENERGY_ATTR_U32::TOTAL_HEATING_ELEMENT_HOURS))
        {
            publishul(
                    ENERGY_SUBTOPIC,
                    ENERGY_TOTAL_HEATING_ELEM_HOURS,
                    message->getAttr(ENERGY_ATTR_U32::TOTAL_HEATING_ELEMENT_HOURS),
                    true);
        }
    }

    if (message->hasAttr(ENERGY_ATTR_U32::TOTAL_HOURS))
    {
        if (fullUpdate || message->hasChanged(ENERGY_ATTR_U32::TOTAL_HOURS))
        {
            publishul(ENERGY_SUBTOPIC, ENERGY_TOTAL_HOURS, message->getAttr(ENERGY_ATTR_U32::TOTAL_HOURS), true);
        }
    }
    if (message->hasAttr(ENERGY_ATTR_U64::TOTAL_ENERGY))
    {
        if (fullUpdate || message->hasChanged(ENERGY_ATTR_U64::TOTAL_ENERGY))
        {
            publishul(ENERGY_SUBTOPIC, ENERGY_TOTAL_ENERGY_WH, message->getAttr(ENERGY_ATTR_U64::TOTAL_ENERGY), true);
        }
    }

    if (message->hasAttr(ENERGY_ATTR_U16::POWER_HEATPUMP))
    {
        if (fullUpdate || message->hasChanged(ENERGY_ATTR_U16::POWER_HEATPUMP))
        {
            publishul(ENERGY_SUBTOPIC, ENERGY_POWER_HEATPUMP, message->getAttr(ENERGY_ATTR_U16::POWER_HEATPUMP));
            if (strlen(optionalPublishTopicHeatPumpCurrentPower) != 0)
            {
                sprintf(reinterpret_cast<char*>(mTopicBuffer), "%s", optionalPublishTopicHeatPumpCurrentPower);
                mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
            }
        }
    }

    if (message->hasAttr(ENERGY_ATTR_U16::POWER_HEATELEMENT)
        && message->hasChanged(ENERGY_ATTR_U16::POWER_HEATELEMENT))
    {
        publishul(ENERGY_SUBTOPIC, ENERGY_POWER_HEAT_ELEMENT, message->getAttr(ENERGY_ATTR_U16::POWER_HEATELEMENT));
        if (strlen(optionalPublishTopicHeatElementCurrentPower) != 0)
        {
            sprintf(reinterpret_cast<char*>(mTopicBuffer), "%s", optionalPublishTopicHeatElementCurrentPower);
            mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
        }
    }

    if (message->hasAttr(ENERGY_ATTR_U16::POWER_TOTAL))
    {
        if (fullUpdate || message->hasChanged(ENERGY_ATTR_U16::POWER_TOTAL))
        {
            publishul(ENERGY_SUBTOPIC, ENERGY_POWER_TOTAL, message->getAttr(ENERGY_ATTR_U16::POWER_TOTAL));
        }
    }

    if (message->hasAttr(ENERGY_ATTR_U16::WATER_TOTAL))
    {
        if (fullUpdate || message->hasChanged(ENERGY_ATTR_U16::WATER_TOTAL))
        {
            publishul(ENERGY_SUBTOPIC, ENERGY_TOTAL_WATER_PRODUCTION, message->getAttr(ENERGY_ATTR_U16::WATER_TOTAL));
        }
    }

    if (config::DEBUG_RAW_SERIAL_MESSAGES)
    {
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%s%s%s",
                config::mqttPrefix,
                BASE_TOPIC,
                ENERGY_SUBTOPIC,
                DEBUG);

        toHexStr(mTransferBuffer, message->getLength(), reinterpret_cast<char*>(mPayloadBuffer));
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
    }
}

void MQTTTask::updateErrorStatus(message::ProtocolVersion& version)
{
    std::unique_ptr<message::IErrorMessage> message;
    if (version == ProtocolVersion::PROTOCOL_NEXT)
    {
        message = std::make_unique<message::next::ErrorMessage>(mTransferBuffer);
    }
    else
    {
        message = std::make_unique<message::legacy::ErrorMessage>(mTransferBuffer);
    }

    if (message->isEmpty())
    {
        return;
    }

    if (message->hasAttr(message::ERROR_ATTR_U8::ERROR_REQUEST_ID)
        && message->hasAttr(message::ERROR_ATTR_U8::ERROR_ERROR_CODE))
    {
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%s%s%u/%s",
                config::mqttPrefix,
                BASE_TOPIC,
                ERROR_SUBTOPIC,
                message->getAttr(ERROR_ATTR_U8::ERROR_REQUEST_ID),
                MAIN_ERROR_CODE);
        itoa(message->getAttr(ERROR_ATTR_U8::ERROR_ERROR_CODE), reinterpret_cast<char*>(mPayloadBuffer), 10);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
    }

    if (!message->isEmpty() && message->hasAttr(message::ERROR_ATTR_U8::ERROR_REQUEST_ID))
    {
        if (message->hasAttr(message::ERROR_ATTR_U8::ERROR_DATE_DAY)
            && message->hasAttr(message::ERROR_ATTR_U8::ERROR_DATE_MONTH)
            && message->hasAttr(message::ERROR_ATTR_U8::ERROR_DATE_YEAR))
        {

            sprintf(reinterpret_cast<char*>(mPayloadBuffer),
                    "%d.%d.%d",
                    message->getAttr(message::ERROR_ATTR_U8::ERROR_DATE_DAY),
                    message->getAttr(message::ERROR_ATTR_U8::ERROR_DATE_MONTH),
                    message->getAttr(message::ERROR_ATTR_U8::ERROR_DATE_YEAR));
            sprintf(reinterpret_cast<char*>(mTopicBuffer),
                    "%s%s%s%u/%s",
                    config::mqttPrefix,
                    BASE_TOPIC,
                    ERROR_SUBTOPIC,
                    message->getAttr(message::ERROR_ATTR_U8::ERROR_REQUEST_ID),
                    HMI_DATE);
            mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
        }

        if (message->hasAttr(message::ERROR_ATTR_U8::ERROR_TIME_HOURS)
            && message->hasAttr(message::ERROR_ATTR_U8::ERROR_TIME_MINUTES))
        {
            sprintf(reinterpret_cast<char*>(mPayloadBuffer),
                    "%02d:%02d",
                    message->getAttr(message::ERROR_ATTR_U8::ERROR_TIME_HOURS),
                    message->getAttr(message::ERROR_ATTR_U8::ERROR_TIME_MINUTES));
            sprintf(reinterpret_cast<char*>(mTopicBuffer),
                    "%s%s%s%u/%s",
                    config::mqttPrefix,
                    BASE_TOPIC,
                    ERROR_SUBTOPIC,
                    message->getAttr(message::ERROR_ATTR_U8::ERROR_REQUEST_ID),
                    HMI_TIME);
            mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
        }

        if (message->hasAttr(message::ERROR_ATTR_FLOAT::ERROR_WATER_TEMPERATURE))
        {
            dtostrf(message->getAttr(message::ERROR_ATTR_FLOAT::ERROR_WATER_TEMPERATURE),
                    3,
                    1,
                    reinterpret_cast<char*>(mPayloadBuffer));
            sprintf(reinterpret_cast<char*>(mTopicBuffer),
                    "%s%s%s%u/%s",
                    config::mqttPrefix,
                    BASE_TOPIC,
                    ERROR_SUBTOPIC,
                    message->getAttr(message::ERROR_ATTR_U8::ERROR_REQUEST_ID),
                    MAIN_HOT_WATER_TEMP);
            mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
        }

        if (message->hasAttr(ERROR_ATTR_FLOAT::ERROR_AIR_TEMPERATURE))
        {
            dtostrf(message->getAttr(ERROR_ATTR_FLOAT::ERROR_AIR_TEMPERATURE),
                    3,
                    1,
                    reinterpret_cast<char*>(mPayloadBuffer));
            sprintf(reinterpret_cast<char*>(mTopicBuffer),
                    "%s%s%s%u/%s",
                    config::mqttPrefix,
                    BASE_TOPIC,
                    ERROR_SUBTOPIC,
                    message->getAttr(message::ERROR_ATTR_U8::ERROR_REQUEST_ID),
                    MAIN_SUPPLY_AIR_TEMP);
            mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
        }

        if (message->hasAttr(ERROR_ATTR_FLOAT::ERROR_EVAPORATOR_LOWER_TEMPERATURE))
        {
            dtostrf(message->getAttr(ERROR_ATTR_FLOAT::ERROR_EVAPORATOR_LOWER_TEMPERATURE),
                    3,
                    1,
                    reinterpret_cast<char*>(mPayloadBuffer));
            sprintf(reinterpret_cast<char*>(mTopicBuffer),
                    "%s%s%s%u/%s",
                    config::mqttPrefix,
                    BASE_TOPIC,
                    ERROR_SUBTOPIC,
                    message->getAttr(message::ERROR_ATTR_U8::ERROR_REQUEST_ID),
                    MAIN_EVAPORATOR_AIR_TEMP_LOWER);
            mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
        }

        if (message->hasAttr(ERROR_ATTR_FLOAT::ERROR_EVAPORATOR_UPPER_TEMPERATURE))
        {
            dtostrf(message->getAttr(ERROR_ATTR_FLOAT::ERROR_EVAPORATOR_UPPER_TEMPERATURE),
                    3,
                    1,
                    reinterpret_cast<char*>(mPayloadBuffer));
            sprintf(reinterpret_cast<char*>(mTopicBuffer),
                    "%s%s%s%u/%s",
                    config::mqttPrefix,
                    BASE_TOPIC,
                    ERROR_SUBTOPIC,
                    message->getAttr(message::ERROR_ATTR_U8::ERROR_REQUEST_ID),
                    MAIN_EVAPORATOR_AIR_TEMP_UPPER);
            mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
        }

        if (message->hasAttr(ERROR_ATTR_FLOAT::ERROR_FAN_SPEED_PWM))
        {
            dtostrf(message->getAttr(ERROR_ATTR_FLOAT::ERROR_FAN_SPEED_PWM),
                    3,
                    1,
                    reinterpret_cast<char*>(mPayloadBuffer));
            sprintf(reinterpret_cast<char*>(mTopicBuffer),
                    "%s%s%s%u/%s",
                    config::mqttPrefix,
                    BASE_TOPIC,
                    ERROR_SUBTOPIC,
                    message->getAttr(message::ERROR_ATTR_U8::ERROR_REQUEST_ID),
                    MAIN_FAN_PWM);
            mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
        }

        if (message->hasAttr(ERROR_ATTR_U16::ERROR_TOTAL_HEATPUMP_HOURS))
        {
            ultoa(message->getAttr(ERROR_ATTR_U16::ERROR_TOTAL_HEATPUMP_HOURS),
                  reinterpret_cast<char*>(mPayloadBuffer),
                  10);
            sprintf(reinterpret_cast<char*>(mTopicBuffer),
                    "%s%s%s%u/%s",
                    config::mqttPrefix,
                    BASE_TOPIC,
                    ERROR_SUBTOPIC,
                    message->getAttr(message::ERROR_ATTR_U8::ERROR_REQUEST_ID),
                    ENERGY_TOTAL_HEATPUMP_HOURS);
            mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
        }

        if (message->hasAttr(ERROR_ATTR_U16::ERROR_TOTAL_HEATING_ELEMENT_HOURS))
        {
            ultoa(message->getAttr(ERROR_ATTR_U16::ERROR_TOTAL_HEATING_ELEMENT_HOURS),
                  reinterpret_cast<char*>(mPayloadBuffer),
                  10);
            sprintf(reinterpret_cast<char*>(mTopicBuffer),
                    "%s%s%s%u/%s",
                    config::mqttPrefix,
                    BASE_TOPIC,
                    ERROR_SUBTOPIC,
                    message->getAttr(message::ERROR_ATTR_U8::ERROR_REQUEST_ID),
                    ENERGY_TOTAL_HEATING_ELEM_HOURS);
            mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
        }
    }

    if (config::DEBUG_RAW_SERIAL_MESSAGES)
    {
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%s%s%s",
                config::mqttPrefix,
                BASE_TOPIC,
                ERROR_SUBTOPIC,
                DEBUG);

        toHexStr(mTransferBuffer, message->getLength(), reinterpret_cast<char*>(mPayloadBuffer));
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
    }
}

void MQTTTask::publishFloat(const char* subtopic, const char* topic, float value, bool retained)
{
    dtostrf(value, 3, 1, reinterpret_cast<char*>(mPayloadBuffer));
    sprintf(reinterpret_cast<char*>(mTopicBuffer), "%s%s%s%s", config::mqttPrefix, BASE_TOPIC, subtopic, topic);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer), retained, 0);
}

void MQTTTask::publishString(const char* subtopic, const char* topic, const char* value, bool retained)
{
    sprintf(reinterpret_cast<char*>(mTopicBuffer), "%s%s%s%s", config::mqttPrefix, BASE_TOPIC, subtopic, topic);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), value, retained, 0);
}

void MQTTTask::publishi(const char* subtopic, const char* topic, int value, bool retained)
{
    itoa(value, reinterpret_cast<char*>(mPayloadBuffer), 10);
    sprintf(reinterpret_cast<char*>(mTopicBuffer), "%s%s%s%s", config::mqttPrefix, BASE_TOPIC, subtopic, topic);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer), retained, 0);
}

void MQTTTask::publishul(const char* subtopic, const char* topic, unsigned long value, bool retained)
{
    ultoa(value, reinterpret_cast<char*>(mPayloadBuffer), 10);
    sprintf(reinterpret_cast<char*>(mTopicBuffer), "%s%s%s%s", config::mqttPrefix, BASE_TOPIC, subtopic, topic);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer), retained, 0);
}

void MQTTTask::publishul(
        const char*   subtopic_1,
        const char*   subtopic_2,
        const char*   topic,
        unsigned long value,
        bool          retained)
{
    ultoa(value, reinterpret_cast<char*>(mPayloadBuffer), 10);
    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%s%s%s%s",
            config::mqttPrefix,
            BASE_TOPIC,
            subtopic_1,
            subtopic_2,
            topic);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer), retained, 0);
}

void MQTTTask::sendHomeassistantDiscovery()
{
    if (!config::ENABLE_HOMEASSISTANT_DISCOVERY_MODE || mPublishedDiscovery)
    {
        return;
    }

    // cannot send discovery yet, protocol is still unknown
    ProtocolVersion protocolVersion = DHWState::getInstance().getVersion();
    if (protocolVersion == PROTOCOL_UNKNOWN)
    {
        return;
    }

    unsigned long long mac = ESP.getEfuseMac();
    FastCRC16          crc;
    uint16_t           identifier = crc.ccitt(reinterpret_cast<uint8_t*>(&mac), sizeof(mac));

    publishDiscovery(identifier, protocolVersion, "sensor", discovery::MQTT_ITEM_SENSOR::RESERVED_COUNT);
    publishDiscovery(identifier, protocolVersion, "binary_sensor", discovery::MQTT_ITEM_BINARY_SENSOR::RESERVED_COUNT);
    publishDiscovery(identifier, protocolVersion, "number", discovery::MQTT_ITEM_NUMBER::RESERVED_COUNT);
    publishDiscovery(identifier, protocolVersion, "button", discovery::MQTT_ITEM_BUTTON::RESERVED_COUNT);
    publishDiscovery(identifier, protocolVersion, "switch", discovery::MQTT_ITEM_SWITCH::RESERVED_COUNT);
    publishDiscovery(identifier, protocolVersion, "select", discovery::MQTT_ITEM_SELECT::RESERVED_COUNT);
    publishDiscovery(identifier, protocolVersion, "water_heater", discovery::MQTT_ITEM_WATER_HEATER::RESERVED_COUNT);

    mPublishedDiscovery = true;
}

template <typename T>
void MQTTTask::publishDiscovery(uint16_t identifier, ProtocolVersion protocolVersion, const char* haCategory, T)
{
    for (int item = 0; item < static_cast<int>(T::RESERVED_COUNT); ++item)
    {
        if (discovery::buildConfiguration(mPayloadBuffer, protocolVersion, identifier, static_cast<T>(item)))
        {
            sprintf(reinterpret_cast<char*>(mTopicBuffer),
                    "%s%s/aquamqtt_%u/%u/config",
                    config::haDiscoveryPrefix,
                    haCategory,
                    identifier,
                    item);

            mMQTTClient
                    .publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer), true, 0);

            mMQTTClient.loop();
        }
    }
}

void MQTTTask::publishFiltered(
        std::unique_ptr<IMainMessage>& message,
        MAIN_ATTR_FLOAT                attribute,
        SimpleKalmanFilter&            filter,
        float&                         mFilteredValue,
        const char*                    topic,
        bool                           fullUpdate)
{

    if (message->hasAttr(attribute))
    {
        float rawValue              = message->getAttr(attribute);
        auto  previousFilteredValue = mFilteredValue;
        mFilteredValue              = std::round(filter.updateEstimate(rawValue) * 10.0f) / 10.0f;

        if (fullUpdate)
        {
            publishFloat(
                    MAIN_SUBTOPIC,
                    topic,
                    config::MQTT_FILTER_TEMPERATURE_NOISE ? mFilteredValue : rawValue);
        }
        else if (!config::MQTT_FILTER_TEMPERATURE_NOISE && message->hasChanged(attribute))
        {
            publishFloat(MAIN_SUBTOPIC, topic, rawValue);
        }
        else if (config::MQTT_FILTER_TEMPERATURE_NOISE && previousFilteredValue != mFilteredValue)
        {
            publishFloat(MAIN_SUBTOPIC, topic, mFilteredValue);
        }
    }
}

}  // namespace aquamqtt
