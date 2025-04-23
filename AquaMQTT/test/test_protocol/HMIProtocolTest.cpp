#include <gtest/gtest.h>

#include <ostream>

#include "include/ProtocolTestData.h"

using namespace aquamqtt::message;
using namespace aquamqtt::test;

TEST(ProtocolTest, SetAndGetWaterTemperature)
{
    std::unique_ptr<std::set<std::unique_ptr<aquamqtt::message::IHMIMessage>>> messages = createHMIMessagesInAllDerivates();

    float waterTempTarget = 0.0;
    for (const auto& message : *messages) {
        message->setAttr(       HMI_ATTR_FLOAT::WATER_TARGET_TEMPERATURE, 62.0);
        waterTempTarget = message->getAttr(HMI_ATTR_FLOAT::WATER_TARGET_TEMPERATURE);
        EXPECT_FLOAT_EQ(62.0, waterTempTarget);

        message->setAttr(       HMI_ATTR_FLOAT::WATER_TARGET_TEMPERATURE, 20.0);
        waterTempTarget = message->getAttr(HMI_ATTR_FLOAT::WATER_TARGET_TEMPERATURE);
        EXPECT_FLOAT_EQ(20.0, waterTempTarget);
    }
}

TEST(ProtocolTest, SetAndGetDate)
{
    std::unique_ptr<std::set<std::unique_ptr<aquamqtt::message::IHMIMessage>>> messages = createHMIMessagesInAllDerivates();
    for (const auto& message : *messages) {

    for (int year = 2000; year <= 2025; ++year) {
        for (int month = 1; month <= 12; ++month) {
            int daysInMonth;
            if (month == 2) {
                daysInMonth = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ? 29 : 28;
            } else if (month == 4 || month == 6 || month == 9 || month == 11) {
                daysInMonth = 30;
            } else {
                daysInMonth = 31;
            }
            for (int day = 1; day <= daysInMonth; ++day) {
                message->setAttr(HMI_ATTR_U8::DATE_DAY, day);
                message->setDateMonthAndYear(month, year);

                ASSERT_EQ(message->getAttr(HMI_ATTR_U8::DATE_DAY), day);
                ASSERT_EQ(message->getAttr(HMI_ATTR_U8::DATE_MONTH), month);
                ASSERT_EQ(message->getAttr(HMI_ATTR_U16::DATE_YEAR), year);
            }
        }
    }
    }
}

TEST(ProtocolTest, SetAndGetTime)
{
    std::unique_ptr<std::set<std::unique_ptr<aquamqtt::message::IHMIMessage>>> messages = createHMIMessagesInAllDerivates();
    for (const auto& message : *messages) {

        if(message->hasAttr(HMI_ATTR_U8::TIME_SECONDS)){
            for(int seconds = 0; seconds <= 59; ++seconds) {
            	message->setAttr(HMI_ATTR_U8::TIME_SECONDS, seconds);
            	ASSERT_EQ(message->getAttr(HMI_ATTR_U8::TIME_SECONDS), seconds);
        	}
        }

        for(int minutes = 0; minutes <= 59; ++minutes) {
            message->setAttr(HMI_ATTR_U8::TIME_MINUTES, minutes);
            ASSERT_EQ(message->getAttr(HMI_ATTR_U8::TIME_MINUTES), minutes);
        }

        for(int hours = 0; hours <= 24; ++hours) {
            message->setAttr(HMI_ATTR_U8::TIME_HOURS, hours);
            ASSERT_EQ(message->getAttr(HMI_ATTR_U8::TIME_HOURS), hours);
        }

    }
}

TEST(ProtocolTest, SetAndGetEmergencyMode)
{
        std::unique_ptr<std::set<std::unique_ptr<aquamqtt::message::IHMIMessage>>> messages = createHMIMessagesInAllDerivates();
    for (const auto& message : *messages) {

        // heating element must be enabled before enabling emergency mode
        message->setAttr(HMI_ATTR_BOOL::HEATING_ELEMENT_ALLOWED, true);

        if(message->hasAttr(HMI_ATTR_BOOL::EMERGENCY_MODE_ENABLED)){

        message->setAttr(HMI_ATTR_BOOL::EMERGENCY_MODE_ENABLED, true);
        ASSERT_TRUE(message->getAttr(HMI_ATTR_BOOL::EMERGENCY_MODE_ENABLED));

		message->setAttr(HMI_ATTR_BOOL::EMERGENCY_MODE_ENABLED, false);
		ASSERT_FALSE(message->getAttr(HMI_ATTR_BOOL::EMERGENCY_MODE_ENABLED));

        }
    }
}

