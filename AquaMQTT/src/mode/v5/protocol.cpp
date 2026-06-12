#include "protocol.h"

#include <cstring>

#include <FastCRC.h>

#include "dhwstate.h"
#include "util.h"


namespace aquamqtt {

bool check_crc(const uint8_t* frame, const uint8_t len)
{
    uint16_t actualCRC  = calculate_crc(frame, len - 2);
    uint16_t messageCRC = frame[len-1] << 8 | frame[len-2];
    return messageCRC == actualCRC;
}

uint16_t calculate_crc(const uint8_t* frame, const uint8_t len)
{
    static FastCRC16 mCRC;

    return mCRC.modbus(frame, len);
}

float parse_temperature(const uint8_t* bytes)
{
    int16_t value = bytes[0] << 8 | bytes[1];
    return (float)value / 100;
}

uint32_t parse_uint32(const uint8_t* bytes)
{
    int32_t value = bytes[0] << 24 | bytes[1] << 16 | bytes[2] << 8 | bytes[3];
    return value;
}

bool check_payload_size(const Frame &frame, int expected_size, const char* frame_type_name) {
    if (frame.payload_size() == 0) {
        return false; // no payload
    }

    if (frame.payload_size() != expected_size) {
        LOG.print("[");
        LOG.print(frame.getChannelName());
        LOG.print("] invalid frame: ");
        LOG.print(frame_type_name);
        LOG.print(" should have payload size ");
        LOG.print(expected_size);
        LOG.print("; found: ");
        LOG.println(frame.payload_size());
        return false;
    }

    return true;
}

void process_temperature_frame(DhwState &state, const Frame &frame) {
    /**
     * byte 0-1: water temperature
     * byte 2-3: compressor outlet temperature
     * byte 4-5: air inlet temperature
     * byte 6-7: evaporator 1 temperature
     * byte 8-9: evaporator 2 temperature
     * byte 10-11: evaporator 3 temperature
     */

    if (!check_payload_size(frame, 12, "temperature")) {
        return;
    }

    const uint8_t* payload = frame.payload();
    state.water_temperature->set_value(parse_temperature(&payload[0]));
    state.compressor_outlet_temperature->set_value(parse_temperature(&payload[2]));
    state.air_inlet_temperature->set_value(parse_temperature(&payload[4]));
    state.evaporator1_temperature->set_value(parse_temperature(&payload[6]));
    state.evaporator2_temperature->set_value(parse_temperature(&payload[8]));
    state.evaporator3_temperature->set_value(parse_temperature(&payload[10]));
}

void process_single_temperature_frame(const Frame &frame, Sensor* entity) {
    if (!check_payload_size(frame, 2, "single_temperature")) {
        return;
    }

    const uint8_t* payload = frame.payload();
    entity->set_value(parse_temperature(&payload[0]));
}

void process_extreme_temperature_frame(const Frame &frame, Sensor* min, Sensor* max) {
    /**
     * byte 0 is always 00
     * byte 1-2 is the smallest value
     * byte 3-4 is the greatest value
     */

     if (!check_payload_size(frame, 5, "temperature")) {
        return;
    }

    const uint8_t* payload = frame.payload();

    if (payload[0] != 0x00) {
        LOG.print(format_string("[%s] invalid frame: extreme temperature: first payload byte should be 0; found: %02x", frame.getChannelName(), payload[0]).c_str());
        return;
    }

    min->set_value(parse_temperature(&payload[1]));
    max->set_value(parse_temperature(&payload[3]));
}

void process_input_frame(DhwState &state, Frame &frame) {
    /**
     * byte 0: boolean value for input I2 (0=no signal; 1=signal)
     * byte 1: boolean value for input I1 (0=no signal; 1=signal)
     * byte 2: boolean value for heating active (0=idle; 1=heating)
     */

     if (!check_payload_size(frame, 3, "input")) {
        return;
    }

    state.input_i2->set_value(frame.payload()[0]);
    state.input_i1->set_value(frame.payload()[1]);
    state.heating_active->set_value(frame.payload()[2]);

    uint8_t payload[] = {0x00, 0x00, frame.payload()[2]};
    switch (state.operation_mode->getIndex()) {
    case OPERATION_MODE_USE_INPUT:
        break;
    case OPERATION_MODE_NORMAL:
        payload[0] = 0x00; // i2
        payload[1] = 0x00; // i1
        frame.replace_payload(payload);
        break;
    case OPERATION_MODE_EAGER:
        payload[0] = 0x00; // i2
        payload[1] = 0x01; // i1
        frame.replace_payload(payload);
        break;
    case OPERATION_MODE_OFF:
        payload[0] = 0x01; // i2
        payload[1] = 0x00; // i1
        frame.replace_payload(payload);
        break;
    case OPERATION_MODE_BOOST:
        payload[0] = 0x01; // i2
        payload[1] = 0x01; // i1
        frame.replace_payload(payload);
        break;
    default:
        break;
    }
}

void process_text_frame(Frame &frame, TextSensor* entity) {
    if (frame.payload_size() == 0) {
        return; // no payload
    }

    if (frame.payload()[frame.payload_size()-1] != 0x00) {
        LOG.print("[");
        LOG.print(frame.getChannelName());
        LOG.print("] invalid frame: payload should be null-terminated");
        LOG.print("; found: ");
        LOG.println(frame.getBufferAsString().c_str());
        return;
    }

    entity->set_state((char*)frame.payload());
}

void process_cycle_frame(Frame &frame, BinarySensor* active_entity, Sensor* count_entity) {
    if (frame.payload_size() == 0) {
        return; // no payload
    }

    if (frame.payload_size() != 12) {
        LOG.print("[");
        LOG.print(frame.getChannelName());
        LOG.print("] invalid cycle frame size: expected: 12; found: ");
        LOG.print(frame.payload_size());
        LOG.print("; frame: ");
        LOG.println(frame.getBufferAsString().c_str());
        return;
    }

    uint32_t seconds1 = parse_uint32(&frame.payload()[0]);
    uint32_t seconds2 = parse_uint32(&frame.payload()[4]);
    uint32_t n_cycles = parse_uint32(&frame.payload()[8]);

    active_entity->set_value(seconds2 > 0);
    count_entity->set_value(n_cycles);
}

void process_frame(Frame &frame)
{
    DhwState &state = DhwState::getInstance();

    uint64_t header = frame.getHeaderValue();

    switch(header) {
    case 0x0164006401: // version number
        break;
    case 0x0164006501: // serial number?
        break;
    case 0x0164006601: // heat pump serial number
        process_text_frame(frame, state.serial_number);
        break;
    case 0x0164006701: // power board version
        process_text_frame(frame, state.power_board_version);
        break;
    case 0x0164006E01: // model type?
        process_text_frame(frame, state.controller_model);
        break;
    case 0x016414B701: // water temperature setpoint
        process_single_temperature_frame(frame, state.setpoint);
        break;
    case 0x0164FEB006:
        process_temperature_frame(state, frame);
        break;
    case 0x0164FF1403:
        process_input_frame(state, frame);
        break;
    case 0x0164FEBA03:
        process_extreme_temperature_frame(frame, state.water_temperature_min, state.water_temperature_max);
        break;
    case 0x0164FEBD03:
        process_extreme_temperature_frame(frame, state.compressor_outlet_temperature_min, state.compressor_outlet_temperature_max);
        break;
    case 0x0164FEC003:
        process_extreme_temperature_frame(frame, state.air_inlet_temperature_min, state.air_inlet_temperature_max);
        break;
    case 0x0164FEC303:
        // process_extreme_temperature_frame: evaporator1
        break;
    case 0x0164FEC603:
        // process_extreme_temperature_frame: evaporator2
        break;
    case 0x0164FEC903:
        // process_extreme_temperature_frame: evaporator3
        break;
    case 0x0164FEE203:
        // counter1
        process_cycle_frame(frame, state.cycle1_active, state.cycle1_count);
        break;
    case 0x0164FEE503:
        // counter2
        process_cycle_frame(frame, state.cycle2_active, state.cycle2_count);
        break;
    case 0x0164FEE803:
        // counter3
        process_cycle_frame(frame, state.cycle3_active, state.cycle3_count);
        break;
    case 0x0164FEEB03:
        // counter4
        process_cycle_frame(frame, state.cycle4_active, state.cycle4_count);
        break;
    case 0x0164FEEE03:
        // counter5
        process_cycle_frame(frame, state.cycle5_active, state.cycle5_count);
        break;
    case 0x0164FEF103:
        // counter6
        process_cycle_frame(frame, state.cycle6_active, state.cycle6_count);
        break;

    case 0x0165000301: // HMI firmware version
        process_text_frame(frame, state.hmi_version);
        break;
    case 0x0165000A01: // HMI model type?
        process_text_frame(frame, state.hmi_model);
        break;
    case 0x0165152301: // unknown
        break;
    case 0x016516B301: // unknown
        break;
    case 0x0165FDF802: // unknown
        break;
    case 0x0165FDFB02: // unknown
        break;
    case 0x0165FDFE02: // unknown
        break;
    case 0x0165FEF701:
        // unknown message
        break;
    case 0x0165FEF901:
        // unknown message
        break;
    case 0x0165FEFB01:
        // unknown message
        break;
    case 0x0165FEFD01:
        // unknown message
        break;
    case 0x0165FEFF01:
        // unknown message
        break;
    case 0x0165FF0101:
        // unknown message
        break;
    case 0x0165FF0301:
        // unknown message
        break;
    }
}

/**
 * return true if the frame is valid
 */
bool V5Protocol::processFrame(message::FrameBufferChannel channel, uint8_t* buffer, uint8_t len, char* err_message, uint8_t err_message_limit) {
    Frame frame(channel, buffer, len);
    if (len < HEADER_LENGTH + 2) {
        snprintf(err_message, err_message_limit, "frame too short: channel=%s; len=%d; frame=%s", frame.getChannelName(), frame.get_buffer_size(), frame.getBufferAsString().c_str());
        return false;
    }

    if (buffer[0] != 0x01 || (buffer[1] != 0x64 && buffer[1] != 0x65)) {
        snprintf(err_message, err_message_limit, "invalid header: channel=%s; len=%d; frame=%s", frame.getChannelName(), frame.get_buffer_size(), frame.getBufferAsString().c_str());
        return false;
    }

    if (len > HEADER_LENGTH + 2 && frame.payload_size() != buffer[HEADER_LENGTH]) {
        int payload_declared_size = buffer[HEADER_LENGTH];
        snprintf(err_message, err_message_limit, "inconsistent payload size: channel=%s; len=%d; frame=%s; payload size=%d; payload declared=%d",
            frame.getChannelName(), len, frame.getBufferAsString().c_str(), frame.payload_size(), payload_declared_size);
        return false;
    }

    if (!check_crc(buffer, len)) {
        snprintf(err_message, err_message_limit, "bad checksum: channel=%s; len=%d; frame=%s",
            frame.getChannelName(), len, frame.getBufferAsString().c_str());
        return false;
    }

    if (frame.payload_size() == 0) {
        return true; // valid frame, but no payload
    }

    process_frame(frame);
    if (frame.isModified()) {
        // update buffer
        memcpy(buffer, frame.get_buffer(), len);
    }
    return true;
}

bool V5Protocol::frameIsReady(message::FrameBufferChannel channel, uint8_t* buffer, uint8_t len)
{
    if (len == 1 && buffer[0] != 0x01) {
        return true; // invalid frame -> to be discarded
    }

    if (len == 2 && (buffer[1] & 0xfe) != 0x64) {
        return true; // invalid frame -> to be discarded
    }

    if (len < HEADER_LENGTH + 2) {
        return false; // incomplete frame
    }

    if (len == HEADER_LENGTH + 2) {
        if (channel == message::FrameBufferChannel::CH_HMI && buffer[1] == 0x64) {
            return true; // request message from HMI without a payload
        } else if (channel == message::FrameBufferChannel::CH_MAIN && buffer[1] == 0x65) {
            return true; // command response from MAIN without a payload
        } else {
            return false; // incomplete frame
        }
    }

    // message with paylaod
    uint8_t frame_len = HEADER_LENGTH + 3 + buffer[HEADER_LENGTH];
    return len >= frame_len;
}

} // namespace aquamqtt
