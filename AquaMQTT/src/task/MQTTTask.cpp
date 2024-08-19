#include "task/MQTTTask.h"

#include <WiFi.h>
#include <esp_task_wdt.h>

#include "config/Configuration.h"
#include "message/ErrorMessage.h"
#include "mqtt/MQTTDefinitions.h"
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
            HMIStateProxy::getInstance().onOperationModeChanged(
                    std::unique_ptr<HMIOperationMode>(new HMIOperationMode(OM_ABSENCE)));
        }

        else if (strstr_P(payload.c_str(), ENUM_OPERATION_MODE_BOOST) != nullptr)
        {
            HMIStateProxy::getInstance().onOperationModeChanged(
                    std::unique_ptr<HMIOperationMode>(new HMIOperationMode(OM_BOOST)));
        }

        else if (strstr_P(payload.c_str(), ENUM_OPERATION_MODE_AUTO) != nullptr)
        {
            HMIStateProxy::getInstance().onOperationModeChanged(
                    std::unique_ptr<HMIOperationMode>(new HMIOperationMode(OM_AUTO)));
        }

        else if (strstr_P(payload.c_str(), ENUM_OPERATION_MODE_ECO_ON) != nullptr)
        {
            HMIStateProxy::getInstance().onOperationModeChanged(
                    std::unique_ptr<HMIOperationMode>(new HMIOperationMode(OM_ECO_ACTIVE)));
        }

        else if (strstr_P(payload.c_str(), ENUM_OPERATION_MODE_ECO_OFF) != nullptr)
        {
            HMIStateProxy::getInstance().onOperationModeChanged(
                    std::unique_ptr<HMIOperationMode>(new HMIOperationMode(OM_ECO_INACTIVE)));
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
            HMIStateProxy::getInstance().onOperationTypeChanged(
                    std::unique_ptr<HMIOperationType>(new HMIOperationType(TIMER)));
        }
        else if (strstr_P(payload.c_str(), ENUM_OPERATION_TYPE_ALWAYS_ON) != nullptr)
        {
            HMIStateProxy::getInstance().onOperationTypeChanged(
                    std::unique_ptr<HMIOperationType>(new HMIOperationType(ALWAYS_ON)));
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
            HMIStateProxy::getInstance().onEmergencyModeEnabledChanged(std::unique_ptr<bool>(new bool(result)));
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
            HMIStateProxy::getInstance().onHeatingElementEnabledChanged(std::unique_ptr<bool>(new bool(result)));
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
            HMIStateProxy::getInstance().onWaterTempTargetChanged(
                    std::unique_ptr<float>(new float(atof(payload.c_str()))));
        }
    }
    else if (strstr_P(topic.c_str(), HMI_INSTALLATION_CONFIG) != nullptr)
    {
        if (strstr_P(payload.c_str(), ENUM_INSTALLATION_THERMODYNAMICS_ONLY) != nullptr)
        {
            HMIStateProxy::getInstance().onInstallationModeChanged(
                    std::unique_ptr<HMIInstallation>(new HMIInstallation(INST_HP_ONLY)));
        }
        else if (strstr_P(payload.c_str(), ENUM_INSTALLATION_SOLAR_BACKUP) != nullptr)
        {
            HMIStateProxy::getInstance().onInstallationModeChanged(
                    std::unique_ptr<HMIInstallation>(new HMIInstallation(INST_HP_AND_SOLAR)));
        }

        else if (strstr_P(payload.c_str(), ENUM_INSTALLATION_BOILER_BACKUP_EXT_OPT) != nullptr)
        {
            HMIStateProxy::getInstance().onInstallationModeChanged(
                    std::unique_ptr<HMIInstallation>(new HMIInstallation(INST_HP_AND_EXT_OPT_EXT)));
        }

        else if (strstr_P(payload.c_str(), ENUM_INSTALLATION_BOILER_BACKUP_EXT_PRIO) != nullptr)
        {
            HMIStateProxy::getInstance().onInstallationModeChanged(
                    std::unique_ptr<HMIInstallation>(new HMIInstallation(INST_HP_AND_EXT_PRIO_EXT)));
        }

        else if (strstr_P(payload.c_str(), ENUM_INSTALLATION_BOILER_BACKUP_HP_OPT) != nullptr)
        {
            HMIStateProxy::getInstance().onInstallationModeChanged(
                    std::unique_ptr<HMIInstallation>(new HMIInstallation(INST_HP_AND_EXT_OPT_HP)));
        }

        else if (strstr_P(payload.c_str(), ENUM_INSTALLATION_BOILER_BACKUP_HP_PRIO) != nullptr)
        {
            HMIStateProxy::getInstance().onInstallationModeChanged(
                    std::unique_ptr<HMIInstallation>(new HMIInstallation(INST_HP_AND_EXT_PRIO_HP)));
        }
        else
        {
            HMIStateProxy::getInstance().onInstallationModeChanged(std::unique_ptr<HMIInstallation>(nullptr));
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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"
void                     MQTTTask::setup()
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
            aquamqtt::config::brokerClientId,
            strlen(aquamqtt::config::brokerUser) == 0 ? nullptr : aquamqtt::config::brokerUser,
            strlen(aquamqtt::config::brokerPassword) == 0 ? nullptr : aquamqtt::config::brokerPassword))
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        esp_task_wdt_reset();
    }

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

    bool fullUpdate = (millis() - mLastFullUpdate) >= config::MQTT_FULL_UPDATE_MS;

    bool statsUpdate = (millis() - mLastStatsUpdate) >= config::MQTT_STATS_UPDATE_MS;

    auto notify = ulTaskNotifyTake(pdTRUE, mqttCycle);

    if ((notify & 1 << 8) != 0 || fullUpdate)
    {
        if (HMIStateProxy::getInstance().copyFrame(aquamqtt::message::HMI_MESSAGE_IDENTIFIER, mTransferBuffer))
        {
            updateHMIStatus(fullUpdate);

            if (mLastProcessedHMIMessage == nullptr)
            {
                mLastProcessedHMIMessage = new uint8_t[aquamqtt::message::HMI_MESSAGE_LENGTH];
            }
            memcpy(mLastProcessedHMIMessage, mTransferBuffer, aquamqtt::message::HMI_MESSAGE_LENGTH);
        }
    }

    if ((notify & 1 << 7) != 0 || fullUpdate)
    {
        if (MainStateProxy::getInstance().copyFrame(aquamqtt::message::MAIN_MESSAGE_IDENTIFIER, mTransferBuffer))
        {
            updateMainStatus(fullUpdate);

            if (mLastProcessedMainMessage == nullptr)
            {
                mLastProcessedMainMessage = new uint8_t[aquamqtt::message::MAIN_MESSAGE_LENGTH];
            }
            memcpy(mLastProcessedMainMessage, mTransferBuffer, aquamqtt::message::MAIN_MESSAGE_LENGTH);
        }
    }

    if ((notify & 1 << 6) != 0 || fullUpdate)
    {
        if (MainStateProxy::getInstance().copyFrame(aquamqtt::message::ENERGY_MESSAGE_IDENTIFIER, mTransferBuffer))
        {
            updateEnergyStats(fullUpdate);

            if (mLastProcessedEnergyMessage == nullptr)
            {
                mLastProcessedEnergyMessage = new uint8_t[aquamqtt::message::ENERGY_MESSAGE_LENGTH];
            }
            memcpy(mLastProcessedEnergyMessage, mTransferBuffer, aquamqtt::message::ENERGY_MESSAGE_LENGTH);
        }
    }

    if ((notify & 1 << 5) != 0)
    {
        if (MainStateProxy::getInstance().copyFrame(aquamqtt::message::ERROR_MESSAGE_IDENTIFIER, mTransferBuffer))
        {
            updateErrorStatus();
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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"
void                     MQTTTask::updateStats()
{
    publishString(
            STATS_SUBTOPIC,
            STATS_AQUAMQTT_MODE,
            config::OPERATION_MODE == config::EOperationMode::LISTENER ? ENUM_AQUAMQTT_MODE_LISTENER
                                                                       : ENUM_AQUAMQTT_MODE_MITM);

    publishString(STATS_SUBTOPIC, STATS_AQUAMQTT_ADDR, WiFi.localIP().toString().c_str());

    publishi(STATS_SUBTOPIC, STATS_AQUAMQTT_RSSI, WiFi.RSSI());

    if (config::OPERATION_MODE == config::EOperationMode::LISTENER)
    {
        auto listenerStats = DHWState::getInstance().getFrameBufferStatistics(0);
        publishul(STATS_SUBTOPIC, STATS_MSG_HANDLED, listenerStats.msgHandled);
        publishul(STATS_SUBTOPIC, STATS_MSG_UNHANDLED, listenerStats.msgUnhandled);
        publishul(STATS_SUBTOPIC, STATS_MSG_CRC_NOK, listenerStats.msgCRCFail);
        publishul(STATS_SUBTOPIC, STATS_DROPPED_BYTES, listenerStats.droppedBytes);
    }
    else
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

        auto overrides = HMIStateProxy::getInstance().getOverrides();

        sprintf(reinterpret_cast<char*>(mPayloadBuffer),
                R"({ "%s": %s, "%s": %s, "%s": %s, "%s": %s, "%s": %s, "%s": %s , "%s": %s })",
                HMI_OPERATION_MODE,
                overrides.operationMode ? "1" : "0",
                HMI_OPERATION_TYPE,
                overrides.operationType ? "1" : "0",
                HMI_HOT_WATER_TEMP_TARGET,
                overrides.waterTempTarget ? "1" : "0",
                HMI_HEATING_ELEMENT_ENABLED,
                overrides.heatingElementEnabled ? "1" : "0",
                HMI_EMERGENCY_MODE,
                overrides.emergencyModeEnabled ? "1" : "0",
                HMI_INSTALLATION_CONFIG,
                overrides.installationMode ? "1" : "0",
                HMI_TIME_AND_DATE,
                (aquamqtt::config::OVERRIDE_TIME_AND_DATE_IN_MITM
                 && aquamqtt::config::OPERATION_MODE != aquamqtt::config::EOperationMode::LISTENER)
                        ? "1"
                        : "0");

        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%s%s%s",
                config::mqttPrefix,
                BASE_TOPIC,
                STATS_SUBTOPIC,
                STATS_ACTIVE_OVERRIDES);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

        auto mainOverrides = MainStateProxy::getInstance().getOverrides();
        sprintf(reinterpret_cast<char*>(mPayloadBuffer),
                R"({ "%s": %s, "%s": %s })",
                MAIN_STATE_PV,
                mainOverrides.pvState ? "1" : "0",
                MAIN_STATE_SOLAR,
                mainOverrides.solarState ? "1" : "0");

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
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"
void                     MQTTTask::updateMainStatus(bool fullUpdate)
{
    message::MainStatusMessage message(mTransferBuffer);
    message.compareWith(fullUpdate ? nullptr : mLastProcessedMainMessage);

    if (message.hotWaterTempChanged())
    {
        publishFloat(MAIN_SUBTOPIC, MAIN_HOT_WATER_TEMP, message.hotWaterTemp());
    }
    if (message.airTempChanged())
    {
        publishFloat(MAIN_SUBTOPIC, MAIN_SUPPLY_AIR_TEMP, message.airTemp());
    }
    if (message.evaporatorLowerAirTempChanged())
    {
        publishFloat(MAIN_SUBTOPIC, MAIN_EVAPORATOR_AIR_TEMP_LOWER, message.evaporatorLowerAirTemp());
    }
    if (message.evaporatorUpperAirTempChanged())
    {
        publishFloat(MAIN_SUBTOPIC, MAIN_EVAPORATOR_AIR_TEMP_UPPER, message.evaporatorUpperAirTemp());
    }
    if (message.fanSpeedChanged())
    {
        publishFloat(MAIN_SUBTOPIC, MAIN_FAN_PWM, message.fanSpeedPwm());
    }
    if (message.statesChanged())
    {
        publishi(MAIN_SUBTOPIC, MAIN_STATE_HEAT_ELEMENT, message.stateHeatingElement());
        publishi(MAIN_SUBTOPIC, MAIN_STATE_HEATPUMP, message.stateHeatpump());
        publishi(MAIN_SUBTOPIC, MAIN_STATE_EXT_BOILER, message.stateBoilerBackup());
        publishi(MAIN_SUBTOPIC, MAIN_STATE_FAN, message.stateFan());
        publishi(MAIN_SUBTOPIC, MAIN_STATE_DEFROST, message.stateDefrost());
    }

    if (message.statePVOrSolarChanged())
    {
        publishi(MAIN_SUBTOPIC, MAIN_STATE_PV, message.statePV());
        publishi(MAIN_SUBTOPIC, MAIN_STATE_SOLAR, message.stateSolar());
    }

    if (message.settingPwmFirstChanged())
    {
        publishi(MAIN_SUBTOPIC, MAIN_SETTING_PWM_01, message.settingPwmFirst());
    }
    if (message.settingPwmSecondChanged())
    {
        publishi(MAIN_SUBTOPIC, MAIN_SETTING_PWM_02, message.settingPwmSecond());
    }
    if (message.settingPwmThirdChanged())
    {
        publishi(MAIN_SUBTOPIC, MAIN_SETTING_PWM_03, message.settingPwmThird());
    }
    if (message.settingMinTTargetChanged())
    {
        publishi(MAIN_SUBTOPIC, MAIN_SETTING_MIN_TEMP_TARGET, message.settingMinTTarget());
    }
    if (message.settingLegionellaTTargetChanged())
    {
        publishi(MAIN_SUBTOPIC, MAIN_SETTING_MIN_TEMP_LEGIONELLA, message.settingLegionellaTTarget());
    }
    if (message.settingWattageHeatingElementChanged())
    {
        publishi(MAIN_SUBTOPIC, MAIN_SETTING_PWR_HEATELEM, message.settingWattageHeatingElement());
    }
    if (message.settingBoilerCapacityChanged())
    {
        publishi(MAIN_SUBTOPIC, MAIN_SETTING_BOILER_CAP, message.settingBoilerCapacity());
    }
    if (message.settingBrandChanged())
    {
        publishString(MAIN_SUBTOPIC, MAIN_SETTING_BOILER_BRAND, brandStr(message.settingBrand()));
    }
    if (message.settingCapabilitiesChanged())
    {
        publishi(MAIN_SUBTOPIC, MAIN_CAPABILITY_HEAT_EXC, message.capabilityHasHeatExchanger());
        publishi(MAIN_SUBTOPIC, MAIN_CAPABILITY_CIRCULATION, message.capabilityHasCirculation());
        publishi(MAIN_SUBTOPIC, MAIN_CAPABILITY_PV_INPUT, message.capabilityHasPVInput());
        publishi(MAIN_SUBTOPIC, MAIN_CAPABILITY_EXT_COMM, message.capabilityHasCommunication());
        publishi(MAIN_SUBTOPIC, MAIN_CAPABILITY_DRY_HEATING, message.capabilityHasAntiDryHeating());
    }

    if (message.errorCodeChanged())
    {
        publishi(MAIN_SUBTOPIC, MAIN_ERROR_CODE, message.errorCode());
    }

    if (config::DEBUG_RAW_SERIAL_MESSAGES)
    {
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%s%s%s",
                config::mqttPrefix,
                BASE_TOPIC,
                MAIN_SUBTOPIC,
                DEBUG);

        toHexStr(mTransferBuffer, MAIN_MESSAGE_LENGTH, reinterpret_cast<char*>(mPayloadBuffer));
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
    }
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"
void                     MQTTTask::updateHMIStatus(bool fullUpdate)
{
    message::HMIMessage message(mTransferBuffer);
    message.compareWith(fullUpdate ? nullptr : mLastProcessedHMIMessage);

    if (message.waterTempTargetChanged())
    {
        publishFloat(HMI_SUBTOPIC, HMI_HOT_WATER_TEMP_TARGET, message.waterTempTarget());
    }

    if (message.operationTypeOrModeChanged())
    {
        publishString(HMI_SUBTOPIC, HMI_OPERATION_MODE, operationModeStr(message.operationMode()));

        publishString(HMI_SUBTOPIC, HMI_OPERATION_TYPE, operationTypeStr(message.getOperationType()));
    }

    if (message.timeChanged())
    {
        sprintf(reinterpret_cast<char*>(mPayloadBuffer),
                "%02d:%02d:%02d",
                message.timeHours(),
                message.timeMinutes(),
                message.timeSeconds());
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%s%s%s",
                config::mqttPrefix,
                BASE_TOPIC,
                HMI_SUBTOPIC,
                HMI_TIME);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
    }

    if (message.dateChanged())
    {
        sprintf(reinterpret_cast<char*>(mPayloadBuffer),
                "%d.%d.%d",
                message.dateDay(),
                message.dateMonth(),
                message.dateYear());
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%s%s%s",
                config::mqttPrefix,
                BASE_TOPIC,
                HMI_SUBTOPIC,
                HMI_DATE);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
    }

    if (message.emergencyModeChanged())
    {
        publishi(HMI_SUBTOPIC, HMI_EMERGENCY_MODE, message.isEmergencyModeEnabled());
    }

    if (message.heatingElemOrSetupStateOrPVActiveChanged())
    {
        publishi(HMI_SUBTOPIC, HMI_HEATING_ELEMENT_ENABLED, message.isHeatingElementEnabled());
        publishi(HMI_SUBTOPIC, HMM_PV_INPUT_ACTIVATED, message.isPVInputActivated());
        publishString(HMI_SUBTOPIC, HMI_SETUP_STATE, setupStr(message.setupMode()));
    }

    if (message.legionellaOrAirductChanged())
    {
        publishi(HMI_SUBTOPIC, HMI_LEGIONELLA, message.antiLegionellaModePerMonth());
        publishString(HMI_SUBTOPIC, HMI_AIR_DUCT_CONFIG, aquamqtt::message::airDuctConfigStr(message.airDuctConfig()));
    }

    if (message.testModeChanged())
    {
        publishString(HMI_SUBTOPIC, HMI_TEST_MODE, testModeStr(message.testMode()));
    }

    if (message.installationConfigChanged())
    {
        publishString(HMI_SUBTOPIC, HMI_INSTALLATION_CONFIG, installationModeStr(message.installationMode()));
    }

    if (message.timerModeOneChanged())
    {
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%s%s%s",
                config::mqttPrefix,
                BASE_TOPIC,
                HMI_SUBTOPIC,
                HMI_TIMER_WINDOW_A);
        message.timerWindowStr(true, reinterpret_cast<char*>(mPayloadBuffer));
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
    }

    if (message.timerModeTwoChanged())
    {
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%s%s%s",
                config::mqttPrefix,
                BASE_TOPIC,
                HMI_SUBTOPIC,
                HMI_TIMER_WINDOW_B);
        message.timerWindowStr(false, reinterpret_cast<char*>(mPayloadBuffer));
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
    }

    // publish the error number to the request id
    if (message.errorRequestChanged() && message.errorRequestId() != 0)
    {
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%s%s%u/%s",
                config::mqttPrefix,
                BASE_TOPIC,
                ERROR_SUBTOPIC,
                message.errorRequestId(),
                ERROR_ERROR_NUMBER);
        itoa(message.errorNumberRequested(), reinterpret_cast<char*>(mPayloadBuffer), 10);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
    }

    if (config::DEBUG_RAW_SERIAL_MESSAGES)
    {
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%s%s%s",
                config::mqttPrefix,
                BASE_TOPIC,
                HMI_SUBTOPIC,
                DEBUG);

        toHexStr(mTransferBuffer, HMI_MESSAGE_LENGTH, reinterpret_cast<char*>(mPayloadBuffer));
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
    }
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"
void                     MQTTTask::updateEnergyStats(bool fullUpdate)
{
    message::MainEnergyMessage message(mTransferBuffer);
    message.compareWith(fullUpdate ? nullptr : mLastProcessedEnergyMessage);

    if (message.totalHeatpumpHoursChanged())
    {
        publishul(ENERGY_SUBTOPIC, ENERGY_TOTAL_HEATPUMP_HOURS, message.totalHeatpumpHours(), true);
    }
    if (message.totalHeatingElemHoursChanged())
    {
        publishul(ENERGY_SUBTOPIC, ENERGY_TOTAL_HEATING_ELEM_HOURS, message.totalHeatingElemHours(), true);
    }
    if (message.totalHoursChanged())
    {
        publishul(ENERGY_SUBTOPIC, ENERGY_TOTAL_HOURS, message.totalHours(), true);
    }
    if (message.totalEnergyCounterChanged())
    {
        publishul(ENERGY_SUBTOPIC, ENERGY_TOTAL_ENERGY_WH, message.totalEnergyCounter(), true);
    }

    if (message.powerHeatpumpChanged())
    {
        publishul(ENERGY_SUBTOPIC, ENERGY_POWER_HEATPUMP, message.powerHeatpump());
        if (strlen(optionalPublishTopicHeatPumpCurrentPower) != 0)
        {
            sprintf(reinterpret_cast<char*>(mTopicBuffer), "%s", optionalPublishTopicHeatPumpCurrentPower);
            mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
        }
    }

    if (message.powerHeatElementChanged())
    {
        publishul(ENERGY_SUBTOPIC, ENERGY_POWER_HEAT_ELEMENT, message.powerHeatElement());
        if (strlen(optionalPublishTopicHeatElementCurrentPower) != 0)
        {
            sprintf(reinterpret_cast<char*>(mTopicBuffer), "%s", optionalPublishTopicHeatElementCurrentPower);
            mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
        }
    }

    if (message.powerOverallChanged())
    {
        publishul(ENERGY_SUBTOPIC, ENERGY_POWER_TOTAL, message.powerOverall());
    }

    if(message.totalWaterProductionChanged()) {
        publishul(ENERGY_SUBTOPIC, ENERGY_TOTAL_WATER_PRODUCTION, message.totalWaterProduction());
    }

    if (config::DEBUG_RAW_SERIAL_MESSAGES)
    {
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%s%s%s",
                config::mqttPrefix,
                BASE_TOPIC,
                ENERGY_SUBTOPIC,
                DEBUG);

        toHexStr(mTransferBuffer, ENERGY_MESSAGE_LENGTH, reinterpret_cast<char*>(mPayloadBuffer));
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
    }
}

