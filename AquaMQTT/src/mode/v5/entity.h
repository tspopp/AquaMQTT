#ifndef AQUAMQTT_ENTITY_H
#define AQUAMQTT_ENTITY_H

#include <string>
#include <map>
#include <vector>

#include <SimpleKalmanFilter.h>


namespace aquamqtt {

#define MAX_UNIQUE_ID_SIZE 60
#define MAX_STATE_TOPIC_SIZE (MAX_UNIQUE_ID_SIZE+30)


class DhwState;


/**
 * Abstract base class for all entities.
 */
class Entity
{
private:
    DhwState* device;
    const char* entity_id;
    const char* name;
    bool is_diagnostic;

    char unique_id[MAX_UNIQUE_ID_SIZE];
    char state_topic[MAX_STATE_TOPIC_SIZE];

    bool enabled_by_default = true;

    bool _has_value = false;
    std::string _state;

public:
    Entity(DhwState* device, const char* entity_id, const char* name, bool is_diagnostic);
    virtual ~Entity() = default;

    virtual const char* getPlatform() const = 0;
    const char* state();
    virtual void set_state(const char* state) = 0;
    void unset_value();

    inline const bool getEnabledByDefault() const { return enabled_by_default; };
    inline void setEnabledByDefault(bool new_value) { enabled_by_default = new_value; };

    inline const DhwState& getDevice() { return *device; };
    inline const char* getUniqueId() const { return unique_id; };
    inline const char* getStateTopic() const { return state_topic; };
    inline virtual const char* getCommandTopic() const { return nullptr; };

    virtual void writeDefinition(std::stringstream& ss);

protected:
    std::map<std::string, std::string> _def;
    std::map<std::string, Entity*> commands;
    void update_state(const char* new_state);
};


/**
 * Binary sensor.
 * 
 * Can take three values: on, off or unknown.
 */
class BinarySensor: public Entity
{
private:
    bool value;

public:
    BinarySensor(DhwState* device_id, const char* entity_id, const char* name, bool is_diagnostic);
    virtual ~BinarySensor() = default;

    const char* getPlatform() const override { return "binary_sensor"; };

    /**
     * Set the state of this entity as a boolean value.
     */
    void set_value(const bool new_value);

    /**
     * Set the state of this entity as a string value.
     * 
     * Expects the argument to be one of: 'on', 'off', nullptr.
     */
    void set_state(const char* state) override;
};


/**
 * A switch is a binary sensor with a command topic.
 */
class Switch: public BinarySensor
{
private:
    char command_topic[MAX_STATE_TOPIC_SIZE];

public:
    Switch(DhwState* device_id, const char* entity_id, const char* name, bool is_diagnostic);
    virtual ~Switch() = default;

    const char* getPlatform() const override { return "switch"; };

    inline virtual const char* getCommandTopic() const override { return command_topic; };
};


/**
 * Sensor entity.
 */
class Sensor: public Entity
{
private:
    float value = 0;
    const char* format;
public:

    Sensor(DhwState* device_id, const char* entity_id, const char* name, bool is_diagnostic, const char* format = "%f");
    virtual ~Sensor() = default;

    const char* getPlatform() const override { return "sensor"; };

    /**
     * Set the state of this entity as a float value.
     */
    virtual void set_value(const float new_value);

    /**
     * Set the state of this entity as a string value.
     * 
     * Expects a float value or nullptr.
     */
    void set_state(const char* state) override;

    /**
     * Set the formatting rules for the state of this entity.
     * 
     * Example: '%0.1f'
     */
    void setFormat(const char* fmt);
};


/**
 * A filtered sensor uses a kalman filter to smoothen the sensor values.
 */
class FilteredSensor: public Sensor
{
private:
    SimpleKalmanFilter filter; // See: https://github.com/denyssene/SimpleKalmanFilter
public:

    FilteredSensor(DhwState* device_id, const char* entity_id, const char* name, bool is_diagnostic);
    virtual ~FilteredSensor() = default;

    virtual void set_value(const float new_value);
};


/**
 * A text entity.
 */
class TextSensor: public Entity
{
public:
    TextSensor(DhwState* device_id, const char* entity_id, const char* name, bool is_diagnostic);
    virtual ~TextSensor() = default;

    const char* getPlatform() const override { return "sensor"; };

    void set_state(const char* state) override { update_state(state); };
};


/**
 * A select entity is an interactive enum entity.
 */
class SelectEntity: public Entity
{
private:
    int value = -1; // the index of the current option, or -1 for no option
    std::vector<std::string> options; // the list of available options
    char command_topic[MAX_STATE_TOPIC_SIZE];

public:
    SelectEntity(DhwState* device_id, const char* entity_id, const char* name, bool is_diagnostic);
    virtual ~SelectEntity() = default;

    const char* getPlatform() const override { return "select"; };

    /**
     * Return the index of the current option, or -1 for no option
     */
    int getIndex() const { return value; };

    /**
     * An an item to the list of available options.
     */
    SelectEntity& addOption(const char* value);

    /**
     * Set the selected option by its index value, or -1 for no option.
     */
    void set_value(const int new_value);

    /**
     * Set the selected option by its string value, or nullptr for no option.
     */
    void set_state(const char* state) override;

    inline virtual const char* getCommandTopic() const override { return command_topic; };
    void writeDefinition(std::stringstream& s) override;
};

}

#endif // AQUAMQTT_ENTITY_H