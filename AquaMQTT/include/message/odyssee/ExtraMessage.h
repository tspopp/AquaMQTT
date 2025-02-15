#ifndef AQUAMQTT_EXTRAMESSAGE_ODYSSEE_H
#define AQUAMQTT_EXTRAMESSAGE_ODYSSEE_H

#include <set>

#include "message/IExtraMessage.h"

namespace aquamqtt::message::odyssee
{
class ExtraMessage : public IExtraMessage
{
public:
    explicit ExtraMessage(uint8_t* data, const uint8_t* previous);

    ~ExtraMessage() override = default;

    uint8_t getLength() override;

    uint16_t getAttr(EXTRA_ATTR_U16 attr) override;

    bool hasAttr(EXTRA_ATTR_U16 attr) const override;

    bool hasChanged(EXTRA_ATTR_U16 attr) const override;

private:
    void compareWith(const uint8_t* data);

    bool     mCreatedWithoutPrevious;
    uint8_t* mData;

    std::set<EXTRA_ATTR_U16> mHasChangedU16;
};

}  // namespace aquamqtt::message::odyssee

#endif  // AQUAMQTT_EXTRAMESSAGE_ODYSSEE_H