void MQTTTask::updateErrorStatus()
{
    message::ErrorMessage message(mTransferBuffer);

    if (message.isEmpty())
    {
        return;
    }

    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%s%s%u/%s",
            config::mqttPrefix,
            BASE_TOPIC,
            ERROR_SUBTOPIC,
            message.errorRequestId(),
            MAIN_ERROR_CODE);
    itoa(message.errorCode(), reinterpret_cast<char*>(mPayloadBuffer), 10);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

    if (!message.isEmpty())
    {
        sprintf(reinterpret_cast<char*>(mPayloadBuffer),
                "%d.%d.%d",
                message.dateDay(),
                message.dateMonth(),
                message.dateYear());
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%s%s%u/%s",
                config::mqttPrefix,
                BASE_TOPIC,
                ERROR_SUBTOPIC,
                message.errorRequestId(),
                HMI_DATE);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

        sprintf(reinterpret_cast<char*>(mPayloadBuffer), "%02d:%02d", message.timeHours(), message.timeMinutes());
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%s%s%u/%s",
                config::mqttPrefix,
                BASE_TOPIC,
                ERROR_SUBTOPIC,
                message.errorRequestId(),
                HMI_TIME);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

        dtostrf(message.hotWaterTemp(), 3, 1, reinterpret_cast<char*>(mPayloadBuffer));
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%s%s%u/%s",
                config::mqttPrefix,
                BASE_TOPIC,
                ERROR_SUBTOPIC,
                message.errorRequestId(),
                MAIN_HOT_WATER_TEMP);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

        dtostrf(message.airTemp(), 3, 1, reinterpret_cast<char*>(mPayloadBuffer));
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%s%s%u/%s",
                config::mqttPrefix,
                BASE_TOPIC,
                ERROR_SUBTOPIC,
                message.errorRequestId(),
                MAIN_SUPPLY_AIR_TEMP);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

        dtostrf(message.evaporatorLowerAirTemp(), 3, 1, reinterpret_cast<char*>(mPayloadBuffer));
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%s%s%u/%s",
                config::mqttPrefix,
                BASE_TOPIC,
                ERROR_SUBTOPIC,
                message.errorRequestId(),
                MAIN_EVAPORATOR_AIR_TEMP_LOWER);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

        dtostrf(message.evaporatorUpperAirTemp(), 3, 1, reinterpret_cast<char*>(mPayloadBuffer));
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%s%s%u/%s",
                config::mqttPrefix,
                BASE_TOPIC,
                ERROR_SUBTOPIC,
                message.errorRequestId(),
                MAIN_EVAPORATOR_AIR_TEMP_UPPER);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

        dtostrf(message.fanSpeedPwm(), 3, 1, reinterpret_cast<char*>(mPayloadBuffer));
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%s%s%u/%s",
                config::mqttPrefix,
                BASE_TOPIC,
                ERROR_SUBTOPIC,
                message.errorRequestId(),
                MAIN_FAN_PWM);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

        ultoa(message.totalHeatpumpHours(), reinterpret_cast<char*>(mPayloadBuffer), 10);
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%s%s%u/%s",
                config::mqttPrefix,
                BASE_TOPIC,
                ERROR_SUBTOPIC,
                message.errorRequestId(),
                ENERGY_TOTAL_HEATPUMP_HOURS);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

        ultoa(message.totalHeatingElemHours(), reinterpret_cast<char*>(mPayloadBuffer), 10);
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%s%s%u/%s",
                config::mqttPrefix,
                BASE_TOPIC,
                ERROR_SUBTOPIC,
                message.errorRequestId(),
                ENERGY_TOTAL_HEATING_ELEM_HOURS);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
    }

    if (config::DEBUG_RAW_SERIAL_MESSAGES)
    {
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%s%s%s",
                config::mqttPrefix,
                BASE_TOPIC,
                ERROR_SUBTOPIC,
                DEBUG);

        toHexStr(mTransferBuffer, ERROR_MESSAGE_LENGTH, reinterpret_cast<char*>(mPayloadBuffer));
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

}  // namespace aquamqtt