TEST(ProtocolTest, SetAndGetHeatingElementEnabled)
{
    std::unique_ptr<std::set<std::unique_ptr<aquamqtt::message::IHMIMessage>>> messages = createHMIMessagesInAllDerivates();
    for (const auto& message : *messages) {

        if(message->hasAttr(HMI_ATTR_BOOL::HEATING_ELEMENT_ALLOWED)){

        message->setAttr(HMI_ATTR_BOOL::HEATING_ELEMENT_ALLOWED, true);
        ASSERT_TRUE(message->getAttr(HMI_ATTR_BOOL::HEATING_ELEMENT_ALLOWED));

		message->setAttr(HMI_ATTR_BOOL::HEATING_ELEMENT_ALLOWED, false);
		ASSERT_FALSE(message->getAttr(HMI_ATTR_BOOL::HEATING_ELEMENT_ALLOWED));

        }
    }
}

TEST(ProtocolTest, SetAndGetInstallationMode)
{
    std::unique_ptr<std::set<std::unique_ptr<aquamqtt::message::IHMIMessage>>> messages = createHMIMessagesInAllDerivates();
    for (const auto& message : *messages) {
        if(message->hasAttr(HMI_ATTR_U8::STATE_INSTALLATION_MODE)){

            message->setAttr(HMI_ATTR_U8::STATE_INSTALLATION_MODE, HMIInstallation::INST_HP_ONLY);
			ASSERT_EQ(message->getAttr(HMI_ATTR_U8::STATE_INSTALLATION_MODE), HMIInstallation::INST_HP_ONLY);

            message->setAttr(HMI_ATTR_U8::STATE_INSTALLATION_MODE, HMIInstallation::INST_HP_AND_EXT_PRIO_HP);
			ASSERT_EQ(message->getAttr(HMI_ATTR_U8::STATE_INSTALLATION_MODE), HMIInstallation::INST_HP_AND_EXT_PRIO_HP);

			message->setAttr(HMI_ATTR_U8::STATE_INSTALLATION_MODE, HMIInstallation::INST_HP_AND_EXT_OPT_HP);
			ASSERT_EQ(message->getAttr(HMI_ATTR_U8::STATE_INSTALLATION_MODE), HMIInstallation::INST_HP_AND_EXT_OPT_HP);

            message->setAttr(HMI_ATTR_U8::STATE_INSTALLATION_MODE, HMIInstallation::INST_HP_AND_EXT_OPT_EXT);
			ASSERT_EQ(message->getAttr(HMI_ATTR_U8::STATE_INSTALLATION_MODE), HMIInstallation::INST_HP_AND_EXT_OPT_EXT);

            message->setAttr(HMI_ATTR_U8::STATE_INSTALLATION_MODE, HMIInstallation::INST_HP_AND_EXT_PRIO_EXT);
			ASSERT_EQ(message->getAttr(HMI_ATTR_U8::STATE_INSTALLATION_MODE), HMIInstallation::INST_HP_AND_EXT_PRIO_EXT);

            message->setAttr(HMI_ATTR_U8::STATE_INSTALLATION_MODE, HMIInstallation::INST_HP_AND_SOLAR);
			ASSERT_EQ(message->getAttr(HMI_ATTR_U8::STATE_INSTALLATION_MODE), HMIInstallation::INST_HP_AND_SOLAR);
        }
    }
}

TEST(ProtocolTest, SetAndGetOperationMode)
{
    std::unique_ptr<std::set<std::unique_ptr<aquamqtt::message::IHMIMessage>>> messages = createHMIMessagesInAllDerivates();
    for (const auto& message : *messages) {
		message->setAttr(HMI_ATTR_U8::OPERATION_MODE, HMIOperationMode::OM_ABSENCE);
        ASSERT_EQ(message->getAttr(HMI_ATTR_U8::OPERATION_MODE), HMIOperationMode::OM_ABSENCE);

        message->setAttr(HMI_ATTR_U8::OPERATION_MODE, HMIOperationMode::OM_ECO_ACTIVE);
        ASSERT_EQ(message->getAttr(HMI_ATTR_U8::OPERATION_MODE), HMIOperationMode::OM_ECO_ACTIVE);

        message->setAttr(HMI_ATTR_U8::OPERATION_MODE, HMIOperationMode::OM_ECO_INACTIVE);
        ASSERT_EQ(message->getAttr(HMI_ATTR_U8::OPERATION_MODE), HMIOperationMode::OM_ECO_INACTIVE);

        message->setAttr(HMI_ATTR_U8::OPERATION_MODE, HMIOperationMode::OM_BOOST);
        ASSERT_EQ(message->getAttr(HMI_ATTR_U8::OPERATION_MODE), HMIOperationMode::OM_BOOST);

        message->setAttr(HMI_ATTR_U8::OPERATION_MODE, HMIOperationMode::OM_AUTO);
        ASSERT_EQ(message->getAttr(HMI_ATTR_U8::OPERATION_MODE), HMIOperationMode::OM_AUTO);
    }
}

