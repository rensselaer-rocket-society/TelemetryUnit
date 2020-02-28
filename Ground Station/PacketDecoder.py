import threading
import time

import serial

import random
from cobs import cobs
import crc8
import struct
from fixedint import UInt8

# Correspondance by packet type ID [None, GPS, Alt, Accel]
GPS_DATA_FORMAT = '<Iff'
ALTIMETER_DATA_FORMAT = '<Iih'
IMU_DATA_FORMAT = '<Ihhhhhh'
ACCEL_MAG_DATA_FORMAT = '<Ihhhhhh'
BATTERY_DATA_FORMAT = '<IH'
PACKET_HEADER_FORMAT = '<BBB'

class DecoderThread(threading.Thread):
	def __init__(self, teleManager, ser_port):
		super(DecoderThread, self).__init__(daemon=True)
		self.ser_port = ser_port
		self.telemetry = teleManager

	def decode_GPS(self,data):
		t, lat, lng = struct.unpack(GPS_DATA_FORMAT, data)
		self.telemetry.logEvents([{
					"id": "gps",
					"timestamp": t,
					"latitude": lat,
					"longitude": lng
				}])

	def decode_Alt(self,data):
		t, alt, temp = struct.unpack(ALTIMETER_DATA_FORMAT, data)
		self.telemetry.logEvents([{
					"id": "altitude",
					"timestamp": t,
					"altitude": alt/16.0
				},{
					"id": "temp",
					"timestamp": t,
					"temp": temp/16.0
				}])

	def decode_IMU(self,data):
		t, accelx, accely, accelz, gyrox, gyroy, gyroz = struct.unpack(IMU_DATA_FORMAT, data)
		accel_to_mpsps = 0.004788
		gyro_to_dps = 0.0175
		self.telemetry.logEvents([
			{"id":"accelx", "timestamp":t, "accelx":accelx*accel_to_mpsps },
			{"id":"accely", "timestamp":t, "accely":accely*accel_to_mpsps },
			{"id":"accelz", "timestamp":t, "accelz":accelz*accel_to_mpsps },
			{"id":"gyrox",  "timestamp":t, "gyrox":gyrox*gyro_to_dps },
			{"id":"gyroy",  "timestamp":t, "gyroy":gyroy*gyro_to_dps },
			{"id":"gyroz",  "timestamp":t, "gyroz":gyroz*gyro_to_dps }
		])

	def decode_AccelMag(self,data):
		t, accelx, accely, accelz, magx, magy, magz = struct.unpack(IMU_DATA_FORMAT, data)
		accel_to_mpsps = 0.0192
		mag_to_utsla = 0.03662
		self.telemetry.logEvents([
			{"id":"accelhgx", "timestamp":t, "accelhgx":accelx*accel_to_mpsps },
			{"id":"accelhgy", "timestamp":t, "accelhgy":accely*accel_to_mpsps },
			{"id":"accelhgz", "timestamp":t, "accelhgz":accelz*accel_to_mpsps },
			{"id":"magx",  "timestamp":t, "magx":magx*mag_to_utsla },
			{"id":"magy",  "timestamp":t, "magy":magy*mag_to_utsla },
			{"id":"magz",  "timestamp":t, "magz":magz*mag_to_utsla }
		])

	def decode_Battery(self, data):
		t, centivolts = struct.unpack(BATTERY_DATA_FORMAT, data)
		self.telemetry.logEvents([{
				"id": "battery",
				"timestamp": t,
				"volts": centivolts/100.0
			}])

	def getSerial(self):
		while 1:
			try:
				stream = serial.Serial(self.ser_port,115200)
				return stream
			except:
				continue



	def run(self):
		packetBuffer = bytearray() # Mutable byte list (efficient append)
		errors = 0
		drops = 0
		next_seq = UInt8(0)

		stream = self.getSerial()

		while 1:
			try:
				newbyte = stream.read(1);
			except:
				print("Serial Read Error")
				stream = self.getSerial()
				continue
			if len(newbyte) == 0:
				continue
			newbyte = newbyte[0] # Indexing to convert length 1 bytes object to single byte
			if newbyte != 0:
				packetBuffer.append(newbyte)
			else:
				try:
					packet_decoded = cobs.decode(packetBuffer) # Clear out buffer after try/catch since this could fail
					p_type, p_len, p_seq = struct.unpack(PACKET_HEADER_FORMAT, packet_decoded[:3])

					# Header inconsistency detection
					if(p_seq != next_seq):
						missed = p_seq-next_seq
						print("Missed {} packets!".format(missed))
						drops += missed

					next_seq = UInt8(p_seq)+1 # Need fixed width to handle wraparound


					if(p_len != len(packet_decoded)):
						raise Exception('Inconsistent Packet Length')

					crcCalc = crc8.crc8()
					crcCalc.update(packet_decoded) # Compute CRC, total CRC should be zero if no error
					crcVal = crcCalc.digest()[0]
					if crcVal != 0:
						raise Exception('Nonzero CRC ({:02X}) {}!'.format(crcVal, packet_decoded.hex()))


					data_bytes = packet_decoded[3:-1]
					if(p_type == 1):
						self.decode_GPS(data_bytes)
					elif(p_type == 2):
						self.decode_Alt(data_bytes)
					elif(p_type == 3):
						self.decode_IMU(data_bytes)
					elif(p_type == 4):
						self.decode_Battery(data_bytes)
					elif(p_type == 5):
						self.decode_AccelMag(data_bytes)
					else:
						print("Unrecognized packet type {}!".format(p_type))

				except Exception as e:
					print("Error: {} --- {}".format(e,packetBuffer.hex()))
					errors += 1

				del packetBuffer[:] # Clear out buffer

if __name__ == '__main__':

	class Dummy:
		def logEvents(self,arr):
			print("Recieved {} events".format(len(arr)))

	decoder = DecoderThread(Dummy(), 'COM7')
	decoder.start()
	while 1:
		time.sleep(0.1)
