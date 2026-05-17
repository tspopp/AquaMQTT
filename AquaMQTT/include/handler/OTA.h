#ifndef AQUAMQTT_OTA_H
#define AQUAMQTT_OTA_H

namespace aquamqtt
{
class OTAHandler
{
public:
    OTAHandler() : mStarted(false) {}

    virtual ~OTAHandler() = default;

    void setup();

    void loop();

private:
    bool mStarted;
};
}  // namespace aquamqtt

#endif  // AQUAMQTT_OTA_H
