#ifndef AQUAMQTT_LEGACYMAINENERGYMESSAGE_H
#define AQUAMQTT_LEGACYMAINENERGYMESSAGE_H

#include <cstring>
#include <set>

#include "message/IEnergyMessage.h"

namespace aquamqtt::message::legacy
{
class MainEnergyMessage : public IEnergyMessage
{
public:
    explicit MainEnergyMessage(uint8_t* data, uint8_t* previous = nullptr);

    ~MainEnergyMessage() override = default;

    uint8_t getLength() override;

    uint64_t getAttr(ENERGY_ATTR_U64 attr) override;
    uint32_t getAttr(ENERGY_ATTR_U32 attr) override;
    uint16_t getAttr(ENERGY_ATTR_U16 attr) override;
    int8_t   getAttr(ENERGY_ATTR_I8 attr)  override;

    bool hasAttr(ENERGY_ATTR_U64 attr) const override;
    bool hasAttr(ENERGY_ATTR_U32 attr) const override;
    bool hasAttr(ENERGY_ATTR_U16 attr) const override;
    bool hasAttr(ENERGY_ATTR_I8 attr) const override;

    bool hasChanged(ENERGY_ATTR_U64 attr) const override;
    bool hasChanged(ENERGY_ATTR_U32 attr) const override;
    bool hasChanged(ENERGY_ATTR_U16 attr) const override;
    bool hasChanged(ENERGY_ATTR_I8 attr) const override;

private:
    void compareWith(uint8_t* data);

    bool     mCreatedWithoutPrevious;
    uint8_t* mData;

    std::set<ENERGY_ATTR_U16> mHasChangedU16;
    std::set<ENERGY_ATTR_U32> mHasChangedU32;
    std::set<ENERGY_ATTR_U64> mHasChangedU64;
};

}  // namespace aquamqtt::message::legacy

#endif  // AQUAMQTT_LEGACYMAINENERGYMESSAGE_H
