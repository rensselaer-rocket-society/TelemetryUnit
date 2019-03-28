#pragma once


#include "Arduino.h"
#include "CRC8.h"
#include "COBS.h"

class Packet
{
public:
    enum PacketContent : uint8_t { GPS, ALTITUDE, ACCEL };

    Packet(HardwareSerial *serial);

    void sendAltitude(float altitude, float temperature);
    void sendGPS(float latitude, float longitude);
    void sendAccel(	float x_accel, float y_accel, float z_accel,
    				float x_gyro, float y_gyro, float z_gyro);

private:
    void sendPacket(PacketContent packetContent, uint8_t *data, uint8_t data_length);

    HardwareSerial *serial;
    CRC8 crcCalculator;
    COBS byteStuffer;

    int sequence_num;

};
