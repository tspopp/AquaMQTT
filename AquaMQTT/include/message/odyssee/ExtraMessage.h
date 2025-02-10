#ifndef AQUAMQTT_EXTRAMESSAGE_ODYSSEE_H
#define AQUAMQTT_EXTRAMESSAGE_ODYSSEE_H

#include "message/IExtraMessage.h"

namespace aquamqtt::message::odyssee
{
class ExtraMessage : public IExtraMessage
{
public:
    explicit ExtraMessage(uint8_t* data, uint8_t* previous);

    ~ExtraMessage() override = default;

    uint8_t getLength() override;

private:
    void compareWith(uint8_t* data);

    bool     mCreatedWithoutPrevious;
    uint8_t* mData;
};

}  // namespace aquamqtt::message::odyssee


#endif //AQUAMQTT_EXTRAMESSAGE_ODYSSEE_H
