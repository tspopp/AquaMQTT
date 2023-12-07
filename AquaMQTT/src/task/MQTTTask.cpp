

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

MQTTTask::MQTTTask() : mLastFullUpdate(0), mMQTTClient(128), mTransferBuffer{ 0 }
{
}

void MQTTTask::messageReceived(String& topic, String& payload)
{
    Serial.println("incoming: " + topic + " - " + payload);

    // Note: Do not use the client in the callback to publish, subscribe or
    // unsubscribe as it may cause deadlocks when other things arrive while
    // sending and receiving acknowledgments. Instead, change a global variable,
    // or push to a queue and handle it in the loop after calling `client.loop()`.

    if (strstr_P(topic.c_str(), OPERATION_MODE) != nullptr)
    {
        if (strstr_P(payload.c_str(), ENUM_OPERATION_MODE_ABSENCE) != nullptr)
        {
            HMIStateProxy::getInstance().onOperationModeChanged(
                    std::unique_ptr<HMIOperationMode>(new HMIOperationMode(ABSENCE)));
        }

        else if (strstr_P(payload.c_str(), ENUM_OPERATION_MODE_BOOST) != nullptr)
        {
            HMIStateProxy::getInstance().onOperationModeChanged(
                    std::unique_ptr<HMIOperationMode>(new HMIOperationMode(BOOST)));
        }

        else if (strstr_P(payload.c_str(), ENUM_OPERATION_MODE_AUTO) != nullptr)
        {
            HMIStateProxy::getInstance().onOperationModeChanged(
                    std::unique_ptr<HMIOperationMode>(new HMIOperationMode(AUTO)));
        }

        else if (strstr_P(payload.c_str(), ENUM_OPERATION_MODE_ECO_ON) != nullptr)
        {
            HMIStateProxy::getInstance().onOperationModeChanged(
                    std::unique_ptr<HMIOperationMode>(new HMIOperationMode(ECO_ACTIVE)));
        }

        else if (strstr_P(payload.c_str(), ENUM_OPERATION_MODE_ECO_OFF) != nullptr)
        {
            HMIStateProxy::getInstance().onOperationModeChanged(
                    std::unique_ptr<HMIOperationMode>(new HMIOperationMode(ECO_INACTIVE)));
        }
        else
        {
            HMIStateProxy::getInstance().onOperationModeChanged(std::unique_ptr<HMIOperationMode>(nullptr));
        }
    }

    else if (strstr_P(topic.c_str(), HOT_WATER_TEMP_TARGET) != nullptr)
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
}

void MQTTTask::spawn()
{
    xTaskCreatePinnedToCore(MQTTTask::innerTask, "mqttTaskLoop", 8000, this, 3, &mTaskHandle, 1);
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

void MQTTTask::setup()
{
    char topic[50];
    char payloadStr[20];

    mMQTTClient.begin(aquamqtt::config::brokerAddr, aquamqtt::config::brokerPort, mWiFiClient);
    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, STATS_SUBTOPIC, AQUAWIN_LAST_WILL);
    sprintf(payloadStr, "%S", AQUAWIN_LAST_WILL_OFFLINE);

    mMQTTClient.setWill(topic, payloadStr, true, 0);
    mMQTTClient.onMessage(MQTTTask::messageReceived);

    while (!mMQTTClient.connect(
            aquamqtt::config::brokerClientId,
            strlen(aquamqtt::config::brokerUser) == 0 ? nullptr : aquamqtt::config::brokerUser,
            strlen(aquamqtt::config::brokerPassword) == 0 ? nullptr : aquamqtt::config::brokerPassword))
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        esp_task_wdt_reset();
    }

    // TODO: add prefix!
    mMQTTClient.subscribe(aquamqtt::mqtt::CONTROL_TOPIC);

    sprintf(payloadStr, "%S", AQUAWIN_LAST_WILL_ONLINE);
    mMQTTClient.publish(topic, payloadStr, true, 0);

    Serial.println("[mqtt] is now connected");
}

