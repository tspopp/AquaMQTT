#ifndef AQUAMQTT_IEXTRAMESSAGE_H
#define AQUAMQTT_IEXTRAMESSAGE_H

#include "MessageConstants.h"

namespace aquamqtt::message
{

class IExtraMessage
{
public:
  virtual ~IExtraMessage() = default;

  virtual uint8_t getLength() = 0;
};
}  // namespace aquamqtt::message

#endif //AQUAMQTT_IEXTRAMESSAGE_H