TEST(ProtocolTest, SetAndGetOperationType)
{
    std::unique_ptr<std::set<std::unique_ptr<aquamqtt::message::IHMIMessage>>> messages = createHMIMessagesInAllDerivates();
    for (const auto& message : *messages) {

        message->setAttr(HMI_ATTR_U8::OPERATION_TYPE, HMIOperationType::OT_ALWAYS_ON);
        ASSERT_EQ(message->getAttr(HMI_ATTR_U8::OPERATION_TYPE), HMIOperationType::OT_ALWAYS_ON);

        message->setAttr(HMI_ATTR_U8::OPERATION_TYPE, HMIOperationType::OT_TIMER);
        ASSERT_EQ(message->getAttr(HMI_ATTR_U8::OPERATION_TYPE), HMIOperationType::OT_TIMER);
    }
}

TEST(ProtocolTest, SetAndGetOperationTypeOdyssee)
{
    // additional test for odyssee protocol, since it has additional operation type OFF_PEAK_HOURS
    std::unique_ptr<std::set<std::unique_ptr<aquamqtt::message::IHMIMessage>>> messages = createHMIMessagesInAllDerivates(false, false, true);
    for (const auto& message : *messages) {
        message->setAttr(HMI_ATTR_U8::OPERATION_TYPE, HMIOperationType::OT_OFF_PEAK_HOURS);
        ASSERT_EQ(message->getAttr(HMI_ATTR_U8::OPERATION_TYPE), HMIOperationType::OT_OFF_PEAK_HOURS);
    }
}

TEST(ProtocolTest, SetAndGetFanExhaustMode)
{
	// not supported by odysee protocol
    std::unique_ptr<std::set<std::unique_ptr<aquamqtt::message::IHMIMessage>>> messages = createHMIMessagesInAllDerivates(true, true, false);
    for (const auto& message : *messages) {
        message->setAttr(HMI_ATTR_U8::CONFIG_FAN_EXHAUST, HMIFanExhaust::EXHAUST_STOP);
        ASSERT_EQ(message->getAttr(HMI_ATTR_U8::CONFIG_FAN_EXHAUST), HMIFanExhaust::EXHAUST_STOP);

        message->setAttr(HMI_ATTR_U8::CONFIG_FAN_EXHAUST, HMIFanExhaust::EXHAUST_LOW_SPEED);
        ASSERT_EQ(message->getAttr(HMI_ATTR_U8::CONFIG_FAN_EXHAUST), HMIFanExhaust::EXHAUST_LOW_SPEED);

        message->setAttr(HMI_ATTR_U8::CONFIG_FAN_EXHAUST, HMIFanExhaust::EXHAUST_HIGH_SPEED);
        ASSERT_EQ(message->getAttr(HMI_ATTR_U8::CONFIG_FAN_EXHAUST), HMIFanExhaust::EXHAUST_HIGH_SPEED);
    }
}

TEST(ProtocolTest, SetAndGetAirductConfig)
{
    std::unique_ptr<std::set<std::unique_ptr<aquamqtt::message::IHMIMessage>>> messages = createHMIMessagesInAllDerivates();
    for (const auto& message : *messages) {
        message->setAttr(HMI_ATTR_U8::CONFIG_AIRDUCT, HMIAirDuctConfig::AD_INT_INT);
        ASSERT_EQ(message->getAttr(HMI_ATTR_U8::CONFIG_AIRDUCT), HMIAirDuctConfig::AD_INT_INT);

        message->setAttr(HMI_ATTR_U8::CONFIG_AIRDUCT, HMIAirDuctConfig::AD_INT_EXT);
        ASSERT_EQ(message->getAttr(HMI_ATTR_U8::CONFIG_AIRDUCT), HMIAirDuctConfig::AD_INT_EXT);

        message->setAttr(HMI_ATTR_U8::CONFIG_AIRDUCT, HMIAirDuctConfig::AD_EXT_EXT);
        ASSERT_EQ(message->getAttr(HMI_ATTR_U8::CONFIG_AIRDUCT), HMIAirDuctConfig::AD_EXT_EXT);
    }
}
