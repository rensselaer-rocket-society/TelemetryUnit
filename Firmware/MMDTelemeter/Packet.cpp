#include "inc/Packet.h"
#include "inc/COBS.h"
#include "inc/CRC8.h"


Packet::Packet(HardwareSerial *serial)
{
    this->serial = serial;
    crcCalculator.begin();
}

void Packet::sendBattery(uint32_t time, uint16_t voltage)
{
    sendPacket(BATTERY, time, (uint8_t*)&voltage, sizeof(voltage));
}

void Packet::sendAltitude(uint32_t time, int32_t altitude, int16_t temperature)
{
    uint8_t data[6];
    data[0] = altitude;
    data[1] = (altitude>>8);
    data[2] = (altitude>>16);
    data[3] = (altitude>>24);
    data[4] = temperature;
    data[5] = temperature>>8;
    sendPacket(ALTITUDE, time, (uint8_t*)data, sizeof(data));
}

void Packet::sendGPS(uint32_t time, float latitude, float longitude)
{
    float data[2];
    data[0] = latitude;
    data[1] = longitude;
    sendPacket(GPS, time, (uint8_t*)data, sizeof(data));
}

void Packet::sendImu(uint32_t time, int16_t x_accel, int16_t y_accel, int16_t z_accel, int16_t x_gyro, int16_t y_gyro, int16_t z_gyro)
{
    int16_t data[6];
    data[0] = x_accel;
    data[1] = y_accel;
    data[2] = z_accel;
    data[3] = x_gyro;
    data[4] = y_gyro;
    data[5] = z_gyro;
    sendPacket(IMU, time, (uint8_t*)data, sizeof(data));
}

void Packet::sendAccelMag(uint32_t time, int16_t x_accel, int16_t y_accel, int16_t z_accel, int16_t x_mag, int16_t y_mag, int16_t z_mag)
{
    int16_t data[6];
    data[0] = x_accel;
    data[1] = y_accel;
    data[2] = z_accel;
    data[3] = x_mag;
    data[4] = y_mag;
    data[5] = z_mag;
    sendPacket(ACCEL_MAG, time, (uint8_t*)data, sizeof(data));
}

void Packet::sendPacket(PacketContent packet_content, uint32_t time, uint8_t *data, uint8_t data_length)
{
    uint8_t packet_length = data_length + 8; //type(1) + length(1) + seq(1) + crc8(1) + time(4)
    uint8_t *packet = new uint8_t[packet_length];
    packet[0] = packet_content;
    packet[1] = packet_length;
    packet[2] = sequence_num++;
    packet[3] = time; //Truncation of upper bits implied in uint to uchar conversion
    packet[4] = time >> 8;
    packet[5] = time >> 16;
    packet[6] = time >> 24;
    memcpy(packet+7, data, data_length);
    packet[packet_length-1] = crcCalculator.get_crc8(packet, packet_length-1); // Compute and insert CRC
    
    unsigned int encoded_size = byteStuffer.getEncodedBufferSize(packet_length);
    uint8_t *encoded_buffer = new uint8_t[encoded_size];
    size_t tx_len = byteStuffer.encode(packet, packet[1], encoded_buffer);
    for (size_t i = 0; i < tx_len; i++) {
        serial->write(encoded_buffer[i]);
    }
    serial->write('\0'); //Send packet terminator
    delete encoded_buffer;
    delete packet;
}
