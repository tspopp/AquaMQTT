#ifndef AQUAMQTT_TASK_H
#define AQUAMQTT_TASK_H


namespace aquamqtt
{

class Task
{
public:
    void spawn();
    virtual void setup() = 0;
    virtual void loop() = 0;

protected:
    const char* taskName;

    Task(const char* name, unsigned long update_interval_ms = 5000);

    /**
     * Convenience method to print a message, prefixed by the task name.
     */
    void log_line(const char* msg) const;

    /**
     * This method is called after every update interval.
     */
    virtual void periodicUpdate();
private:
    unsigned long update_interval_ms;

    unsigned long last_statistics_update_timestamp = 0;

    [[noreturn]] static void innerTask(void* pvParameters);
};

}  // namespace aquamqtt

#endif  // AQUAMQTT_TASK_H
