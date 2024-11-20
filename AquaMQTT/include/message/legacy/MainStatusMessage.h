#ifndef AQUAMQTT_LEGACYMAINSTATUSMESSAGE_H
#define AQUAMQTT_LEGACYMAINSTATUSMESSAGE_H

#include <cstring>
#include <memory>
#include <set>

#include "message/IMainMessage.h"

namespace aquamqtt::message::legacy
{
class MainStatusMessage : public IMainMessage
{
public:
    explicit MainStatusMessage(uint8_t* data, uint8_t* previous = nullptr);

    ~MainStatusMessage() override = default;

    uint8_t getLength() override;

    float    getAttr(MAIN_ATTR_FLOAT attr) override;
    bool     getAttr(MAIN_ATTR_BOOL attr) override;
    uint8_t  getAttr(MAIN_ATTR_U8 attr) override;
    uint16_t getAttr(MAIN_ATTR_U16 attr) override;

    void setAttr(MAIN_ATTR_FLOAT attr, float value) override;
    void setAttr(MAIN_ATTR_BOOL attr, bool value) override;
    void setAttr(MAIN_ATTR_U8 attr, uint8_t value) override;
    void setAttr(MAIN_ATTR_U16 attr, uint16_t value) override;

    bool hasAttr(MAIN_ATTR_FLOAT attr) const override;
    bool hasAttr(MAIN_ATTR_BOOL attr) const override;
    bool hasAttr(MAIN_ATTR_U8 attr) const override;
    bool hasAttr(MAIN_ATTR_U16 attr) const override;

    bool hasChanged(MAIN_ATTR_FLOAT attr) const override;
    bool hasChanged(MAIN_ATTR_BOOL attr) const override;
    bool hasChanged(MAIN_ATTR_U8 attr) const override;
    bool hasChanged(MAIN_ATTR_U16 attr) const override;

private:
    void compareWith(uint8_t* data);

    bool     mCreatedWithoutPrevious;
    uint8_t* mData;

    std::set<MAIN_ATTR_FLOAT> mHasChangedFloat;
    std::set<MAIN_ATTR_BOOL>  mHasChangedBool;
    std::set<MAIN_ATTR_U8>    mHasChangedU8;
    std::set<MAIN_ATTR_U16>   mHasChangedU16;
};

}  // namespace aquamqtt::message::legacy

#endif  // AQUAMQTT_LEGACYMAINSTATUSMESSAGE_H
