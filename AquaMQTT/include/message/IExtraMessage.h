#ifndef AQUAMQTT_IEXTRAMESSAGE_H
#define AQUAMQTT_IEXTRAMESSAGE_H

#include "MessageConstants.h"

namespace aquamqtt::message
{

enum class EXTRA_ATTR_FLOAT {
  EXTRA_AMPERAGE
};

enum class EXTRA_ATTR_U16
{
  EXTRA_POWER_TOTAL,
  EXTRA_VOLTAGE_GRID,
};

enum class EXTRA_ATTR_U32
{
  EXTRA_TOTAL_ENERGY
};

class IExtraMessage
{
public:
  virtual ~IExtraMessage() = default;

  virtual uint8_t getLength() = 0;

  virtual uint16_t getAttr(EXTRA_ATTR_U16 attr) = 0;
  virtual uint32_t getAttr(EXTRA_ATTR_U32 attr) = 0;
  virtual float getAttr(EXTRA_ATTR_FLOAT attr) = 0;

  virtual bool hasAttr(EXTRA_ATTR_U16 attr) const = 0;
  virtual bool hasAttr(EXTRA_ATTR_U32 attr) const = 0;
  virtual bool hasAttr(EXTRA_ATTR_FLOAT attr) const = 0;

  virtual bool hasChanged(EXTRA_ATTR_U16 attr) const = 0;
  virtual bool hasChanged(EXTRA_ATTR_U32 attr) const = 0;
  virtual bool hasChanged(EXTRA_ATTR_FLOAT attr) const = 0;
};
}  // namespace aquamqtt::message

#endif //AQUAMQTT_IEXTRAMESSAGE_H
