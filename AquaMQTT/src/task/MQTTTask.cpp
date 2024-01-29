#include "task/MQTTTask.h"

#include <WiFi.h>
#include <esp_task_wdt.h>

#include "config/Configuration.h"
#include "mqtt/MQTTDefinitions.h"
#include "state/HMIStateProxy.h"

namespace aquamqtt
{

using namespace mqtt;
using namespace message;

MQTTTask::MQTTTask()
    : mLastFullUpdate(0)
    , mMQTTClient(256)
    , mTransferBuffer{ 0 }
    , mTaskHandle(nullptr)
    , mTopicBuffer{ 0 }
    , mPayloadBuffer{ 0 }
{
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
    else if (strstr_P(topic.c_str(), AQUAMQTT_RESET_OVERRIDES) != nullptr)
    {
        HMIStateProxy::getInstance().onResetOverrides();
    }
}

void MQTTTask::spawn()
{
    xTaskCreatePinnedToCore(MQTTTask::innerTask, "mqttTaskLoop", 9000, this, 3, &mTaskHandle, 1);
    esp_task_wdt_add(mTaskHandle);
    DHWState::getInstance().setListener(mTaskHandle);
    HMIStateProxy::getInstance().setListener(mTaskHandle);
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
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            STATS_SUBTOPIC,
            STATS_AQUAMQTT_LAST_WILL);
    sprintf(reinterpret_cast<char*>(mPayloadBuffer), "%S", ENUM_LAST_WILL_OFFLINE);

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
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            STATS_SUBTOPIC,
            STATS_AQUAMQTT_LAST_WILL);

    sprintf(reinterpret_cast<char*>(mPayloadBuffer), "%S", ENUM_LAST_WILL_ONLINE);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer), true, 0);

    sprintf(reinterpret_cast<char*>(mTopicBuffer), "%s%S", config::mqttPrefix, CONTROL_TOPIC);
    mMQTTClient.subscribe(reinterpret_cast<char*>(mTopicBuffer));

    Serial.println("[mqtt] is now connected");
}