void MQTTTask::loop()
{
    if (!mMQTTClient.connected())
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        return;
    }

    auto mqttCycle = pdMS_TO_TICKS(5);

    bool statsUpdate = (millis() - mLastFullUpdate) >= config::MQTT_STATS_UPDATE_MS;

    auto notify = ulTaskNotifyTake(pdFALSE, mqttCycle);

    if ((notify & 1 << 8) != 0)
    {
        if (HMIStateProxy::getInstance().copyFrame(aquamqtt::message::HMI_MESSAGE_IDENTIFIER, mTransferBuffer))
        {
            updateHMIStatus();
        }
        ulTaskNotifyValueClear(mTaskHandle, (1UL << 8UL));
        return;
    }

    if ((notify & 1 << 7) != 0)
    {
        if (DHWState::getInstance().copyFrame(aquamqtt::message::MAIN_MESSAGE_IDENTIFIER, mTransferBuffer))
        {
            updateMainStatus();
        }
        ulTaskNotifyValueClear(mTaskHandle, (1UL << 7UL));
        return;
    }

    if ((notify & 1 << 6) != 0)
    {
        if (DHWState::getInstance().copyFrame(aquamqtt::message::ENERGY_MESSAGE_IDENTIFIER, mTransferBuffer))
        {
            updateEnergyStats();
        }
        ulTaskNotifyValueClear(mTaskHandle, (1UL << 6UL));
        return;
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

void MQTTTask::updateStats()
{
    char topic[50];
    char payloadStr[20];

    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, STATS_SUBTOPIC, AQUAWIN_MODE);
    sprintf(payloadStr,
            "%S",
            config::OPERATION_MODE == EOperationMode::LISTENER ? AQUAWIN_MODE_LISTENER : AQUAWIN_MODE_MITM);
    mMQTTClient.publish(topic, payloadStr);

    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, STATS_SUBTOPIC, AQUAWIN_ADDR);
    mMQTTClient.publish(topic, WiFi.localIP().toString());

    itoa(WiFi.RSSI(), payloadStr, 10);
    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, STATS_SUBTOPIC, AQUAWIN_RSSI);
    mMQTTClient.publish(topic, payloadStr);

    if (config::OPERATION_MODE == EOperationMode::LISTENER)
    {
        auto listenerStats = DHWState::getInstance().getFrameBufferStatistics(0);

        ultoa(listenerStats.mshHandled, payloadStr, 10);
        sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, STATS_SUBTOPIC, STATS_MSG_HANDLED);
        mMQTTClient.publish(topic, payloadStr);

        ultoa(listenerStats.msgUnhandled, payloadStr, 10);
        sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, STATS_SUBTOPIC, STATS_MSG_UNHANDLED);
        mMQTTClient.publish(topic, payloadStr);

        ultoa(listenerStats.msgCRCFail, payloadStr, 10);
        sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, STATS_SUBTOPIC, STATS_MSG_CRC_NOK);
        mMQTTClient.publish(topic, payloadStr);

        ultoa(listenerStats.droppedBytes, payloadStr, 10);
        sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, STATS_SUBTOPIC, STATS_DROPPED_BYTES);
        mMQTTClient.publish(topic, payloadStr);
    }
    else
    {
        auto hmiStats = DHWState::getInstance().getFrameBufferStatistics(1);

        ultoa(hmiStats.mshHandled, payloadStr, 10);
        sprintf(topic, "%s%S%S%S%S", config::mqttPrefix, BASE_TOPIC, STATS_SUBTOPIC, HMI_SUBTOPIC, STATS_MSG_HANDLED);
        mMQTTClient.publish(topic, payloadStr);

        ultoa(hmiStats.msgUnhandled, payloadStr, 10);
        sprintf(topic,
                "%s%S%S%S%S",
                config::mqttPrefix,
                BASE_TOPIC,
                STATS_SUBTOPIC,
                HMI_SUBTOPIC,
                STATS_MSG_UNHANDLED);
        mMQTTClient.publish(topic, payloadStr);

        ultoa(hmiStats.msgCRCFail, payloadStr, 10);
        sprintf(topic, "%s%S%S%S%S", config::mqttPrefix, BASE_TOPIC, STATS_SUBTOPIC, HMI_SUBTOPIC, STATS_MSG_CRC_NOK);
        mMQTTClient.publish(topic, payloadStr);

        ultoa(hmiStats.droppedBytes, payloadStr, 10);
        sprintf(topic,
                "%s%S%S%S%S",
                config::mqttPrefix,
                BASE_TOPIC,
                STATS_SUBTOPIC,
                HMI_SUBTOPIC,
                STATS_DROPPED_BYTES);
        mMQTTClient.publish(topic, payloadStr);

        ultoa(hmiStats.msgSent, payloadStr, 10);
        sprintf(topic, "%s%S%S%S%S", config::mqttPrefix, BASE_TOPIC, STATS_SUBTOPIC, HMI_SUBTOPIC, STATS_MSG_SENT);
        mMQTTClient.publish(topic, payloadStr);

        auto mainStats = DHWState::getInstance().getFrameBufferStatistics(2);

        ultoa(mainStats.mshHandled, payloadStr, 10);
        sprintf(topic, "%s%S%S%S%S", config::mqttPrefix, BASE_TOPIC, STATS_SUBTOPIC, MAIN_SUBTOPIC, STATS_MSG_HANDLED);
        mMQTTClient.publish(topic, payloadStr);

        ultoa(mainStats.msgUnhandled, payloadStr, 10);
        sprintf(topic,
                "%s%S%S%S%S",
                config::mqttPrefix,
                BASE_TOPIC,
                STATS_SUBTOPIC,
                MAIN_SUBTOPIC,
                STATS_MSG_UNHANDLED);
        mMQTTClient.publish(topic, payloadStr);

        ultoa(mainStats.msgCRCFail, payloadStr, 10);
        sprintf(topic, "%s%S%S%S%S", config::mqttPrefix, BASE_TOPIC, STATS_SUBTOPIC, MAIN_SUBTOPIC, STATS_MSG_CRC_NOK);
        mMQTTClient.publish(topic, payloadStr);

        ultoa(mainStats.droppedBytes, payloadStr, 10);
        sprintf(topic,
                "%s%S%S%S%S",
                config::mqttPrefix,
                BASE_TOPIC,
                STATS_SUBTOPIC,
                MAIN_SUBTOPIC,
                STATS_DROPPED_BYTES);
        mMQTTClient.publish(topic, payloadStr);

        ultoa(mainStats.msgSent, payloadStr, 10);
        sprintf(topic, "%s%S%S%S%S", config::mqttPrefix, BASE_TOPIC, STATS_SUBTOPIC, MAIN_SUBTOPIC, STATS_MSG_SENT);
        mMQTTClient.publish(topic, payloadStr);
    }
}

