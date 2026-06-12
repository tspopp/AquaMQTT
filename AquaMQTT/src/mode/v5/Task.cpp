#include "Task.h"

#include <Arduino.h>
#include <esp_task_wdt.h>

#include "constants.h"


namespace aquamqtt
{

Task::Task(const char* name, unsigned long _update_interval_ms)
    : taskName(name)
    , update_interval_ms(_update_interval_ms)
{}

void Task::spawn()
{
    TaskHandle_t handle;
    xTaskCreatePinnedToCore(Task::innerTask, taskName, 8192, this, 5, &handle, 1);
    //esp_task_wdt_add(handle);
}

[[noreturn]] void Task::innerTask(void* pvParameters)
{
    auto* self = static_cast<Task*>(pvParameters);

    self->setup();

    while (true)
    {
        //esp_task_wdt_reset();
        self->loop();
    }
}

void Task::setup()
{
    log_line("setup");
}

void Task::loop()
{
    if ((millis() - last_statistics_update_timestamp) >= update_interval_ms)
    {
        periodicUpdate();
        last_statistics_update_timestamp = millis();
    }
}

void Task::log_line(const char* msg) const
{
    LOG.print("[");
    LOG.print(taskName);
    LOG.print("] ");
    LOG.println(msg);
}

void Task::periodicUpdate()
{
    LOG.print("[");
    LOG.print(taskName);
    LOG.print("] stack size: ");
    LOG.print(uxTaskGetStackHighWaterMark(nullptr));
    LOG.print("; minimum ever heep size: ");
    LOG.print(xPortGetMinimumEverFreeHeapSize());
    LOG.print("; uptime: ");

    unsigned long uptime_secs = millis() / 1000;
    unsigned long uptime_mins = uptime_secs / 60;
    unsigned long uptime_hours = uptime_mins / 60;
    unsigned long uptime_days = uptime_hours / 24;
    if (uptime_days > 0) {
        LOG.print(uptime_days);
        LOG.print("d");
    }
    if (uptime_days > 0 || uptime_hours > 0) {
        LOG.print(uptime_hours % 24);
        LOG.print("h");
    }
    if (uptime_days > 0 || uptime_hours > 0 || uptime_mins > 0) {
        LOG.print(uptime_mins % 60);
        LOG.print("m");
    }
    LOG.print(uptime_secs % 60);
    LOG.print("s");
    LOG.println();
}


}  // namespace aquamqtt
