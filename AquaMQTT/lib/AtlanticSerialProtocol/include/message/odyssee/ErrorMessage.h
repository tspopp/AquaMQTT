#ifndef AQUAMQTT_ERRORMESSAGE_ODYSSEE_H
#define AQUAMQTT_ERRORMESSAGE_ODYSSEE_H

#include <cstring>

#include "message/IErrorMessage.h"

namespace aquamqtt::message::odyssee
{
class ErrorMessage : public IErrorMessage
{
public:
    explicit ErrorMessage(uint8_t* data);

    ~ErrorMessage() override = default;

    uint8_t getLength() override;

    uint8_t  getAttr(ERROR_ATTR_U8 attr) override;
    uint16_t getAttr(ERROR_ATTR_U16 attr) override;
    float    getAttr(ERROR_ATTR_FLOAT attr) override;

    bool hasAttr(ERROR_ATTR_U8 attr) const override;
    bool hasAttr(ERROR_ATTR_U16 attr) const override;
    bool hasAttr(ERROR_ATTR_FLOAT attr) const override;

    bool isEmpty() override;

private:
    uint8_t* mData;
};

}  // namespace aquamqtt::message::odyssee

#endif  // AQUAMQTT_ERRORMESSAGE_ODYSSEE_H
