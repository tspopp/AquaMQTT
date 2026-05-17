#include "message/optitronic2/ModbusRtuFrame.h"

namespace aquamqtt::message::optitronic2
{

uint16_t modbusRtuCrc16(const uint8_t* data, uint8_t length)
{
    uint16_t crc = 0xFFFF;
    for (uint8_t i = 0; i < length; i++)
    {
        crc ^= static_cast<uint16_t>(data[i]);
        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 0x0001)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc;
}

bool parseModbusFrame(const uint8_t* buffer, uint8_t length, ModbusFrame& frame)
{
    frame.valid = false;

    // minimum frame size: addr(1) + fn(1) + data(1) + crc(2) = 5
    if (length < 5)
    {
        return false;
    }

    // verify CRC
    uint16_t receivedCrc = static_cast<uint16_t>(buffer[length - 2]) | (static_cast<uint16_t>(buffer[length - 1]) << 8);
    uint16_t calculatedCrc = modbusRtuCrc16(buffer, length - 2);

    if (receivedCrc != calculatedCrc)
    {
        return false;
    }

    frame.slaveAddress = buffer[0];
    frame.functionCode = buffer[1];
    frame.dataLength   = length - 4;  // minus addr, fn, crc16
    if (frame.dataLength > sizeof(frame.data))
    {
        return false;
    }
    for (uint8_t i = 0; i < frame.dataLength; i++)
    {
        frame.data[i] = buffer[2 + i];
    }
    frame.valid = true;
    return true;
}

bool parseReadRequest(const ModbusFrame& frame, ModbusReadRequest& request)
{
    if (!frame.valid || frame.functionCode != FC_READ_HOLDING_REGISTERS)
    {
        return false;
    }
    if (frame.dataLength != 4)
    {
        return false;
    }

    request.startRegister = (static_cast<uint16_t>(frame.data[0]) << 8) | frame.data[1];
    request.quantity      = (static_cast<uint16_t>(frame.data[2]) << 8) | frame.data[3];
    return true;
}

bool parseReadResponse(const ModbusFrame& frame, ModbusReadResponse& response)
{
    if (!frame.valid || frame.functionCode != FC_READ_HOLDING_REGISTERS)
    {
        return false;
    }
    if (frame.dataLength < 1)
    {
        return false;
    }

    response.byteCount = frame.data[0];

    if (frame.dataLength != static_cast<uint8_t>(1 + response.byteCount))
    {
        return false;
    }
    if (response.byteCount % 2 != 0)
    {
        return false;
    }

    response.registerCount = response.byteCount / 2;
    if (response.registerCount > 50)
    {
        return false;
    }

    for (uint8_t i = 0; i < response.registerCount; i++)
    {
        response.registers[i] = (static_cast<uint16_t>(frame.data[1 + i * 2]) << 8) | frame.data[2 + i * 2];
    }

    return true;
}

bool parseWriteSingle(const ModbusFrame& frame, ModbusWriteSingle& write)
{
    if (!frame.valid || frame.functionCode != FC_WRITE_SINGLE_REGISTER)
    {
        return false;
    }
    if (frame.dataLength != 4)
    {
        return false;
    }

    write.registerAddress = (static_cast<uint16_t>(frame.data[0]) << 8) | frame.data[1];
    write.value           = (static_cast<uint16_t>(frame.data[2]) << 8) | frame.data[3];
    return true;
}

bool parseWriteMultiple(const ModbusFrame& frame, ModbusWriteMultiple& write)
{
    if (!frame.valid || frame.functionCode != FC_WRITE_MULTIPLE_REGISTERS)
    {
        return false;
    }
    if (frame.dataLength < 5)
    {
        return false;
    }

    write.startRegister = (static_cast<uint16_t>(frame.data[0]) << 8) | frame.data[1];
    write.quantity      = (static_cast<uint16_t>(frame.data[2]) << 8) | frame.data[3];
    uint8_t byteCount   = frame.data[4];

    if (byteCount != write.quantity * 2)
    {
        return false;
    }
    if (write.quantity > 50)
    {
        return false;
    }
    if (frame.dataLength != static_cast<uint8_t>(5 + byteCount))
    {
        return false;
    }

    for (uint16_t i = 0; i < write.quantity; i++)
    {
        write.values[i] = (static_cast<uint16_t>(frame.data[5 + i * 2]) << 8) | frame.data[6 + i * 2];
    }

    return true;
}

uint8_t buildReadRequest(uint8_t slaveAddr, uint16_t startReg, uint16_t qty, uint8_t* buffer)
{
    buffer[0] = slaveAddr;
    buffer[1] = FC_READ_HOLDING_REGISTERS;
    buffer[2] = static_cast<uint8_t>(startReg >> 8);
    buffer[3] = static_cast<uint8_t>(startReg & 0xFF);
    buffer[4] = static_cast<uint8_t>(qty >> 8);
    buffer[5] = static_cast<uint8_t>(qty & 0xFF);

    uint16_t crc = modbusRtuCrc16(buffer, 6);
    buffer[6] = static_cast<uint8_t>(crc & 0xFF);
    buffer[7] = static_cast<uint8_t>(crc >> 8);
    return 8;
}

uint8_t buildWriteSingle(uint8_t slaveAddr, uint16_t reg, uint16_t value, uint8_t* buffer)
{
    buffer[0] = slaveAddr;
    buffer[1] = FC_WRITE_SINGLE_REGISTER;
    buffer[2] = static_cast<uint8_t>(reg >> 8);
    buffer[3] = static_cast<uint8_t>(reg & 0xFF);
    buffer[4] = static_cast<uint8_t>(value >> 8);
    buffer[5] = static_cast<uint8_t>(value & 0xFF);

    uint16_t crc = modbusRtuCrc16(buffer, 6);
    buffer[6] = static_cast<uint8_t>(crc & 0xFF);
    buffer[7] = static_cast<uint8_t>(crc >> 8);
    return 8;
}

uint8_t buildWriteMultiple(uint8_t slaveAddr, uint16_t startReg, uint16_t qty, const uint16_t* values, uint8_t* buffer)
{
    buffer[0] = slaveAddr;
    buffer[1] = FC_WRITE_MULTIPLE_REGISTERS;
    buffer[2] = static_cast<uint8_t>(startReg >> 8);
    buffer[3] = static_cast<uint8_t>(startReg & 0xFF);
    buffer[4] = static_cast<uint8_t>(qty >> 8);
    buffer[5] = static_cast<uint8_t>(qty & 0xFF);
    buffer[6] = static_cast<uint8_t>(qty * 2);

    for (uint16_t i = 0; i < qty; i++)
    {
        buffer[7 + i * 2] = static_cast<uint8_t>(values[i] >> 8);
        buffer[8 + i * 2] = static_cast<uint8_t>(values[i] & 0xFF);
    }

    uint8_t  frameLen = 7 + qty * 2;
    uint16_t crc      = modbusRtuCrc16(buffer, frameLen);
    buffer[frameLen]     = static_cast<uint8_t>(crc & 0xFF);
    buffer[frameLen + 1] = static_cast<uint8_t>(crc >> 8);
    return frameLen + 2;
}

}  // namespace aquamqtt::message::optitronic2
