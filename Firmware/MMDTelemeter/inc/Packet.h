#pragma once


#include "Arduino.h"
#include "CRC8.h"
#include "COBS.h"

class Packet
{
public:
    enum PacketContent : uint8_t { GPS=0x01, ALTITUDE=0x02, ACCEL=0x03, BATTERY=0x04 };

    Packet(HardwareSerial *serial);

    void sendBattery(uint32_t time, uint16_t voltage);
    void sendAltitude(uint32_t time, int32_t altitude, int16_t temperature);
    void sendGPS(uint32_t time, float latitude, float longitude);
    void sendImu(uint32_t time, 	int16_t x_accel,  int16_t y_accel,   int16_t z_accel,
    				                int16_t x_gyro,   int16_t y_gyro,    int16_t z_gyro);

private:
    void sendPacket(PacketContent packetContent, uint32_t time, uint8_t *data, uint8_t data_length);

    HardwareSerial *serial;
    CRC8 crcCalculator;
    COBS byteStuffer;

    uint8_t sequence_num;

};
