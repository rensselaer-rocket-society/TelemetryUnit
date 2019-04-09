#include "libs/Packet.h"
#include "libs/COBS.h"
#include "libs/CRC8.h"


Packet::Packet(HardwareSerial *serial)
{
    this->serial = serial;
    crcCalculator.begin();
}

void Packet::sendAltitude(float altitude, float temperature)
{
    float data[2];
    data[0] = altitude;
    data[1] = temperature;
    sendPacket(ALTITUDE, (uint8_t*)data, sizeof(data));
}

void Packet::sendGPS(float latitude, float longitude)
{
    float data[2];
    data[0] = latitude;
    data[1] = longitude;
    sendPacket(GPS, (uint8_t*)data, sizeof(data));
}

void Packet::sendAccel(float x_accel, float y_accel, float z_accel, float x_gyro, float y_gyro, float z_gyro)
{
    float data[6];
    data[0] = x_accel;
    data[1] = y_accel;
    data[2] = z_accel;
    data[3] = x_gyro;
    data[4] = y_gyro;
    data[5] = z_gyro;
    sendPacket(ACCEL, (uint8_t*)data, sizeof(data));
}

void Packet::sendPacket(PacketContent packet_content, uint8_t *data, uint8_t data_length)
{
    uint8_t *packet = new uint8_t[data_length + 4];
    packet[0] = packet_content;
    packet[1] = data_length + 4;
    packet[2] = sequence_num++;
    memcpy(&packet[3], data, data_length);
    packet[4] = crcCalculator.get_crc8(packet, packet[1] - 1);
    
    unsigned int encoded_size = byteStuffer.getEncodedBufferSize(packet[1]);
    uint8_t *encoded_buffer = new uint8_t[encoded_size];
    size_t tx_len = byteStuffer.encode(packet, packet[1], encoded_buffer);
    for (size_t i = 0; i < tx_len; i++) {
        serial->write(encoded_buffer[i]);
    }
    serial->write('\0'); //Send packet terminator
    delete encoded_buffer;
    delete packet;
}
