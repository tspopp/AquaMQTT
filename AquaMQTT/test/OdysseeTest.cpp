#include <gtest/gtest.h>

#include <ostream>

#include "message/IHMIMessage.h"
#include "message/odyssee/HMIMessage.h"

using namespace aquamqtt::message::odyssee;
using namespace aquamqtt::message;

uint8_t msg194[] = {
  0x1d,  0x36,  0x02,
  0x00,  0x21,  0x00,  0x00,
  0x00,  0x00,  0x00,  0x00,
  0x00,  0x00,  0x00,  0x00,
  0x00,  0x00,  0x21,  0x32,
  0x26,  0x11,  0x00,  0x00,
  0x00,  0x00,  0x41,  0x42,
  0x00,  0x00
};

TEST(OdysseeProtocolTest, SetAndGetWaterTemperature)
{
    HMIMessage message (msg194, nullptr);

    auto waterTempTarget = message.getAttr(HMI_ATTR_FLOAT::WATER_TARGET_TEMPERATURE);
    EXPECT_FLOAT_EQ(54.0, waterTempTarget);

    message.setAttr(       HMI_ATTR_FLOAT::WATER_TARGET_TEMPERATURE, 62.0);
    waterTempTarget = message.getAttr(HMI_ATTR_FLOAT::WATER_TARGET_TEMPERATURE);
    EXPECT_FLOAT_EQ(62.0, waterTempTarget);
}

