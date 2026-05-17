#ifndef AQUAMQTT_MODBUS_RTU_FRAME_H
#define AQUAMQTT_MODBUS_RTU_FRAME_H

#include <cstdint>

namespace aquamqtt::message::optitronic2
{

/**
 * Modbus RTU function codes used by the Optitronic 2 protocol
 */
enum ModbusFunctionCode : uint8_t
{
    FC_READ_HOLDING_REGISTERS  = 0x03,
    FC_WRITE_SINGLE_REGISTER   = 0x06,
    FC_WRITE_MULTIPLE_REGISTERS = 0x10,
};

/**
 * Maximum Modbus RTU frame size.
 * Largest expected frame: fn=0x10 write of 42 registers (ventilation schedule)
 * = 1 (addr) + 1 (fn) + 2 (start) + 2 (qty) + 1 (byte count) + 84 (data) + 2 (CRC) = 93 bytes
 */
constexpr uint8_t MODBUS_MAX_FRAME_SIZE = 128;

/**
 * Optitronic 2 bus parameters
 */
constexpr uint8_t       MODBUS_SLAVE_ADDRESS = 0x01;
constexpr unsigned long MODBUS_BAUD_RATE     = 57600;

/**
 * Inter-frame silence detection threshold.
 * Modbus RTU spec: 3.5 character times. At 57600 baud (11 bits/char):
 * 3.5 * 11 / 57600 = ~0.67ms. We use 2ms for safety margin.
 */
constexpr unsigned long MODBUS_FRAME_SILENCE_MS = 2;

/**
 * Parsed Modbus RTU frame
 */
struct ModbusFrame
{
    uint8_t slaveAddress;
    uint8_t functionCode;
    uint8_t data[MODBUS_MAX_FRAME_SIZE - 4];  // minus addr, fn, crc16
    uint8_t dataLength;
    bool    valid;
};

/**
 * Parsed read request (fn=0x03 from master)
 */
struct ModbusReadRequest
{
    uint16_t startRegister;
    uint16_t quantity;
};

/**
 * Parsed read response (fn=0x03 from slave)
 */
struct ModbusReadResponse
{
    uint8_t  byteCount;
    uint16_t registers[50];  // max 50 registers per response
    uint8_t  registerCount;
};

/**
 * Parsed write single register (fn=0x06)
 */
struct ModbusWriteSingle
{
    uint16_t registerAddress;
    uint16_t value;
};

/**
 * Parsed write multiple registers (fn=0x10)
 */
struct ModbusWriteMultiple
{
    uint16_t startRegister;
    uint16_t quantity;
    uint16_t values[50];
};

/**
 * CRC-16/Modbus calculation
 */
uint16_t modbusRtuCrc16(const uint8_t* data, uint8_t length);

/**
 * Parse a raw byte buffer into a ModbusFrame structure.
 * Returns true if frame is valid (correct CRC and minimum length).
 */
bool parseModbusFrame(const uint8_t* buffer, uint8_t length, ModbusFrame& frame);

/**
 * Parse a read request from frame data (fn=0x03 master request)
 */
bool parseReadRequest(const ModbusFrame& frame, ModbusReadRequest& request);

/**
 * Parse a read response from frame data (fn=0x03 slave response)
 */
bool parseReadResponse(const ModbusFrame& frame, ModbusReadResponse& response);

/**
 * Parse a write single register from frame data (fn=0x06)
 */
bool parseWriteSingle(const ModbusFrame& frame, ModbusWriteSingle& write);

/**
 * Parse a write multiple registers from frame data (fn=0x10 master request)
 */
bool parseWriteMultiple(const ModbusFrame& frame, ModbusWriteMultiple& write);

/**
 * Build a Modbus RTU read request frame
 */
uint8_t buildReadRequest(uint8_t slaveAddr, uint16_t startReg, uint16_t qty, uint8_t* buffer);

/**
 * Build a Modbus RTU write single register frame
 */
uint8_t buildWriteSingle(uint8_t slaveAddr, uint16_t reg, uint16_t value, uint8_t* buffer);

/**
 * Build a Modbus RTU write multiple registers frame
 */
uint8_t buildWriteMultiple(uint8_t slaveAddr, uint16_t startReg, uint16_t qty, const uint16_t* values, uint8_t* buffer);

}  // namespace aquamqtt::message::optitronic2

#endif  // AQUAMQTT_MODBUS_RTU_FRAME_H
