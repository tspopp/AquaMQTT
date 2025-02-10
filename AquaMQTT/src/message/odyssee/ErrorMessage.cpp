#include "message/odyssee/ErrorMessage.h"

namespace aquamqtt::message::odyssee
{
ErrorMessage::ErrorMessage(uint8_t* data) : mData(data)
{
}

bool ErrorMessage::isEmpty()
{
    return false;
}

uint8_t ErrorMessage::getLength()
{
    return ERROR_MESSAGE_LENGTH_ODYSSEE;
}

uint8_t ErrorMessage::getAttr(ERROR_ATTR_U8 attr)
{
    return 0;
}

uint16_t ErrorMessage::getAttr(ERROR_ATTR_U16 attr)
{
    return 0;
}

float ErrorMessage::getAttr(ERROR_ATTR_FLOAT attr)
{
    return 0;
}

bool ErrorMessage::hasAttr(ERROR_ATTR_U8 attr) const
{

    return false;
}

bool ErrorMessage::hasAttr(ERROR_ATTR_U16 attr) const
{
    return false;
}

bool ErrorMessage::hasAttr(ERROR_ATTR_FLOAT attr) const
{
    return false;
}

}