TEST(OdysseeProtocolTest, SetOperationTypeAndMode)
{
  HMIMessage message (msg194, nullptr);


  // default values set by initial buffer
  HMIOperationMode operationMode = static_cast<HMIOperationMode>(message.getAttr(HMI_ATTR_U8::OPERATION_MODE));
  HMIOperationType operationType = static_cast<HMIOperationType>(message.getAttr(HMI_ATTR_U8::OPERATION_TYPE));
  ASSERT_EQ(operationMode, HMIOperationMode::OM_ECO_INACTIVE);
  ASSERT_EQ(operationType, HMIOperationType::OT_OFF_PEAK_HOURS);


  // set all operation modes in off peak hours type
  message.setAttr(HMI_ATTR_U8::OPERATION_MODE, HMIOperationMode::OM_ABSENCE);
  operationMode = static_cast<HMIOperationMode>(message.getAttr(HMI_ATTR_U8::OPERATION_MODE));
  operationType = static_cast<HMIOperationType>(message.getAttr(HMI_ATTR_U8::OPERATION_TYPE));
  ASSERT_EQ(operationMode, HMIOperationMode::OM_ABSENCE);
  ASSERT_EQ(operationType, HMIOperationType::OT_OFF_PEAK_HOURS);

  message.setAttr(HMI_ATTR_U8::OPERATION_MODE, HMIOperationMode::OM_ECO_ACTIVE);
  operationMode = static_cast<HMIOperationMode>(message.getAttr(HMI_ATTR_U8::OPERATION_MODE));
  operationType = static_cast<HMIOperationType>(message.getAttr(HMI_ATTR_U8::OPERATION_TYPE));
  ASSERT_EQ(operationMode, HMIOperationMode::OM_ECO_ACTIVE);
  ASSERT_EQ(operationType, HMIOperationType::OT_OFF_PEAK_HOURS);

  message.setAttr(HMI_ATTR_U8::OPERATION_MODE, HMIOperationMode::OM_ECO_INACTIVE);
  operationMode = static_cast<HMIOperationMode>(message.getAttr(HMI_ATTR_U8::OPERATION_MODE));
  operationType = static_cast<HMIOperationType>(message.getAttr(HMI_ATTR_U8::OPERATION_TYPE));
  ASSERT_EQ(operationMode, HMIOperationMode::OM_ECO_INACTIVE);
  ASSERT_EQ(operationType, HMIOperationType::OT_OFF_PEAK_HOURS);

  message.setAttr(HMI_ATTR_U8::OPERATION_MODE, HMIOperationMode::OM_BOOST);
  operationMode = static_cast<HMIOperationMode>(message.getAttr(HMI_ATTR_U8::OPERATION_MODE));
  operationType = static_cast<HMIOperationType>(message.getAttr(HMI_ATTR_U8::OPERATION_TYPE));
  ASSERT_EQ(operationMode, HMIOperationMode::OM_BOOST);
  ASSERT_EQ(operationType, HMIOperationType::OT_OFF_PEAK_HOURS);

  message.setAttr(HMI_ATTR_U8::OPERATION_MODE, HMIOperationMode::OM_AUTO);
  operationMode = static_cast<HMIOperationMode>(message.getAttr(HMI_ATTR_U8::OPERATION_MODE));
  operationType = static_cast<HMIOperationType>(message.getAttr(HMI_ATTR_U8::OPERATION_TYPE));
  ASSERT_EQ(operationMode, HMIOperationMode::OM_AUTO);
  ASSERT_EQ(operationType, HMIOperationType::OT_OFF_PEAK_HOURS);

  // set all five modes in always on
  message.setAttr(HMI_ATTR_U8::OPERATION_TYPE, HMIOperationType::OT_ALWAYS_ON);
  message.setAttr(HMI_ATTR_U8::OPERATION_MODE, HMIOperationMode::OM_ABSENCE);
  operationMode = static_cast<HMIOperationMode>(message.getAttr(HMI_ATTR_U8::OPERATION_MODE));
  operationType = static_cast<HMIOperationType>(message.getAttr(HMI_ATTR_U8::OPERATION_TYPE));
  ASSERT_EQ(operationMode, HMIOperationMode::OM_ABSENCE);
  ASSERT_EQ(operationType, HMIOperationType::OT_ALWAYS_ON);

  message.setAttr(HMI_ATTR_U8::OPERATION_MODE, HMIOperationMode::OM_ECO_ACTIVE);
  operationMode = static_cast<HMIOperationMode>(message.getAttr(HMI_ATTR_U8::OPERATION_MODE));
  operationType = static_cast<HMIOperationType>(message.getAttr(HMI_ATTR_U8::OPERATION_TYPE));
  ASSERT_EQ(operationMode, HMIOperationMode::OM_ECO_ACTIVE);
  ASSERT_EQ(operationType, HMIOperationType::OT_ALWAYS_ON);

  message.setAttr(HMI_ATTR_U8::OPERATION_MODE, HMIOperationMode::OM_ECO_INACTIVE);
  operationMode = static_cast<HMIOperationMode>(message.getAttr(HMI_ATTR_U8::OPERATION_MODE));
  operationType = static_cast<HMIOperationType>(message.getAttr(HMI_ATTR_U8::OPERATION_TYPE));
  ASSERT_EQ(operationMode, HMIOperationMode::OM_ECO_INACTIVE);
  ASSERT_EQ(operationType, HMIOperationType::OT_ALWAYS_ON);

  message.setAttr(HMI_ATTR_U8::OPERATION_MODE, HMIOperationMode::OM_BOOST);
  operationMode = static_cast<HMIOperationMode>(message.getAttr(HMI_ATTR_U8::OPERATION_MODE));
  operationType = static_cast<HMIOperationType>(message.getAttr(HMI_ATTR_U8::OPERATION_TYPE));
  ASSERT_EQ(operationMode, HMIOperationMode::OM_BOOST);
  ASSERT_EQ(operationType, HMIOperationType::OT_ALWAYS_ON);

  message.setAttr(HMI_ATTR_U8::OPERATION_MODE, HMIOperationMode::OM_AUTO);
  operationMode = static_cast<HMIOperationMode>(message.getAttr(HMI_ATTR_U8::OPERATION_MODE));
  operationType = static_cast<HMIOperationType>(message.getAttr(HMI_ATTR_U8::OPERATION_TYPE));
  ASSERT_EQ(operationMode, HMIOperationMode::OM_AUTO);
  ASSERT_EQ(operationType, HMIOperationType::OT_ALWAYS_ON);


    // set all five modes in timed mode
  message.setAttr(HMI_ATTR_U8::OPERATION_TYPE, HMIOperationType::OT_TIMER);
  message.setAttr(HMI_ATTR_U8::OPERATION_MODE, HMIOperationMode::OM_ABSENCE);
  operationMode = static_cast<HMIOperationMode>(message.getAttr(HMI_ATTR_U8::OPERATION_MODE));
  operationType = static_cast<HMIOperationType>(message.getAttr(HMI_ATTR_U8::OPERATION_TYPE));
  ASSERT_EQ(operationMode, HMIOperationMode::OM_ABSENCE);
  ASSERT_EQ(operationType, HMIOperationType::OT_TIMER);

  message.setAttr(HMI_ATTR_U8::OPERATION_MODE, HMIOperationMode::OM_ECO_ACTIVE);
  operationMode = static_cast<HMIOperationMode>(message.getAttr(HMI_ATTR_U8::OPERATION_MODE));
  operationType = static_cast<HMIOperationType>(message.getAttr(HMI_ATTR_U8::OPERATION_TYPE));
  ASSERT_EQ(operationMode, HMIOperationMode::OM_ECO_ACTIVE);
  ASSERT_EQ(operationType, HMIOperationType::OT_TIMER);

  message.setAttr(HMI_ATTR_U8::OPERATION_MODE, HMIOperationMode::OM_ECO_INACTIVE);
  operationMode = static_cast<HMIOperationMode>(message.getAttr(HMI_ATTR_U8::OPERATION_MODE));
  operationType = static_cast<HMIOperationType>(message.getAttr(HMI_ATTR_U8::OPERATION_TYPE));
  ASSERT_EQ(operationMode, HMIOperationMode::OM_ECO_INACTIVE);
  ASSERT_EQ(operationType, HMIOperationType::OT_TIMER);

  message.setAttr(HMI_ATTR_U8::OPERATION_MODE, HMIOperationMode::OM_BOOST);
  operationMode = static_cast<HMIOperationMode>(message.getAttr(HMI_ATTR_U8::OPERATION_MODE));
  operationType = static_cast<HMIOperationType>(message.getAttr(HMI_ATTR_U8::OPERATION_TYPE));
  ASSERT_EQ(operationMode, HMIOperationMode::OM_BOOST);
  ASSERT_EQ(operationType, HMIOperationType::OT_TIMER);

  message.setAttr(HMI_ATTR_U8::OPERATION_MODE, HMIOperationMode::OM_AUTO);
  operationMode = static_cast<HMIOperationMode>(message.getAttr(HMI_ATTR_U8::OPERATION_MODE));
  operationType = static_cast<HMIOperationType>(message.getAttr(HMI_ATTR_U8::OPERATION_TYPE));
  ASSERT_EQ(operationMode, HMIOperationMode::OM_AUTO);
  ASSERT_EQ(operationType, HMIOperationType::OT_TIMER);

}