#ifndef AQUAMQTT_LEGACYHMIMESSAGE_H
#define AQUAMQTT_LEGACYHMIMESSAGE_H

#include <cstring>
#include <set>

#include "message/IHMIMessage.h"

namespace aquamqtt::message::legacy
{
class HMIMessage : public IHMIMessage
{
public:
    explicit HMIMessage(uint8_t* data, uint8_t* previous = nullptr);

    ~HMIMessage() override = default;

    uint8_t getLength() override;

    void setDateMonthAndYear(uint8_t month, uint16_t year) const override;

    uint8_t  getAttr(HMI_ATTR_U8 attr) override;
    uint16_t getAttr(HMI_ATTR_U16 attr) override;
    bool     getAttr(HMI_ATTR_BOOL attr) override;
    float    getAttr(HMI_ATTR_FLOAT attr) override;
    void     getAttr(HMI_ATTR_STR attr, char* buffer) override;

    void setAttr(HMI_ATTR_U8 attr, uint8_t value) override;
    void setAttr(HMI_ATTR_U16 attr, uint16_t value) override;
    void setAttr(HMI_ATTR_BOOL attr, bool value) override;
    void setAttr(HMI_ATTR_FLOAT attr, float value) override;

    bool hasAttr(HMI_ATTR_U8 attr) const override;
    bool hasAttr(HMI_ATTR_U16 attr) const override;
    bool hasAttr(HMI_ATTR_BOOL attr) const override;
    bool hasAttr(HMI_ATTR_FLOAT attr) const override;
    bool hasAttr(HMI_ATTR_STR attr) const override;

    bool hasChanged(HMI_ATTR_U8 attr) const override;
    bool hasChanged(HMI_ATTR_U16 attr) const override;
    bool hasChanged(HMI_ATTR_BOOL attr) const override;
    bool hasChanged(HMI_ATTR_FLOAT attr) const override;
    bool hasChanged(HMI_ATTR_STR attr) const override;

private:
    void compareWith(uint8_t* data);

    bool                     mCreatedWithoutPrevious;
    std::set<HMI_ATTR_U8>    mHasChangedU8;
    std::set<HMI_ATTR_U16>   mHasChangedU16;
    std::set<HMI_ATTR_BOOL>  mHasChangedBool;
    std::set<HMI_ATTR_FLOAT> mHasChangedFloat;
    std::set<HMI_ATTR_STR>   mHasChangedStr;

    uint8_t* mData;
};
}  // namespace aquamqtt::message::legacy

#endif  // AQUAMQTT_LEGACYHMIMESSAGE_H
