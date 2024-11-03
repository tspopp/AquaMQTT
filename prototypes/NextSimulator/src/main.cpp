#include <Arduino.h>

void setup() {
    Serial.begin(9550, SERIAL_8N2);
}

uint8_t msg193[] = {
        0xC1, 0x23, 0x2E, 0x00, 0x23, 0x00, 0x18, 0x00,
        0x18, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x82,
        0x00, 0x00, 0x00, 0x00, 0x45, 0x45, 0x28, 0xCB,
        0x00, 0x0E, 0x01, 0x11, 0x44
};

uint8_t msg193_len = 37;

uint8_t next194 = 0;

uint8_t msg194a[] = {
        0xC2, 0x22, 0x32, 0x12, 0x00, 0x00, 0x00, 0x10,
        0x00, 0x06, 0x2C, 0x01, 0xD0, 0x02, 0x00, 0x00,
        0x00, 0x3A, 0x21, 0x1E, 0x0C, 0x0C, 0x00, 0x00,
        0x00, 0x00, 0x4E, 0x45, 0x00, 0x00, 0x06, 0x04,
        0x22, 0x01, 0x3E, 0xFA
};
uint8_t msg194_len = 36;

uint8_t msg194b[] = {
        0xC2, 0x22, 0x32, 0x12, 0x00, 0x00, 0x00, 0x10, 0x00, 0x06,
        0x2C, 0x01, 0xD0, 0x02, 0x00, 0x00, 0x00, 0x3B,
        0x21, 0x1E, 0x0C, 0x0C, 0x00, 0x00, 0x00, 0x00,
        0x4E, 0x45, 0x00, 0x00, 0x06, 0x04, 0x22, 0x01,
        0x3E, 0xFB
};

uint8_t msg194c[] = {
        0xC2, 0x22, 0x32, 0x12, 0x00, 0x00, 0x00, 0x10, 0x00, 0x06,
        0x2C, 0x01, 0xD0, 0x02, 0x00, 0x00, 0x00, 0x00,
        0x21, 0x1E, 0x0D, 0x0C, 0x00, 0x00, 0x00, 0x00,
        0x4E, 0x45, 0x00, 0x00, 0x06, 0x04, 0x22, 0x01,
        0x3E, 0xC1
};

//uint8_t msg194d[] = {
//        0xC2, 0x22, 0x32, 0x12, 0x00, 0x00, 0x00, 0x10,
//        0x00, 0x06, 0x2C, 0x01, 0xD0, 0x02, 0x00, 0x00,
//        0x00, 0x01, 0x21, 0x1E, 0x0D, 0x0C, 0x00, 0x00,
//        0x00, 0x00, 0x4E, 0x45, 0x00, 0x00, 0x06, 0x04,
//        0x22, 0x01, 0x3E, 0xC0
//};


uint8_t msg194e[] = {
    0xC2, 0x22, 0x32, 0x12, 0x00, 0x00, 0x00, 0x10,
    0x00, 0x06, 0x2C, 0x01, 0xD0, 0x02, 0x00, 0x00,
    0x00, 0x02, 0x21, 0x1E, 0x0D, 0x0C, 0x00, 0x00,
    0x00, 0x00, 0x4E, 0x45, 0x00, 0x00, 0x06, 0x04,
    0x22, 0x01, 0x3E, 0xC3
};

uint8_t msg67[] = {
        0x43, 0x2D, 0x00, 0x00, 0x07, 0x1D, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x1D,
        0x00, 0x00, 0xEE, 0xB2, 0x39, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x4C,
        0x10, 0x5A, 0xFB, 0x27, 0xFB, 0x25, 0x0B, 0x1B,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x57,
};
uint8_t msg67_len = 47;

void loop() {
    if (next194 == 0) {
        Serial.write(msg194a, msg194_len);
        next194 = 1;
    } else if (next194 == 1) {
        Serial.write(msg194b, msg194_len);
        next194 = 2;
    } else if (next194 == 2) {
        Serial.write(msg194c, msg194_len);
        next194 = 3;
    }
    else if(next194 == 3) {
        Serial.write(msg194e, msg194_len);
        next194 = 0;
    }

    Serial.flush();
    delay(100);
    Serial.write(msg67, msg67_len);
    Serial.flush();
    delay(100);
    Serial.write(msg193, msg193_len);
    Serial.flush();
    delay(200);
    // TODO: error message not yet known
}