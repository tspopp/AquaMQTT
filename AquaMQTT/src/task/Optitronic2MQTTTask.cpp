#include "task/Optitronic2MQTTTask.h"

#include <ArduinoJson.h>
#include <WiFi.h>
#include <esp_task_wdt.h>

#include "Version.h"
#include "config/Configuration.h"
#include "config/WebConfig.h"
#include "message/optitronic2/RegisterMap.h"
#include "state/Optitronic2State.h"

using namespace aquamqtt::message::optitronic2;

namespace aquamqtt
{

// MQTT topic definitions for Optitronic 2
namespace o2mqtt
{
constexpr char BASE[]    = "aquamqtt/";
constexpr char CTRL[]    = "aquamqtt/ctrl/";

// Sensor topics (read-only)
constexpr char WATER_TEMP[]         = "waterTemp";
constexpr char AMBIENT_TEMP[]       = "supplyAirTemp";
constexpr char EVAPORATOR_TEMP[]    = "evaporatorTemp";
constexpr char PV_ACTIVE[]          = "statePV";
constexpr char ACTIVE_SETPOINT[]    = "activeSetpoint";
constexpr char OPERATING_STATE[]    = "operatingState";
constexpr char QUICK_HEAT_ACTIVE[]  = "quickHeatActive";
constexpr char HEAT_SOURCE[]        = "heatSource";
constexpr char COMPRESSOR_STATUS[]  = "heatingTrigger";

// Settings topics (read + writable)
constexpr char DHW_SETPOINT[]       = "waterTempTarget";
constexpr char PROGRAM[]            = "operationMode";
constexpr char AUX_HEAT_MODE[]      = "auxHeatMode";
constexpr char EXT_INPUT_FN[]       = "extInputFunction";
constexpr char ECO_DEVIATION[]      = "ecoDeviation";
constexpr char KOMFORT_DEVIATION[]  = "komfortDeviation";
constexpr char FORCE_HEATING[]      = "forceHeating";

// Installer topics (read-only, available after installer menu access)
constexpr char FROST_PROTECT[]      = "frostProtectTemp";
constexpr char BIVALENT_THRESHOLD[] = "bivalentThreshold";
constexpr char PV_TARGET[]          = "pvTargetSetpoint";
constexpr char EXT_MAX_TEMP[]       = "extSourceMaxTemp";
constexpr char ANTI_LEGIO_DAYS[]    = "antiLegionellaInterval";

// Stats
constexpr char STATS_FRAMES[]       = "stats/framesReceived";
constexpr char STATS_CRC_ERR[]      = "stats/crcErrors";
constexpr char STATS_REG_UPDATES[]  = "stats/registerUpdates";
constexpr char STATS_LAST_UPDATE[]  = "stats/lastUpdateMs";

// Control subtopics (for subscribing)
constexpr char CTRL_SET_TEMP[]      = "ctrl/waterTempTarget";
constexpr char CTRL_SET_PROGRAM[]   = "ctrl/operationMode";
constexpr char CTRL_SET_EXT_INPUT[] = "ctrl/extInputFunction";
constexpr char CTRL_FORCE_HEAT[]    = "ctrl/forceHeating";
constexpr char CTRL_QUICK_HEAT[]    = "ctrl/quickHeat";
constexpr char CTRL_ANTI_LEGIO[]    = "ctrl/triggerAntiLegionella";
constexpr char CTRL_RESET[]         = "ctrl/reset";
constexpr char CTRL_ECO_DEV[]       = "ctrl/ecoDeviation";
constexpr char CTRL_KOMF_DEV[]      = "ctrl/komfortDeviation";
constexpr char CTRL_AUX_HEAT[]      = "ctrl/auxHeatMode";
constexpr char CTRL_PV_TARGET[]     = "ctrl/pvTargetSetpoint";
constexpr char CTRL_ANTI_LEGIO_INT[]= "ctrl/antiLegionellaInterval";
constexpr char CTRL_FROST_PROT[]    = "ctrl/frostProtectTemp";
constexpr char CTRL_EXT_PRIORITY[]  = "ctrl/extSourcePriority";
constexpr char CTRL_BIVALENT[]      = "ctrl/bivalentThreshold";
constexpr char CTRL_EXT_MAX_TEMP[]  = "ctrl/extSourceMaxTemp";

// Program enum strings
constexpr char ENUM_PROGRAM_NORMAL[]       = "NORMAL";
constexpr char ENUM_PROGRAM_ECO[]          = "ECO";
constexpr char ENUM_PROGRAM_KOMFORT[]      = "KOMFORT";
constexpr char ENUM_PROGRAM_KOMFORT_PLUS[] = "KOMFORT_PLUS";

// Ext input enum strings
constexpr char ENUM_EXT_QUICK_HEAT[]   = "QUICK_HEAT";
constexpr char ENUM_EXT_OFF[]          = "OFF";
constexpr char ENUM_EXT_NORMAL[]       = "NORMAL";
constexpr char ENUM_EXT_ECO[]          = "ECO";
constexpr char ENUM_EXT_KOMFORT[]      = "KOMFORT";
constexpr char ENUM_EXT_KOMFORT_PLUS[] = "KOMFORT_PLUS";
constexpr char ENUM_EXT_PV[]           = "PHOTOVOLTAIK";
constexpr char ENUM_EXT_RESERVE[]      = "RESERVE";
constexpr char ENUM_EXT_FUNCTION1[]    = "FUNCTION1";

// Operating state enum strings
constexpr char ENUM_STATE_OFF[]              = "OFF";
constexpr char ENUM_STATE_STANDBY[]          = "STANDBY";
constexpr char ENUM_STATE_IDLE[]             = "IDLE";
constexpr char ENUM_STATE_HEATING[]          = "HEATING";
constexpr char ENUM_STATE_HEATING_ELECTRIC[] = "HEATING_ELECTRIC";
constexpr char ENUM_STATE_HEATING_BOTH[]     = "HEATING_BOTH";
constexpr char ENUM_STATE_PV_BOOST[]         = "PV_BOOST";
constexpr char ENUM_STATE_DEFROST[]          = "DEFROST";
constexpr char ENUM_STATE_ANTI_LEGIONELLA[]  = "ANTI_LEGIONELLA";
constexpr char ENUM_STATE_UNKNOWN[]          = "UNKNOWN";

// Aux heat mode enum strings
constexpr char ENUM_AUX_ELECTRIC[] = "ELECTRIC";
constexpr char ENUM_AUX_EXTERNAL[] = "EXTERNAL";
constexpr char ENUM_AUX_BOTH[]     = "BOTH";

// Ext source priority enum strings
constexpr char ENUM_PRIORITY_DEVICE[]   = "DEVICE";
constexpr char ENUM_PRIORITY_EXTERNAL[] = "EXTERNAL";

// Last will
constexpr char LWT_TOPIC[]   = "aquamqtt/status";
constexpr char LWT_ONLINE[]  = "ONLINE";
constexpr char LWT_OFFLINE[] = "OFFLINE";
}  // namespace o2mqtt

// Static callback trampoline
static Optitronic2MQTTTask* sInstance = nullptr;

static void mqttMessageCallback(const String& topic, const String& payload)
{
    if (sInstance != nullptr)
    {
        sInstance->handleMessage(topic, payload);
    }
}

Optitronic2MQTTTask::Optitronic2MQTTTask()
    : mMQTTClient(512)
    , mTaskHandle(nullptr)
    , mTopicBuffer{}
    , mPayloadBuffer{}
    , mLastFullUpdate(0)
    , mLastStatsUpdate(0)
    , mPublishedDiscovery(false)
    , mWriteQueue{}
    , mWriteQueueHead(0)
    , mWriteMutex(xSemaphoreCreateMutex())
{
    sInstance = this;
}

void Optitronic2MQTTTask::spawn()
{
    xTaskCreatePinnedToCore(Optitronic2MQTTTask::innerTask, "o2mqttLoop", 8192, this, 3, &mTaskHandle, 0);
    esp_task_wdt_add(mTaskHandle);

    // Register ourselves with state for change notifications
    Optitronic2State::getInstance().setListener(mTaskHandle);
}

[[noreturn]] void Optitronic2MQTTTask::innerTask(void* pvParameters)
{
    auto* self = static_cast<Optitronic2MQTTTask*>(pvParameters);
    self->setup();

    while (true)
    {
        esp_task_wdt_reset();
        self->loop();
    }
}

void Optitronic2MQTTTask::setup()
{
    // nothing additional needed — MQTT connection handled in loop
}

void Optitronic2MQTTTask::loop()
{
    unsigned long now = millis();

    if (!mMQTTClient.connected())
    {
        connectMqtt();
        vTaskDelay(pdMS_TO_TICKS(1000));
        return;
    }

    mMQTTClient.loop();

    // Wait for notification from state changes or periodic timeout
    uint32_t notifyValue = 0;
    xTaskNotifyWaitIndexed(0, 0, 0xFFFFFFFF, &notifyValue, pdMS_TO_TICKS(5000));

    // Publish HA discovery once after first data arrives
    if (!mPublishedDiscovery && Optitronic2State::getInstance().hasBlock(REG_BLOCK1_START))
    {
        publishDiscovery();
        mPublishedDiscovery = true;
    }

    // Publish based on what changed
    if (notifyValue & Optitronic2State::CHANGE_SENSORS)
    {
        publishSensors();
    }
    if (notifyValue & (Optitronic2State::CHANGE_SETTINGS | Optitronic2State::CHANGE_STATE))
    {
        publishSettings();
        publishState();
    }
    if (notifyValue & Optitronic2State::CHANGE_INSTALLER)
    {
        publishInstaller();
    }
    if (notifyValue & Optitronic2State::CHANGE_SCHEDULE)
    {
        publishInstaller();  // schedule is published inside publishInstaller
    }

    // Full update every 60s regardless of changes
    if ((now - mLastFullUpdate) >= 60000)
    {
        publishAll();
        mLastFullUpdate = now;
    }

    // Stats every 10s
    if ((now - mLastStatsUpdate) >= 10000)
    {
        publishStats();
        mLastStatsUpdate = now;
    }
}

void Optitronic2MQTTTask::connectMqtt()
{
    mMQTTClient.begin(mqttConfig.server.c_str(), mqttConfig.port, mWiFiClient);
    mMQTTClient.onMessage([](const String& topic, const String& payload) { mqttMessageCallback(topic, payload); });
    mMQTTClient.setWill(o2mqtt::LWT_TOPIC, o2mqtt::LWT_OFFLINE, true, 1);

    if (mMQTTClient.connect(
                mqttConfig.clientId.c_str(),
                mqttConfig.user.length() == 0 ? nullptr : mqttConfig.user.c_str(),
                mqttConfig.password.length() == 0 ? nullptr : mqttConfig.password.c_str()))
    {
        Serial.println("[o2mqtt] connected to broker");

        // Subscribe to control topics
        mMQTTClient.subscribe("aquamqtt/ctrl/#");

        // Publish online status
        mMQTTClient.publish(o2mqtt::LWT_TOPIC, o2mqtt::LWT_ONLINE, true, 1);
    }
}

void Optitronic2MQTTTask::publishAll()
{
    publishSensors();
    publishSettings();
    publishState();
    publishInstaller();
}

void Optitronic2MQTTTask::publishSensors()
{
    auto& state = Optitronic2State::getInstance();

    if (state.hasBlock(REG_BLOCK2_START))
    {
        publishFloat(o2mqtt::WATER_TEMP, state.getWaterTemp());
        publishFloat(o2mqtt::AMBIENT_TEMP, state.getAmbientTemp());
        publishFloat(o2mqtt::EVAPORATOR_TEMP, state.getEvaporatorTemp());
        publishBool(o2mqtt::PV_ACTIVE, state.isPvActive());
    }
}

void Optitronic2MQTTTask::publishSettings()
{
    auto& state = Optitronic2State::getInstance();

    if (!state.hasBlock(REG_BLOCK1_START))
    {
        return;
    }

    publishFloat(o2mqtt::DHW_SETPOINT, state.getDhwSetpoint());
    publishFloat(o2mqtt::ECO_DEVIATION, state.getEcoDeviation());
    publishFloat(o2mqtt::KOMFORT_DEVIATION, state.getKomfortDeviation());

    // Program enum
    const char* programStr = o2mqtt::ENUM_STATE_UNKNOWN;
    switch (state.getProgram())
    {
        case PROGRAM_NORMAL:
            programStr = o2mqtt::ENUM_PROGRAM_NORMAL;
            break;
        case PROGRAM_ECO:
            programStr = o2mqtt::ENUM_PROGRAM_ECO;
            break;
        case PROGRAM_KOMFORT:
            programStr = o2mqtt::ENUM_PROGRAM_KOMFORT;
            break;
        case PROGRAM_KOMFORT_PLUS:
            programStr = o2mqtt::ENUM_PROGRAM_KOMFORT_PLUS;
            break;
    }
    publishString(o2mqtt::PROGRAM, programStr);

    // Aux heat mode
    const char* auxStr = o2mqtt::ENUM_STATE_UNKNOWN;
    switch (state.getAuxHeatMode())
    {
        case AUX_HEAT_ELECTRIC:
            auxStr = o2mqtt::ENUM_AUX_ELECTRIC;
            break;
        case AUX_HEAT_EXTERNAL:
            auxStr = o2mqtt::ENUM_AUX_EXTERNAL;
            break;
        case AUX_HEAT_BOTH:
            auxStr = o2mqtt::ENUM_AUX_BOTH;
            break;
    }
    publishString(o2mqtt::AUX_HEAT_MODE, auxStr);

    // Ext input function
    const char* extStr = o2mqtt::ENUM_STATE_UNKNOWN;
    switch (state.getExtInputFunction())
    {
        case EXT_INPUT_QUICK_HEAT:
            extStr = o2mqtt::ENUM_EXT_QUICK_HEAT;
            break;
        case EXT_INPUT_OFF:
            extStr = o2mqtt::ENUM_EXT_OFF;
            break;
        case EXT_INPUT_NORMAL:
            extStr = o2mqtt::ENUM_EXT_NORMAL;
            break;
        case EXT_INPUT_ECO:
            extStr = o2mqtt::ENUM_EXT_ECO;
            break;
        case EXT_INPUT_KOMFORT:
            extStr = o2mqtt::ENUM_EXT_KOMFORT;
            break;
        case EXT_INPUT_KOMFORT_PLUS:
            extStr = o2mqtt::ENUM_EXT_KOMFORT_PLUS;
            break;
        case EXT_INPUT_PHOTOVOLTAIK:
            extStr = o2mqtt::ENUM_EXT_PV;
            break;
        case EXT_INPUT_RESERVE:
            extStr = o2mqtt::ENUM_EXT_RESERVE;
            break;
        case EXT_INPUT_FUNCTION_1:
            extStr = o2mqtt::ENUM_EXT_FUNCTION1;
            break;
    }
    publishString(o2mqtt::EXT_INPUT_FN, extStr);

    publishBool(o2mqtt::FORCE_HEATING, state.isForceHeating());
}

void Optitronic2MQTTTask::publishState()
{
    auto& state = Optitronic2State::getInstance();

    if (!state.hasBlock(REG_BLOCK1_START))
    {
        return;
    }

    publishFloat(o2mqtt::ACTIVE_SETPOINT, state.getActiveSetpoint());

    uint16_t rawState = state.getOperatingState();
    const char* stateStr = o2mqtt::ENUM_STATE_UNKNOWN;
    switch (rawState)
    {
        case STATE_OFF:
            stateStr = o2mqtt::ENUM_STATE_OFF;
            break;
        case STATE_STANDBY:
            stateStr = o2mqtt::ENUM_STATE_STANDBY;
            break;
        case STATE_IDLE:
            stateStr = o2mqtt::ENUM_STATE_IDLE;
            break;
        case STATE_HEATING:
            stateStr = o2mqtt::ENUM_STATE_HEATING;
            break;
        case STATE_HEATING_ELECTRIC:
            stateStr = o2mqtt::ENUM_STATE_HEATING_ELECTRIC;
            break;
        case STATE_HEATING_BOTH:
            stateStr = o2mqtt::ENUM_STATE_HEATING_BOTH;
            break;
        case STATE_PV_BOOST:
            stateStr = o2mqtt::ENUM_STATE_PV_BOOST;
            break;
        case STATE_DEFROST:
            stateStr = o2mqtt::ENUM_STATE_DEFROST;
            break;
        case STATE_ANTI_LEGIONELLA:
            stateStr = o2mqtt::ENUM_STATE_ANTI_LEGIONELLA;
            break;
    }
    publishString(o2mqtt::OPERATING_STATE, stateStr);
    publishInt("operatingStateRaw", rawState);
    publishBool(o2mqtt::QUICK_HEAT_ACTIVE, state.isQuickHeatActive());

    // Heat source active
    const char* heatSrcStr = "OFF";
    switch (state.getHeatSource())
    {
        case HEAT_SRC_ELECTRIC:  heatSrcStr = "ELECTRIC"; break;
        case HEAT_SRC_HEATPUMP:  heatSrcStr = "HEATPUMP"; break;
        case HEAT_SRC_BOTH:      heatSrcStr = "BOTH"; break;
    }
    publishString(o2mqtt::HEAT_SOURCE, heatSrcStr);

    // Compressor status
    const char* compStr = "IDLE";
    switch (state.getCompressorStatus())
    {
        case COMP_RUNNING:    compStr = "RUNNING"; break;
        case COMP_PV_ACTIVE:  compStr = "PV_ACTIVE"; break;
    }
    publishString(o2mqtt::COMPRESSOR_STATUS, compStr);
}

void Optitronic2MQTTTask::publishInstaller()
{
    auto& state = Optitronic2State::getInstance();

    if (state.hasBlock(REG_INSTALLER_BLK1_START))
    {
        publishFloat(o2mqtt::FROST_PROTECT, state.getFrostProtectTemp());
        publishInt(o2mqtt::ANTI_LEGIO_DAYS, state.getAntiLegioInterval());
        publishString("extSourcePriority",
                      state.getExtSourcePriority() == 0 ? o2mqtt::ENUM_PRIORITY_DEVICE : o2mqtt::ENUM_PRIORITY_EXTERNAL);
    }

    if (state.hasBlock(REG_INSTALLER_BLK2_START))
    {
        publishFloat(o2mqtt::BIVALENT_THRESHOLD, state.getBivalentThreshold());
        publishFloat(o2mqtt::PV_TARGET, state.getPvTargetSetpoint());
        publishFloat(o2mqtt::EXT_MAX_TEMP, state.getExtSourceMaxTemp());
    }

    // Publish DHW schedule as JSON if available
    if (state.hasBlock(REG_SCHEDULE_DHW_START))
    {
        String json = "[";
        static const char* days[] = {"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
        for (int d = 0; d < 7; d++)
        {
            if (d > 0) json += ",";
            json += "{\"day\":\"";
            json += days[d];
            json += "\",\"slots\":[";
            for (int s = 0; s < 3; s++)
            {
                uint16_t sv, ev;
                uint16_t sr = REG_SCHEDULE_DHW_START + d * 6 + s * 2;
                uint16_t er = sr + 1;
                state.getRegister(sr, sv);
                state.getRegister(er, ev);
                if (s > 0) json += ",";
                if (sv == SCHED_DISABLED)
                {
                    json += "null";
                }
                else
                {
                    uint8_t sh = ((sv & SCHED_TIME_MASK) * 15) / 60;
                    uint8_t sm = ((sv & SCHED_TIME_MASK) * 15) % 60;
                    uint8_t eh = ((ev & SCHED_TIME_MASK) * 15) / 60;
                    uint8_t em = ((ev & SCHED_TIME_MASK) * 15) % 60;
                    char buf[32];
                    snprintf(buf, sizeof(buf), "\"%02d:%02d-%02d:%02d\"", sh, sm, eh, em);
                    json += buf;
                }
            }
            json += "]}";
        }
        json += "]";
        publishString("schedule/dhw", json.c_str());
    }
}

void Optitronic2MQTTTask::publishStats()
{
    auto stats = Optitronic2State::getInstance().getStats();
    publishInt(o2mqtt::STATS_FRAMES, stats.framesReceived);
    publishInt(o2mqtt::STATS_CRC_ERR, stats.crcErrors);
    publishInt(o2mqtt::STATS_REG_UPDATES, stats.registerUpdates);
    publishInt(o2mqtt::STATS_LAST_UPDATE, stats.lastUpdateMs);
}

void Optitronic2MQTTTask::publishDiscovery()
{
    if (!mqttConfig.enableDiscovery)
    {
        return;
    }

    // Helper lambda to publish a HA discovery config
    auto publishSensorDiscovery = [&](const char* name, const char* stateTopic, const char* unit,
                                      const char* deviceClass, const char* uniqueIdSuffix) {
        JsonDocument doc;
        doc["name"]                = name;
        doc["stat_t"]              = String(o2mqtt::BASE) + stateTopic;
        doc["uniq_id"]             = String("aquamqtt_o2_") + uniqueIdSuffix;
        doc["val_tpl"]             = "{{ value }}";
        if (unit != nullptr)
            doc["unit_of_meas"] = unit;
        if (deviceClass != nullptr)
            doc["dev_cla"] = deviceClass;

        auto device        = doc["dev"].to<JsonObject>();
        device["ids"]      = "aquamqtt_optitronic2";
        device["name"]     = aquaMqttConfig.heatpumpModelName.c_str();
        device["mf"]       = "Austria Email";
        device["mdl"]      = "WPA 450 ECO (Optitronic 2)";
        device["sw"]       = String("AquaMQTT ") + aquamqtt::VERSION;

        char discoveryTopic[128];
        snprintf(discoveryTopic, sizeof(discoveryTopic), "%ssensor/aquamqtt_o2/%s/config",
                 mqttConfig.discoveryPrefix.c_str(), uniqueIdSuffix);

        char payload[512];
        serializeJson(doc, payload, sizeof(payload));
        mMQTTClient.publish(discoveryTopic, payload, true, 0);
    };

    auto publishNumberDiscovery = [&](const char* name, const char* stateTopic, const char* cmdTopic,
                                      float min, float max, float step, const char* unit, const char* uniqueIdSuffix) {
        JsonDocument doc;
        doc["name"]       = name;
        doc["stat_t"]     = String(o2mqtt::BASE) + stateTopic;
        doc["cmd_t"]      = String(o2mqtt::BASE) + cmdTopic;
        doc["uniq_id"]    = String("aquamqtt_o2_") + uniqueIdSuffix;
        doc["min"]        = min;
        doc["max"]        = max;
        doc["step"]       = step;
        if (unit != nullptr)
            doc["unit_of_meas"] = unit;

        auto device        = doc["dev"].to<JsonObject>();
        device["ids"]      = "aquamqtt_optitronic2";
        device["name"]     = aquaMqttConfig.heatpumpModelName.c_str();
        device["mf"]       = "Austria Email";
        device["mdl"]      = "WPA 450 ECO (Optitronic 2)";
        device["sw"]       = String("AquaMQTT ") + aquamqtt::VERSION;

        char discoveryTopic[128];
        snprintf(discoveryTopic, sizeof(discoveryTopic), "%snumber/aquamqtt_o2/%s/config",
                 mqttConfig.discoveryPrefix.c_str(), uniqueIdSuffix);

        char payload[512];
        serializeJson(doc, payload, sizeof(payload));
        mMQTTClient.publish(discoveryTopic, payload, true, 0);
    };

    auto publishSelectDiscovery = [&](const char* name, const char* stateTopic, const char* cmdTopic,
                                      const char* const* options, uint8_t optionCount, const char* uniqueIdSuffix) {
        JsonDocument doc;
        doc["name"]       = name;
        doc["stat_t"]     = String(o2mqtt::BASE) + stateTopic;
        doc["cmd_t"]      = String(o2mqtt::BASE) + cmdTopic;
        doc["uniq_id"]    = String("aquamqtt_o2_") + uniqueIdSuffix;

        auto opts = doc["options"].to<JsonArray>();
        for (uint8_t i = 0; i < optionCount; i++)
        {
            opts.add(options[i]);
        }

        auto device        = doc["dev"].to<JsonObject>();
        device["ids"]      = "aquamqtt_optitronic2";
        device["name"]     = aquaMqttConfig.heatpumpModelName.c_str();
        device["mf"]       = "Austria Email";
        device["mdl"]      = "WPA 450 ECO (Optitronic 2)";
        device["sw"]       = String("AquaMQTT ") + aquamqtt::VERSION;

        char discoveryTopic[128];
        snprintf(discoveryTopic, sizeof(discoveryTopic), "%sselect/aquamqtt_o2/%s/config",
                 mqttConfig.discoveryPrefix.c_str(), uniqueIdSuffix);

        char payload[512];
        serializeJson(doc, payload, sizeof(payload));
        mMQTTClient.publish(discoveryTopic, payload, true, 0);
    };

    auto publishSwitchDiscovery = [&](const char* name, const char* stateTopic, const char* cmdTopic,
                                      const char* uniqueIdSuffix) {
        JsonDocument doc;
        doc["name"]       = name;
        doc["stat_t"]     = String(o2mqtt::BASE) + stateTopic;
        doc["cmd_t"]      = String(o2mqtt::BASE) + cmdTopic;
        doc["uniq_id"]    = String("aquamqtt_o2_") + uniqueIdSuffix;
        doc["pl_on"]      = "1";
        doc["pl_off"]     = "0";

        auto device        = doc["dev"].to<JsonObject>();
        device["ids"]      = "aquamqtt_optitronic2";
        device["name"]     = aquaMqttConfig.heatpumpModelName.c_str();
        device["mf"]       = "Austria Email";
        device["mdl"]      = "WPA 450 ECO (Optitronic 2)";
        device["sw"]       = String("AquaMQTT ") + aquamqtt::VERSION;

        char discoveryTopic[128];
        snprintf(discoveryTopic, sizeof(discoveryTopic), "%sswitch/aquamqtt_o2/%s/config",
                 mqttConfig.discoveryPrefix.c_str(), uniqueIdSuffix);

        char payload[512];
        serializeJson(doc, payload, sizeof(payload));
        mMQTTClient.publish(discoveryTopic, payload, true, 0);
    };

    // --- Sensors ---
    publishSensorDiscovery("Water Temperature", o2mqtt::WATER_TEMP, "°C", "temperature", "water_temp");
    publishSensorDiscovery("Ambient Temperature", o2mqtt::AMBIENT_TEMP, "°C", "temperature", "ambient_temp");
    publishSensorDiscovery("Evaporator Temperature", o2mqtt::EVAPORATOR_TEMP, "°C", "temperature", "evap_temp");
    publishSensorDiscovery("Active Setpoint", o2mqtt::ACTIVE_SETPOINT, "°C", "temperature", "active_setpoint");
    publishSensorDiscovery("Operating State", o2mqtt::OPERATING_STATE, nullptr, nullptr, "op_state");
    publishSensorDiscovery("Heat Source", o2mqtt::HEAT_SOURCE, nullptr, nullptr, "heat_source");
    publishSensorDiscovery("Heating Trigger", o2mqtt::COMPRESSOR_STATUS, nullptr, nullptr, "heat_trigger");
    publishSensorDiscovery("PV Active", o2mqtt::PV_ACTIVE, nullptr, nullptr, "pv_active");
    publishSensorDiscovery("Quick Heat Active", o2mqtt::QUICK_HEAT_ACTIVE, nullptr, nullptr, "quick_heat");
    publishSensorDiscovery("Eco Deviation", o2mqtt::ECO_DEVIATION, "°C", "temperature", "eco_dev");
    publishSensorDiscovery("Komfort Deviation", o2mqtt::KOMFORT_DEVIATION, "°C", "temperature", "komf_dev");
    publishSensorDiscovery("Aux Heat Mode", o2mqtt::AUX_HEAT_MODE, nullptr, nullptr, "aux_heat_mode");
    publishSensorDiscovery("Frost Protection Temp", o2mqtt::FROST_PROTECT, "°C", "temperature", "frost_protect");
    publishSensorDiscovery("Anti-Legionella Interval", o2mqtt::ANTI_LEGIO_DAYS, "d", nullptr, "anti_legio_interval");
    publishSensorDiscovery("Bivalent Threshold", o2mqtt::BIVALENT_THRESHOLD, "°C", "temperature", "bivalent_threshold");
    publishSensorDiscovery("PV Target Setpoint", o2mqtt::PV_TARGET, "°C", "temperature", "pv_target");
    publishSensorDiscovery("Ext Source Max Temp", o2mqtt::EXT_MAX_TEMP, "°C", "temperature", "ext_max_temp");

    // --- Controllable entities ---
    publishNumberDiscovery("DHW Target Temperature", o2mqtt::DHW_SETPOINT, o2mqtt::CTRL_SET_TEMP,
                           35.0, 70.0, 0.5, "°C", "dhw_setpoint");

    publishNumberDiscovery("Eco Deviation", o2mqtt::ECO_DEVIATION, o2mqtt::CTRL_ECO_DEV,
                           -15.0, 0.0, 0.5, "°C", "ctrl_eco_dev");

    publishNumberDiscovery("Komfort Deviation", o2mqtt::KOMFORT_DEVIATION, o2mqtt::CTRL_KOMF_DEV,
                           0.0, 10.0, 0.5, "°C", "ctrl_komf_dev");

    publishNumberDiscovery("Frost Protection Temp", o2mqtt::FROST_PROTECT, o2mqtt::CTRL_FROST_PROT,
                           2.0, 15.0, 0.5, "°C", "ctrl_frost_protect");

    publishNumberDiscovery("Anti-Legionella Interval", o2mqtt::ANTI_LEGIO_DAYS, o2mqtt::CTRL_ANTI_LEGIO_INT,
                           0.0, 30.0, 1.0, "d", "ctrl_anti_legio");

    publishNumberDiscovery("PV Target Setpoint", o2mqtt::PV_TARGET, o2mqtt::CTRL_PV_TARGET,
                           35.0, 70.0, 0.5, "°C", "ctrl_pv_target");

    publishNumberDiscovery("Bivalent Threshold", o2mqtt::BIVALENT_THRESHOLD, o2mqtt::CTRL_BIVALENT,
                           -10.0, 20.0, 0.5, "°C", "ctrl_bivalent");

    publishNumberDiscovery("Ext Source Max Temp", o2mqtt::EXT_MAX_TEMP, o2mqtt::CTRL_EXT_MAX_TEMP,
                           35.0, 70.0, 0.5, "°C", "ctrl_ext_max");

    const char* programOptions[] = { o2mqtt::ENUM_PROGRAM_NORMAL, o2mqtt::ENUM_PROGRAM_ECO,
                                     o2mqtt::ENUM_PROGRAM_KOMFORT, o2mqtt::ENUM_PROGRAM_KOMFORT_PLUS };
    publishSelectDiscovery("Operating Program", o2mqtt::PROGRAM, o2mqtt::CTRL_SET_PROGRAM,
                           programOptions, 4, "program");

    const char* extInputOptions[] = { o2mqtt::ENUM_EXT_QUICK_HEAT, o2mqtt::ENUM_EXT_OFF,
                                      o2mqtt::ENUM_EXT_NORMAL, o2mqtt::ENUM_EXT_ECO,
                                      o2mqtt::ENUM_EXT_KOMFORT, o2mqtt::ENUM_EXT_KOMFORT_PLUS,
                                      o2mqtt::ENUM_EXT_PV, o2mqtt::ENUM_EXT_RESERVE,
                                      o2mqtt::ENUM_EXT_FUNCTION1 };
    publishSelectDiscovery("External Input Function", o2mqtt::EXT_INPUT_FN, o2mqtt::CTRL_SET_EXT_INPUT,
                           extInputOptions, 9, "ext_input");

    const char* auxHeatOptions[] = { o2mqtt::ENUM_AUX_ELECTRIC, o2mqtt::ENUM_AUX_EXTERNAL, o2mqtt::ENUM_AUX_BOTH };
    publishSelectDiscovery("Aux Heat Mode", o2mqtt::AUX_HEAT_MODE, o2mqtt::CTRL_AUX_HEAT,
                           auxHeatOptions, 3, "aux_heat");

    const char* priorityOptions[] = { o2mqtt::ENUM_PRIORITY_DEVICE, o2mqtt::ENUM_PRIORITY_EXTERNAL };
    publishSelectDiscovery("Ext Source Priority", "extSourcePriority", o2mqtt::CTRL_EXT_PRIORITY,
                           priorityOptions, 2, "ext_priority");

    publishSwitchDiscovery("Force Heating", o2mqtt::FORCE_HEATING, o2mqtt::CTRL_FORCE_HEAT, "force_heating");

    Serial.println("[o2mqtt] HA discovery published");
}

// --- Command handling ---

void Optitronic2MQTTTask::handleMessage(const String& topic, const String& payload)
{
    if (topic.endsWith("waterTempTarget"))
    {
        float temp = payload.toFloat();
        if (temp >= 35.0f && temp <= 70.0f)
        {
            handleSetDhwSetpoint(temp);
        }
    }
    else if (topic.endsWith("operationMode"))
    {
        if (payload == o2mqtt::ENUM_PROGRAM_NORMAL)
            handleSetProgram(PROGRAM_NORMAL);
        else if (payload == o2mqtt::ENUM_PROGRAM_ECO)
            handleSetProgram(PROGRAM_ECO);
        else if (payload == o2mqtt::ENUM_PROGRAM_KOMFORT)
            handleSetProgram(PROGRAM_KOMFORT);
        else if (payload == o2mqtt::ENUM_PROGRAM_KOMFORT_PLUS)
            handleSetProgram(PROGRAM_KOMFORT_PLUS);
    }
    else if (topic.endsWith("extInputFunction"))
    {
        if (payload == o2mqtt::ENUM_EXT_QUICK_HEAT)
            handleSetExtInputFunction(EXT_INPUT_QUICK_HEAT);
        else if (payload == o2mqtt::ENUM_EXT_OFF)
            handleSetExtInputFunction(EXT_INPUT_OFF);
        else if (payload == o2mqtt::ENUM_EXT_NORMAL)
            handleSetExtInputFunction(EXT_INPUT_NORMAL);
        else if (payload == o2mqtt::ENUM_EXT_ECO)
            handleSetExtInputFunction(EXT_INPUT_ECO);
        else if (payload == o2mqtt::ENUM_EXT_KOMFORT)
            handleSetExtInputFunction(EXT_INPUT_KOMFORT);
        else if (payload == o2mqtt::ENUM_EXT_KOMFORT_PLUS)
            handleSetExtInputFunction(EXT_INPUT_KOMFORT_PLUS);
        else if (payload == o2mqtt::ENUM_EXT_PV)
            handleSetExtInputFunction(EXT_INPUT_PHOTOVOLTAIK);
        else if (payload == o2mqtt::ENUM_EXT_RESERVE)
            handleSetExtInputFunction(EXT_INPUT_RESERVE);
        else if (payload == o2mqtt::ENUM_EXT_FUNCTION1)
            handleSetExtInputFunction(EXT_INPUT_FUNCTION_1);
    }
    else if (topic.endsWith("forceHeating"))
    {
        handleSetForceHeating(payload == "1");
    }
    else if (topic.endsWith("quickHeat"))
    {
        // Payload: "1" to activate (uses current target), "0" to deactivate
        // Or JSON: {"active": true, "target": 43.5}
        if (payload == "1")
        {
            handleSetQuickHeat(true, 0);  // use existing target
        }
        else if (payload == "0")
        {
            handleSetQuickHeat(false, 0);
        }
    }
    else if (topic.endsWith("triggerAntiLegionella"))
    {
        if (payload == "1")
        {
            handleTriggerAntiLegionella();
        }
    }
    else if (topic.endsWith("reset"))
    {
        if (payload == "1")
        {
            ESP.restart();
        }
    }
    else if (topic.endsWith("ecoDeviation"))
    {
        float dev = payload.toFloat();
        if (dev >= -15.0f && dev <= 0.0f)
        {
            queueWrite(REG_ECO_DEVIATION, tempToReg(dev));
        }
    }
    else if (topic.endsWith("komfortDeviation"))
    {
        float dev = payload.toFloat();
        if (dev >= 0.0f && dev <= 10.0f)
        {
            queueWrite(REG_KOMFORT_DEVIATION, tempToReg(dev));
        }
    }
    else if (topic.endsWith("auxHeatMode"))
    {
        if (payload == o2mqtt::ENUM_AUX_ELECTRIC)
            queueWrite(REG_AUX_HEAT_MODE, AUX_HEAT_ELECTRIC);
        else if (payload == o2mqtt::ENUM_AUX_EXTERNAL)
            queueWrite(REG_AUX_HEAT_MODE, AUX_HEAT_EXTERNAL);
        else if (payload == o2mqtt::ENUM_AUX_BOTH)
            queueWrite(REG_AUX_HEAT_MODE, AUX_HEAT_BOTH);
    }
    else if (topic.endsWith("pvTargetSetpoint"))
    {
        float temp = payload.toFloat();
        if (temp >= 35.0f && temp <= 70.0f)
        {
            queueWrite(REG_PV_TARGET_SETPOINT, tempToReg(temp));
        }
    }
    else if (topic.endsWith("antiLegionellaInterval"))
    {
        int days = payload.toInt();
        if (days >= 0 && days <= 30)
        {
            queueWrite(REG_ANTI_LEGIO_INTERVAL, (uint16_t)days);
        }
    }
    else if (topic.endsWith("frostProtectTemp"))
    {
        float temp = payload.toFloat();
        if (temp >= 2.0f && temp <= 15.0f)
        {
            queueWrite(REG_FROST_PROTECT_TEMP, tempToReg(temp));
        }
    }
    else if (topic.endsWith("extSourcePriority"))
    {
        if (payload == o2mqtt::ENUM_PRIORITY_DEVICE)
            queueWrite(REG_EXT_SOURCE_PRIORITY, 0);
        else if (payload == o2mqtt::ENUM_PRIORITY_EXTERNAL)
            queueWrite(REG_EXT_SOURCE_PRIORITY, 1);
    }
    else if (topic.endsWith("bivalentThreshold"))
    {
        float temp = payload.toFloat();
        if (temp >= -10.0f && temp <= 20.0f)
        {
            queueWrite(REG_BIVALENT_THRESHOLD, tempToReg(temp));
        }
    }
    else if (topic.endsWith("extSourceMaxTemp"))
    {
        float temp = payload.toFloat();
        if (temp >= 35.0f && temp <= 70.0f)
        {
            queueWrite(REG_EXT_SOURCE_MAX_TEMP, tempToReg(temp));
        }
    }
}

void Optitronic2MQTTTask::handleSetDhwSetpoint(float temp)
{
    queueWrite(REG_DHW_SETPOINT, tempToReg(temp));
}

void Optitronic2MQTTTask::handleSetProgram(uint16_t program)
{
    queueWrite(REG_PROGRAM, program);
}

void Optitronic2MQTTTask::handleSetExtInputFunction(uint16_t fn)
{
    queueWrite(REG_EXT_INPUT_FUNCTION, fn);
}

void Optitronic2MQTTTask::handleSetForceHeating(bool enable)
{
    queueWrite(REG_FORCE_HEATING, enable ? 1 : 0);
}

void Optitronic2MQTTTask::handleSetQuickHeat(bool activate, float target)
{
    uint16_t currentReg = 0;
    Optitronic2State::getInstance().getRegister(REG_QUICK_HEAT_STATE, currentReg);
    uint16_t currentTarget = getQuickHeatTarget(currentReg);

    uint16_t value;
    if (activate)
    {
        uint16_t targetReg = (target > 0) ? tempToReg(target) : currentTarget;
        value = makeQuickHeatActivate(targetReg);
    }
    else
    {
        value = currentTarget;
    }

    queueWrite(REG_QUICK_HEAT_STATE, value);
}

void Optitronic2MQTTTask::handleTriggerAntiLegionella()
{
    queueWrite(REG_ANTI_LEGIONELLA, 1);
}

void Optitronic2MQTTTask::queueWrite(uint16_t reg, uint16_t value)
{
    if (xSemaphoreTake(mWriteMutex, pdMS_TO_TICKS(100)))
    {
        for (auto& cmd : mWriteQueue)
        {
            if (!cmd.pending)
            {
                cmd.reg     = reg;
                cmd.value   = value;
                cmd.pending = true;
                break;
            }
        }
        xSemaphoreGive(mWriteMutex);
    }
}

// --- Write queue interface for ModbusListenerTask ---

bool Optitronic2MQTTTask::hasPendingWrite() const
{
    for (const auto& cmd : mWriteQueue)
    {
        if (cmd.pending)
        {
            return true;
        }
    }
    return false;
}

bool Optitronic2MQTTTask::dequeuePendingWrite(uint16_t& reg, uint16_t& value)
{
    if (!xSemaphoreTake(mWriteMutex, pdMS_TO_TICKS(10)))
    {
        return false;
    }

    bool found = false;
    for (auto& cmd : mWriteQueue)
    {
        if (cmd.pending)
        {
            reg         = cmd.reg;
            value       = cmd.value;
            cmd.pending = false;
            found       = true;
            break;
        }
    }

    xSemaphoreGive(mWriteMutex);
    return found;
}

// --- Publish helpers ---

void Optitronic2MQTTTask::publishFloat(const char* subtopic, float value, uint8_t decimals)
{
    snprintf(mTopicBuffer, sizeof(mTopicBuffer), "%s%s", o2mqtt::BASE, subtopic);
    dtostrf(value, 1, decimals, mPayloadBuffer);
    mMQTTClient.publish(mTopicBuffer, mPayloadBuffer, true, 0);
}

void Optitronic2MQTTTask::publishInt(const char* subtopic, int value)
{
    snprintf(mTopicBuffer, sizeof(mTopicBuffer), "%s%s", o2mqtt::BASE, subtopic);
    snprintf(mPayloadBuffer, sizeof(mPayloadBuffer), "%d", value);
    mMQTTClient.publish(mTopicBuffer, mPayloadBuffer, true, 0);
}

void Optitronic2MQTTTask::publishString(const char* subtopic, const char* value)
{
    snprintf(mTopicBuffer, sizeof(mTopicBuffer), "%s%s", o2mqtt::BASE, subtopic);
    mMQTTClient.publish(mTopicBuffer, value, true, 0);
}

void Optitronic2MQTTTask::publishBool(const char* subtopic, bool value)
{
    snprintf(mTopicBuffer, sizeof(mTopicBuffer), "%s%s", o2mqtt::BASE, subtopic);
    mMQTTClient.publish(mTopicBuffer, value ? "1" : "0", true, 0);
}

}  // namespace aquamqtt
