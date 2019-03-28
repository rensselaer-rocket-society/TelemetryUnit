#include "Packet.h"
#include "COBS.h"
#include "CRC8.h"


Packet::Packet(HardwareSerial *serial)
{
    this->serial = serial;
    CRC8.begin()
}

Packet::sendAltitude(uint8_t altitude, uint8_t temperature)
{
    //uint8_t *data = 
    //sendPacket(PacketContent.ALTITUDE, 
}

Packet::sendPacket(uint8_t packet_content, uint8_t *data, uint8_t data_length)
{
    uint8_t *packet = new uint8_t[data_length + 4];
    packet[0] = packet_content;
    packet[1] = data_length + 4;
    packet[2] = sequence_num++;
    memcpy(&packet[3], data, data_length);
    packet[4] = CRC8.get_crc8(packet, packet[1] - 1);
    
    encoded_size = COBS.getEncodedBufferSize(packet[1]);
    uint8_t *encoded_buffer = new uint8_t[encoded_size];
    size_t tx_len = COBS.encode(packet, packet[1], encoded_buffer);
    for (size_t i = 0; i < tx_len; i++) {
        serial->write(encoded_buffer[i]);
    }
}
