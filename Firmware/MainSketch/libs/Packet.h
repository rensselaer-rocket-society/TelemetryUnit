#pragma once


#include "Arduino.h"

class Packet
{
public:
    enum PacketContent { GPS, ALTITUDE, ACCEL };

    Packet(HardwareSerial *serial);

    void sendAltitude(uint8_t altitude, uint8_t temperature);

private:
    void sendPacket(uint8_t packetContent, uint8_t *data, uint8_t data_length):

    HardwareSerial *serial;

    int sequence_num;

};
