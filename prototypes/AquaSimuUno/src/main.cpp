#include <Arduino.h>

#include <FastCRC.h>

void setup() {
    Serial.begin(9550, SERIAL_8N2);
}

uint8_t msg67_pl[] = {31, 0, 0, 0, 0, 0, 0, 0, 0, 109, 5, 116, 10, 0, 0, 35, 0, 0, 0, 117, 10, 0, 0, 16, 121, 20, 0, 0,
                      0, 0, 0};

uint8_t msg193_pl[] = {37, 8, 2, 200, 0, 193, 0, 204, 0, 0, 0, 0, 0, 1, 100, 65, 81, 0, 0, 0, 44, 62, 0, 255, 255, 255,
                       87, 9, 66, 88, 2, 0, 16, 14, 1, 78, 17};

FastCRC16 mCRC;

void send67() {
    Serial.write(67);
    Serial.flush();
    delayMicroseconds(200);
    Serial.write(msg67_pl, 31);
    uint16_t actualCRC = mCRC.ccitt(msg67_pl, 31);
    Serial.write((uint8_t) (actualCRC >> 8)); // extract the high byte
    Serial.write((uint8_t) (actualCRC & 0xFF)); // extract the low byte
};

void send193() {
    Serial.write(193);
    Serial.flush();
    delayMicroseconds(200);
    Serial.write(msg193_pl, 37);
    uint16_t actualCRC = mCRC.ccitt(msg193_pl, 37);
    Serial.write((uint8_t) (actualCRC >> 8)); // extract the high byte
    Serial.write((uint8_t) (actualCRC & 0xFF)); // extract the low byte
};

void send194() {
    Serial.write(194);
}

void loop() {
    send194();
    delay(100);
    send67();
    delay(100);
    send193();
    delay(200);
}