void MQTTTask::updateMainStatus()
{
    message::MainStatusMessage message(mTransferBuffer);

    char topic[50];
    char payloadStr[20];

    dtostrf(message.hotWaterTemp(), 3, 1, payloadStr);
    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, MAIN_SUBTOPIC, HOT_WATER_TEMP);
    mMQTTClient.publish(topic, payloadStr);

    dtostrf(message.airTemp(), 3, 1, payloadStr);
    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, MAIN_SUBTOPIC, SUPPLY_AIR_TEMP);
    mMQTTClient.publish(topic, payloadStr);

    dtostrf(message.evaporatorLowerAirTemp(), 3, 1, payloadStr);
    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, MAIN_SUBTOPIC, EVAPORATOR_AIR_TEMP_LOWER);
    mMQTTClient.publish(topic, payloadStr);

    dtostrf(message.evaporatorUpperAirTemp(), 3, 1, payloadStr);
    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, MAIN_SUBTOPIC, EVAPORATOR_AIR_TEMP_UPPER);
    mMQTTClient.publish(topic, payloadStr);

    ultoa(message.fanSpeed(), payloadStr, 10);
    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, MAIN_SUBTOPIC, FAN_SPEED);
    mMQTTClient.publish(topic, payloadStr);

    itoa(message.stateHeatingElement(), payloadStr, 10);
    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, MAIN_SUBTOPIC, STATE_HEAT_ELEMENT);
    mMQTTClient.publish(topic, payloadStr);

    itoa(message.stateHeatpump(), payloadStr, 10);
    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, MAIN_SUBTOPIC, STATE_HEATPUMP);
    mMQTTClient.publish(topic, payloadStr);

    itoa(message.stateBoilerBackup(), payloadStr, 10);
    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, MAIN_SUBTOPIC, STATE_EXT_BOILER);
    mMQTTClient.publish(topic, payloadStr);

    itoa(message.stateFan(), payloadStr, 10);
    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, MAIN_SUBTOPIC, STATE_FAN);
    mMQTTClient.publish(topic, payloadStr);

    itoa(message.stateDefrost(), payloadStr, 10);
    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, MAIN_SUBTOPIC, STATE_DEFROST);
    mMQTTClient.publish(topic, payloadStr);
}