void MQTTTask::loop()
{
    check_mqtt_connection();

    auto mqttCycle = pdMS_TO_TICKS(5);

    bool statsUpdate = (millis() - mLastFullUpdate) >= config::MQTT_STATS_UPDATE_MS;

    auto notify = ulTaskNotifyTake(pdTRUE, mqttCycle);

    if ((notify & 1 << 8) != 0)
    {
        if (HMIStateProxy::getInstance().copyFrame(aquamqtt::message::HMI_MESSAGE_IDENTIFIER, mTransferBuffer))
        {
            updateHMIStatus();
        }
    }

    if ((notify & 1 << 7) != 0)
    {
        if (DHWState::getInstance().copyFrame(aquamqtt::message::MAIN_MESSAGE_IDENTIFIER, mTransferBuffer))
        {
            updateMainStatus();
        }
    }

    if ((notify & 1 << 6) != 0)
    {
        if (DHWState::getInstance().copyFrame(aquamqtt::message::ENERGY_MESSAGE_IDENTIFIER, mTransferBuffer))
        {
            updateEnergyStats();
        }
    }

    if (statsUpdate)
    {
        updateStats();
    }

    if (statsUpdate)
    {
        Serial.println("[mqtt] stat update");
        mLastFullUpdate = millis();
        Serial.print("[mqtt]: stack size (words)");
        Serial.println(uxTaskGetStackHighWaterMark(nullptr));
    }

    mMQTTClient.loop();
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"
void                     MQTTTask::updateStats()
{
    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            STATS_SUBTOPIC,
            STATS_AQUAMQTT_MODE);
    sprintf(reinterpret_cast<char*>(mPayloadBuffer),
            "%S",
            config::OPERATION_MODE == config::EOperationMode::LISTENER ? ENUM_AQUAMQTT_MODE_LISTENER
                                                                       : ENUM_AQUAMQTT_MODE_MITM);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            STATS_SUBTOPIC,
            STATS_AQUAMQTT_ADDR);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), WiFi.localIP().toString());

    itoa(WiFi.RSSI(), reinterpret_cast<char*>(mPayloadBuffer), 10);
    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            STATS_SUBTOPIC,
            STATS_AQUAMQTT_RSSI);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

    if (config::OPERATION_MODE == config::EOperationMode::LISTENER)
    {
        auto listenerStats = DHWState::getInstance().getFrameBufferStatistics(0);

        ultoa(listenerStats.msgHandled, reinterpret_cast<char*>(mPayloadBuffer), 10);
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%S%S%S",
                config::mqttPrefix,
                BASE_TOPIC,
                STATS_SUBTOPIC,
                STATS_MSG_HANDLED);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

        ultoa(listenerStats.msgUnhandled, reinterpret_cast<char*>(mPayloadBuffer), 10);
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%S%S%S",
                config::mqttPrefix,
                BASE_TOPIC,
                STATS_SUBTOPIC,
                STATS_MSG_UNHANDLED);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

        ultoa(listenerStats.msgCRCFail, reinterpret_cast<char*>(mPayloadBuffer), 10);
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%S%S%S",
                config::mqttPrefix,
                BASE_TOPIC,
                STATS_SUBTOPIC,
                STATS_MSG_CRC_NOK);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

        ultoa(listenerStats.droppedBytes, reinterpret_cast<char*>(mPayloadBuffer), 10);
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%S%S%S",
                config::mqttPrefix,
                BASE_TOPIC,
                STATS_SUBTOPIC,
                STATS_DROPPED_BYTES);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
    }
    else
    {
        auto hmiStats = DHWState::getInstance().getFrameBufferStatistics(1);

        ultoa(hmiStats.msgHandled, reinterpret_cast<char*>(mPayloadBuffer), 10);
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%S%S%S%S",
                config::mqttPrefix,
                BASE_TOPIC,
                STATS_SUBTOPIC,
                HMI_SUBTOPIC,
                STATS_MSG_HANDLED);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

        ultoa(hmiStats.msgUnhandled, reinterpret_cast<char*>(mPayloadBuffer), 10);
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%S%S%S%S",
                config::mqttPrefix,
                BASE_TOPIC,
                STATS_SUBTOPIC,
                HMI_SUBTOPIC,
                STATS_MSG_UNHANDLED);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

        ultoa(hmiStats.msgCRCFail, reinterpret_cast<char*>(mPayloadBuffer), 10);
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%S%S%S%S",
                config::mqttPrefix,
                BASE_TOPIC,
                STATS_SUBTOPIC,
                HMI_SUBTOPIC,
                STATS_MSG_CRC_NOK);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

        ultoa(hmiStats.droppedBytes, reinterpret_cast<char*>(mPayloadBuffer), 10);
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%S%S%S%S",
                config::mqttPrefix,
                BASE_TOPIC,
                STATS_SUBTOPIC,
                HMI_SUBTOPIC,
                STATS_DROPPED_BYTES);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

        ultoa(hmiStats.msgSent, reinterpret_cast<char*>(mPayloadBuffer), 10);
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%S%S%S%S",
                config::mqttPrefix,
                BASE_TOPIC,
                STATS_SUBTOPIC,
                HMI_SUBTOPIC,
                STATS_MSG_SENT);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

        auto mainStats = DHWState::getInstance().getFrameBufferStatistics(2);

        ultoa(mainStats.msgHandled, reinterpret_cast<char*>(mPayloadBuffer), 10);
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%S%S%S%S",
                config::mqttPrefix,
                BASE_TOPIC,
                STATS_SUBTOPIC,
                MAIN_SUBTOPIC,
                STATS_MSG_HANDLED);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

        ultoa(mainStats.msgUnhandled, reinterpret_cast<char*>(mPayloadBuffer), 10);
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%S%S%S%S",
                config::mqttPrefix,
                BASE_TOPIC,
                STATS_SUBTOPIC,
                MAIN_SUBTOPIC,
                STATS_MSG_UNHANDLED);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

        ultoa(mainStats.msgCRCFail, reinterpret_cast<char*>(mPayloadBuffer), 10);
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%S%S%S%S",
                config::mqttPrefix,
                BASE_TOPIC,
                STATS_SUBTOPIC,
                MAIN_SUBTOPIC,
                STATS_MSG_CRC_NOK);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

        ultoa(mainStats.droppedBytes, reinterpret_cast<char*>(mPayloadBuffer), 10);
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%S%S%S%S",
                config::mqttPrefix,
                BASE_TOPIC,
                STATS_SUBTOPIC,
                MAIN_SUBTOPIC,
                STATS_DROPPED_BYTES);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

        ultoa(mainStats.msgSent, reinterpret_cast<char*>(mPayloadBuffer), 10);
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%S%S%S%S",
                config::mqttPrefix,
                BASE_TOPIC,
                STATS_SUBTOPIC,
                MAIN_SUBTOPIC,
                STATS_MSG_SENT);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

        auto overrides = HMIStateProxy::getInstance().getOverrides();
        auto offset    = sprintf(reinterpret_cast<char*>(mPayloadBuffer), "[");
        if (overrides.operationMode)
        {
            offset += sprintf(reinterpret_cast<char*>(mPayloadBuffer) + offset, "\"%S\"", HMI_OPERATION_MODE);
            if (overrides.waterTempTarget || aquamqtt::config::OVERRIDE_TIME_AND_DATE_IN_MITM)
            {
                offset += sprintf(reinterpret_cast<char*>(mPayloadBuffer) + offset, ",");
            }
        }

        if (overrides.waterTempTarget)
        {
            offset += sprintf(reinterpret_cast<char*>(mPayloadBuffer) + offset, "\"%S\"", HMI_HOT_WATER_TEMP_TARGET);
            if (aquamqtt::config::OVERRIDE_TIME_AND_DATE_IN_MITM)
            {
                offset += sprintf(reinterpret_cast<char*>(mPayloadBuffer) + offset, ",");
            }
        }

        if (aquamqtt::config::OVERRIDE_TIME_AND_DATE_IN_MITM)
        {
            offset += sprintf(reinterpret_cast<char*>(mPayloadBuffer) + offset, "\"%S\"", HMI_TIME_AND_DATE);
        }
        sprintf(reinterpret_cast<char*>(mPayloadBuffer) + offset, "]");
        sprintf(reinterpret_cast<char*>(mTopicBuffer),
                "%s%S%S%S",
                config::mqttPrefix,
                BASE_TOPIC,
                STATS_SUBTOPIC,
                STATS_ACTIVE_OVERRIDES);
        mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
    }
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"
void                     MQTTTask::updateMainStatus()
{
    message::MainStatusMessage message(mTransferBuffer);

    dtostrf(message.hotWaterTemp(), 3, 1, reinterpret_cast<char*>(mPayloadBuffer));
    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            MAIN_SUBTOPIC,
            MAIN_HOT_WATER_TEMP);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

    dtostrf(message.airTemp(), 3, 1, reinterpret_cast<char*>(mPayloadBuffer));
    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            MAIN_SUBTOPIC,
            MAIN_SUPPLY_AIR_TEMP);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

    dtostrf(message.evaporatorLowerAirTemp(), 3, 1, reinterpret_cast<char*>(mPayloadBuffer));
    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            MAIN_SUBTOPIC,
            MAIN_EVAPORATOR_AIR_TEMP_LOWER);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

    dtostrf(message.evaporatorUpperAirTemp(), 3, 1, reinterpret_cast<char*>(mPayloadBuffer));
    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            MAIN_SUBTOPIC,
            MAIN_EVAPORATOR_AIR_TEMP_UPPER);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

    ultoa(message.fanSpeed(), reinterpret_cast<char*>(mPayloadBuffer), 10);
    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            MAIN_SUBTOPIC,
            MAIN_FAN_SPEED);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

    itoa(message.stateHeatingElement(), reinterpret_cast<char*>(mPayloadBuffer), 10);
    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            MAIN_SUBTOPIC,
            MAIN_STATE_HEAT_ELEMENT);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

    itoa(message.stateHeatpump(), reinterpret_cast<char*>(mPayloadBuffer), 10);
    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            MAIN_SUBTOPIC,
            MAIN_STATE_HEATPUMP);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

    itoa(message.stateBoilerBackup(), reinterpret_cast<char*>(mPayloadBuffer), 10);
    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            MAIN_SUBTOPIC,
            MAIN_STATE_EXT_BOILER);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

    itoa(message.stateFan(), reinterpret_cast<char*>(mPayloadBuffer), 10);
    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            MAIN_SUBTOPIC,
            MAIN_STATE_FAN);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

    itoa(message.stateDefrost(), reinterpret_cast<char*>(mPayloadBuffer), 10);
    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            MAIN_SUBTOPIC,
            MAIN_STATE_DEFROST);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"
