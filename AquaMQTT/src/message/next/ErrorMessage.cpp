#include "message/next/ErrorMessage.h"

namespace aquamqtt
{
namespace message
{
namespace next
{

ErrorMessage::ErrorMessage(uint8_t* data) : mData(data)
{
}

bool ErrorMessage::isEmpty()
{
    // there are no empty messages in next protocol
    return false;
}

uint8_t ErrorMessage::getLength()
{
    return ERROR_MESSAGE_LENGTH_NEXT;
}

uint8_t ErrorMessage::getAttr(ERROR_ATTR_U8 attr)
{
    switch (attr)
    {
        case ERROR_ATTR_U8::ERROR_REQUEST_ID:
            return mData[1];
        case ERROR_ATTR_U8::ERROR_ERROR_CODE:
            return mData[2];
        case ERROR_ATTR_U8::ERROR_TIME_MINUTES:
        case ERROR_ATTR_U8::ERROR_TIME_HOURS:
        case ERROR_ATTR_U8::ERROR_DATE_DAY:
        case ERROR_ATTR_U8::ERROR_DATE_MONTH:
        case ERROR_ATTR_U8::ERROR_DATE_YEAR:
            break;
    }
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
    switch (attr)
    {
        case ERROR_ATTR_U8::ERROR_REQUEST_ID:
        case ERROR_ATTR_U8::ERROR_ERROR_CODE:
            return true;
        case ERROR_ATTR_U8::ERROR_TIME_MINUTES:
        case ERROR_ATTR_U8::ERROR_TIME_HOURS:
        case ERROR_ATTR_U8::ERROR_DATE_DAY:
        case ERROR_ATTR_U8::ERROR_DATE_MONTH:
        case ERROR_ATTR_U8::ERROR_DATE_YEAR:
            break;
    }
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

}  // namespace next
}  // namespace message
}  // namespace aquamqtt