void MQTTTask::updateHMIStatus()
{
    message::HMIMessage message(mTransferBuffer);

    char topic[50];
    char payloadStr[20];

    dtostrf(message.waterTempTarget(), 3, 1, payloadStr);
    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, HMI_SUBTOPIC, HOT_WATER_TEMP_TARGET);
    mMQTTClient.publish(topic, payloadStr);

    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, HMI_SUBTOPIC, OPERATION_MODE);
    switch (message.operationMode())
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
    mMQTTClient.publish(topic, payloadStr);

    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, HMI_SUBTOPIC, OPERATION_TYPE);
    if (message.isTimerModeEnabled())
    {
        sprintf(payloadStr, "%S", ENUM_OPERATION_TYPE_TIMER);
    }
    else
    {
        sprintf(payloadStr, "%S", ENUM_OPERATION_TYPE_ALWAYS_ON);
    }
    mMQTTClient.publish(topic, payloadStr);

    sprintf(payloadStr, "%02d:%02d:%02d", message.timeHours(), message.timeMinutes(), message.timeSeconds());
    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, HMI_SUBTOPIC, TIME);
    mMQTTClient.publish(topic, payloadStr);

    sprintf(payloadStr, "%d.%d.%d", message.dateDay(), message.dateMonth(), message.dateYear());
    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, HMI_SUBTOPIC, DATE);
    mMQTTClient.publish(topic, payloadStr);

    itoa(message.isEmergencyModeEnabled(), payloadStr, 10);
    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, HMI_SUBTOPIC, EMERGENCY_MODE);
    mMQTTClient.publish(topic, payloadStr);

    itoa(message.isHeatingElementEnabled(), payloadStr, 10);
    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, HMI_SUBTOPIC, HEATING_ELEMENT_ENABLED);
    mMQTTClient.publish(topic, payloadStr);

    itoa(message.antiLegionellaModePerMonth(), payloadStr, 10);
    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, HMI_SUBTOPIC, LEGIONELLA);
    mMQTTClient.publish(topic, payloadStr);

    itoa(message.airFlow(), payloadStr, 10);
    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, HMI_SUBTOPIC, AIR_DUCT_CONFIG);
    mMQTTClient.publish(topic, payloadStr);

    itoa(message.testMode(), payloadStr, 10);
    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, HMI_SUBTOPIC, TEST_MODE);
    mMQTTClient.publish(topic, payloadStr);

    // FIXME: get rid of string
    sprintf(topic, "%s%S%S%s", config::mqttPrefix, BASE_TOPIC, HMI_SUBTOPIC, "timerWindowA");
    auto msgA = message.timerWindowStr(true);
    mMQTTClient.publish(topic, msgA.c_str());

    // FIXME: get rid of string
    sprintf(topic, "%s%S%S%s", config::mqttPrefix, BASE_TOPIC, HMI_SUBTOPIC, "timerWindowB");
    auto msgB = message.timerWindowStr(false);
    mMQTTClient.publish(topic, msgB.c_str());
}

void MQTTTask::updateEnergyStats()
{
    message::MainEnergyMessage message(mTransferBuffer);

    char topic[50];
    char payloadStr[20];

    ultoa(message.totalHeatpumpHours(), payloadStr, 10);
    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, ENERGY_SUBTOPIC, TOTAL_HEATPUMP_HOURS);
    mMQTTClient.publish(topic, payloadStr);

    ultoa(message.totalHeatingElemHours(), payloadStr, 10);
    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, ENERGY_SUBTOPIC, TOTAL_HEATING_ELEM_HOURS);
    mMQTTClient.publish(topic, payloadStr);

    ultoa(message.totalHours(), payloadStr, 10);
    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, ENERGY_SUBTOPIC, TOTAL_HOURS);
    mMQTTClient.publish(topic, payloadStr);

    ultoa(message.totalEnergyCounter(), payloadStr, 10);
    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, ENERGY_SUBTOPIC, TOTAL_ENERGY_WH);
    mMQTTClient.publish(topic, payloadStr);

    ultoa(message.powerHeatpump(), payloadStr, 10);
    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, ENERGY_SUBTOPIC, POWER_HEATPUMP);
    mMQTTClient.publish(topic, payloadStr);

    ultoa(message.powerHeatElement(), payloadStr, 10);
    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, ENERGY_SUBTOPIC, POWER_HEAT_ELEMENT);
    mMQTTClient.publish(topic, payloadStr);

    ultoa(message.powerOverall(), payloadStr, 10);
    sprintf(topic, "%s%S%S%S", config::mqttPrefix, BASE_TOPIC, ENERGY_SUBTOPIC, POWER_TOTAL);
    mMQTTClient.publish(topic, payloadStr);
}

}  // namespace aquamqtt
