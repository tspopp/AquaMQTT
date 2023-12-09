#ifndef AQUAMQTT_OTA_H
#define AQUAMQTT_OTA_H

namespace aquamqtt
{
class OTAHandler
{
public:
    OTAHandler() = default;

    virtual ~OTAHandler() = default;

    void setup();

    void loop();
};
}  // namespace aquamqtt

#endif  // AQUAMQTT_OTA_H
