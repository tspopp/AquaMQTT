
#ifndef DUMMYPROTOCOL_H
#define DUMMYPROTOCOL_H

#include <set>
#include <memory>

#include <message/Factory.h>

namespace aquamqtt::test
{

uint8_t msg194_odyssee[] = {
    0x1d,  0x36,  0x02,
    0x00,  0x21,  0x00,  0x00,
    0x00,  0x00,  0x00,  0x00,
    0x00,  0x00,  0x00,  0x00,
    0x00,  0x00,  0x21,  0x32,
    0x26,  0x11,  0x00,  0x00,
    0x00,  0x00,  0x41,  0x42,
    0x00,  0x00
  };

uint8_t msg194_next[] = {
  0x22, 0x32, 0x12, 0x00, 0x00, 0x00, 0x10,
  0x00, 0x06, 0x2C, 0x01, 0xD0, 0x02, 0x00, 0x00,
  0x00, 0x3A, 0x21, 0x1E, 0x0C, 0x0C, 0x00, 0x00,
  0x00, 0x00, 0x4E, 0x45, 0x00, 0x00, 0x06, 0x04,
  0x22, 0x01, 0x3E, 0xFA
};

uint8_t msg194_legacy[] = {
  35,
  18,  2,
  66,
  252, 0,   240, 17, 240, 6,  16, 56,
  0,   0,
  255, 255, 0,
  33,
  147,
  46,
  11,
  11,
  0,   0,   255, 0,  9,   66, 0,  0,  255, 255, 255, 255, 255
};

static std::unique_ptr<std::set<std::unique_ptr<aquamqtt::message::IHMIMessage>>> createHMIMessagesInAllDerivates(bool legacy = true, bool next = true, bool odyssee = true	){
    auto set = std::make_unique<std::set<std::unique_ptr<aquamqtt::message::IHMIMessage>>>();
    if(odyssee){
        set->insert(std::move(aquamqtt::message::createHmiMessageFromBuffer(aquamqtt::message::ProtocolVersion::PROTOCOL_ODYSSEE, msg194_odyssee, nullptr)));
    }
    if(next){
    set->insert(std::move(aquamqtt::message::createHmiMessageFromBuffer(aquamqtt::message::ProtocolVersion::PROTOCOL_NEXT, msg194_next, nullptr)));
    }
    if(legacy){
    set->insert(std::move(aquamqtt::message::createHmiMessageFromBuffer(aquamqtt::message::ProtocolVersion::PROTOCOL_LEGACY, msg194_legacy, nullptr)));
    }
    return std::move(set);
}

}

#endif //DUMMYPROTOCOL_H