void                     MQTTTask::updateHMIStatus()
{
    message::HMIMessage message(mTransferBuffer);

    dtostrf(message.waterTempTarget(), 3, 1, reinterpret_cast<char*>(mPayloadBuffer));
    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            HMI_SUBTOPIC,
            HMI_HOT_WATER_TEMP_TARGET);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            HMI_SUBTOPIC,
            HMI_OPERATION_MODE);
    mMQTTClient.publish(
            reinterpret_cast<char*>(mTopicBuffer),
            aquamqtt::message::operationModeStr(message.operationMode()));

    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            HMI_SUBTOPIC,
            HMI_OPERATION_TYPE);
    if (message.isTimerModeEnabled())
    {
        sprintf(reinterpret_cast<char*>(mPayloadBuffer), "%S", ENUM_OPERATION_TYPE_TIMER);
    }
    else
    {
        sprintf(reinterpret_cast<char*>(mPayloadBuffer), "%S", ENUM_OPERATION_TYPE_ALWAYS_ON);
    }
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

    sprintf(reinterpret_cast<char*>(mPayloadBuffer),
            "%02d:%02d:%02d",
            message.timeHours(),
            message.timeMinutes(),
            message.timeSeconds());
    sprintf(reinterpret_cast<char*>(mTopicBuffer), "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, HMI_SUBTOPIC, HMI_TIME);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

    sprintf(reinterpret_cast<char*>(mPayloadBuffer),
            "%d.%d.%d",
            message.dateDay(),
            message.dateMonth(),
            message.dateYear());
    sprintf(reinterpret_cast<char*>(mTopicBuffer), "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, HMI_SUBTOPIC, HMI_DATE);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

    itoa(message.isEmergencyModeEnabled(), reinterpret_cast<char*>(mPayloadBuffer), 10);
    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            HMI_SUBTOPIC,
            HMI_EMERGENCY_MODE);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

    itoa(message.isHeatingElementEnabled(), reinterpret_cast<char*>(mPayloadBuffer), 10);
    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            HMI_SUBTOPIC,
            HMI_HEATING_ELEMENT_ENABLED);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

    itoa(message.antiLegionellaModePerMonth(), reinterpret_cast<char*>(mPayloadBuffer), 10);
    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            HMI_SUBTOPIC,
            HMI_LEGIONELLA);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            HMI_SUBTOPIC,
            HMI_AIR_DUCT_CONFIG);
    mMQTTClient.publish(
            reinterpret_cast<char*>(mTopicBuffer),
            aquamqtt::message::airDuctConfigStr(message.airDuctConfig()));

    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            HMI_SUBTOPIC,
            HMI_TEST_MODE);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), testModeStr(message.testMode()));

    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            HMI_SUBTOPIC,
            HMI_INSTALLATION_CONFIG);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), installationModeStr(message.installationMode()));

    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            HMI_SUBTOPIC,
            HMI_SETUP_STATE);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), setupStr(message.setupMode()));

    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            HMI_SUBTOPIC,
            HMI_TIMER_WINDOW_A);
    message.timerWindowStr(true, reinterpret_cast<char*>(mPayloadBuffer));
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            HMI_SUBTOPIC,
            HMI_TIMER_WINDOW_B);
    message.timerWindowStr(false, reinterpret_cast<char*>(mPayloadBuffer));
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"
void                     MQTTTask::updateEnergyStats()
{
    message::MainEnergyMessage message(mTransferBuffer);

    ultoa(message.totalHeatpumpHours(), reinterpret_cast<char*>(mPayloadBuffer), 10);
    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            ENERGY_SUBTOPIC,
            ENERGY_TOTAL_HEATPUMP_HOURS);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer), true, 0);

    ultoa(message.totalHeatingElemHours(), reinterpret_cast<char*>(mPayloadBuffer), 10);
    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            ENERGY_SUBTOPIC,
            ENERGY_TOTAL_HEATING_ELEM_HOURS);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer), true, 0);

    ultoa(message.totalHours(), reinterpret_cast<char*>(mPayloadBuffer), 10);
    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            ENERGY_SUBTOPIC,
            ENERGY_TOTAL_HOURS);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer), true, 0);

    ultoa(message.totalEnergyCounter(), reinterpret_cast<char*>(mPayloadBuffer), 10);
    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            ENERGY_SUBTOPIC,
            ENERGY_TOTAL_ENERGY_WH);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer), true, 0);

    ultoa(message.powerHeatpump(), reinterpret_cast<char*>(mPayloadBuffer), 10);
    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            ENERGY_SUBTOPIC,
            ENERGY_POWER_HEATPUMP);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

    ultoa(message.powerHeatElement(), reinterpret_cast<char*>(mPayloadBuffer), 10);
    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            ENERGY_SUBTOPIC,
            ENERGY_POWER_HEAT_ELEMENT);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));

    ultoa(message.powerOverall(), reinterpret_cast<char*>(mPayloadBuffer), 10);
    sprintf(reinterpret_cast<char*>(mTopicBuffer),
            "%s%S%S%S",
            config::mqttPrefix,
            BASE_TOPIC,
            ENERGY_SUBTOPIC,
            ENERGY_POWER_TOTAL);
    mMQTTClient.publish(reinterpret_cast<char*>(mTopicBuffer), reinterpret_cast<char*>(mPayloadBuffer));
}

}  // namespace aquamqtt
