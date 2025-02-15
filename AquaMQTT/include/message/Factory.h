#ifndef MESSAGE_FACTORY_H
#define MESSAGE_FACTORY_H

#include <memory>

#include "IEnergyMessage.h"
#include "IHMIMessage.h"
#include "IMainMessage.h"
#include "MessageConstants.h"
#include "legacy/ErrorMessage.h"
#include "legacy/HMIMessage.h"
#include "legacy/MainEnergyMessage.h"
#include "legacy/MainStatusMessage.h"
#include "next/ErrorMessage.h"
#include "next/HMIMessage.h"
#include "next/MainEnergyMessage.h"
#include "next/MainStatusMessage.h"
#include "odyssee/ErrorMessage.h"
#include "odyssee/ExtraMessage.h"
#include "odyssee/HMIMessage.h"
#include "odyssee/MainEnergyMessage.h"
#include "odyssee/MainStatusMessage.h"

namespace aquamqtt::message
{
class IExtraMessage;
class IErrorMessage;

static std::unique_ptr<IHMIMessage>
        createHmiMessageFromBuffer(const ProtocolVersion version, uint8_t* buffer, uint8_t* previousBuffer = nullptr)
{
    std::unique_ptr<IHMIMessage> message;
    if (version == message::PROTOCOL_LEGACY)
    {
        message = std::make_unique<legacy::HMIMessage>(buffer, previousBuffer);
    }
    else if (version == PROTOCOL_ODYSSEE)
    {
        message = std::make_unique<odyssee::HMIMessage>(buffer, previousBuffer);
    }
    else
    {
        message = std::make_unique<next::HMIMessage>(buffer, previousBuffer);
    }

    return std::move(message);
}

static std::unique_ptr<IMainMessage>
        createMainMessageFromBuffer(const ProtocolVersion version, uint8_t* buffer, uint8_t* previousBuffer = nullptr)
{
    std::unique_ptr<IMainMessage> message;
    if (version == PROTOCOL_NEXT)
    {
        message = std::make_unique<next::MainStatusMessage>(buffer, previousBuffer);
    }
    else if (version == PROTOCOL_ODYSSEE)
    {
        message = std::make_unique<odyssee::MainStatusMessage>(buffer, previousBuffer);
    }
    else
    {
        message = std::make_unique<legacy::MainStatusMessage>(buffer, previousBuffer);
    }
    return std::move(message);
}

static std::unique_ptr<IEnergyMessage> createEnergyMessageFromBuffer(
        const ProtocolVersion version,
        uint8_t*              buffer,
        uint8_t*              previousBuffer = nullptr)
{
    std::unique_ptr<message::IEnergyMessage> message;
    if (version == PROTOCOL_NEXT)
    {
        message = std::make_unique<next::MainEnergyMessage>(buffer, previousBuffer);
    }
    else if (version == PROTOCOL_ODYSSEE)
    {
        message = std::make_unique<odyssee::MainEnergyMessage>(buffer, previousBuffer);
    }
    else
    {
        message = std::make_unique<legacy::MainEnergyMessage>(buffer, previousBuffer);
    }
    return std::move(message);
}

static std::unique_ptr<IErrorMessage> createErrorMessageFromBuffer(const ProtocolVersion version, uint8_t* buffer)
{
    std::unique_ptr<IErrorMessage> message;
    if (version == PROTOCOL_NEXT)
    {
        message = std::make_unique<next::ErrorMessage>(buffer);
    }
    else if (version == PROTOCOL_ODYSSEE)
    {
        message = std::make_unique<odyssee::ErrorMessage>(buffer);
    }
    else
    {
        message = std::make_unique<legacy::ErrorMessage>(buffer);
    }

    return std::move(message);
}

static std::unique_ptr<IExtraMessage>
        createExtraMessageFromBuffer(const ProtocolVersion version, uint8_t* buffer, uint8_t* previousBuffer = nullptr)
{
    std::unique_ptr<IExtraMessage> message = std::make_unique<odyssee::ExtraMessage>(buffer, previousBuffer);
    return std::move(message);
}

}  // namespace aquamqtt::message

#endif  // MESSAGE_FACTORY_H
