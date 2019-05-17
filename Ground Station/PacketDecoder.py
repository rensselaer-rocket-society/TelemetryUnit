import threading
import time

import random
from cobs import cobs
import crc8
import struct
from fixedint import UInt8

# Correspondance by packet type ID [None, GPS, Alt, Accel]
GPS_DATA_FORMAT = '<ff'
ALTIMETER_DATA_FORMAT = '<ff'
ACCEL_DATA_FORMAT = '<ffffff'
PACKET_HEADER_FORMAT = '<BBB'

class DecoderThread(threading.Thread):
	def __init__(self, teleManager):
		super(DecoderThread, self).__init__(daemon=True)
		self.telemetry = teleManager

	def decode_GPS(self,t,data):
		lat, lng = struct.unpack(GPS_DATA_FORMAT, data)
		self.telemetry.logEvents([{
					"id": "gps",
					"timestamp": t,
					"latitude": lat,
					"longitude": lng
				}])

	def decode_Alt(self,t,data):
		alt, temp = struct.unpack(ALTIMETER_DATA_FORMAT, data)
		self.telemetry.logEvents([{
					"id": "altitude",
					"timestamp": t,
					"altitude": alt
				}])

	def decode_Accel(self,t,data):
		accelx, accely, accelz, gyrox, gyroy, gyroz = struct.unpack(ACCEL_DATA_FORMAT, data)
		self.telemetry.logEvents([
			{"id":"accelx", "timestamp":t, "accelx":accelx },
			{"id":"accely", "timestamp":t, "accely":accely },
			{"id":"accelz", "timestamp":t, "accelz":accelz },
			{"id":"gyrox",  "timestamp":t, "gyrox":gyrox },
			{"id":"gyroy",  "timestamp":t, "gyroy":gyroy },
			{"id":"gyroz",  "timestamp":t, "gyroz":gyroz }
		])

	def run(self):
		packetBuffer = b''
		errors = 0
		drops = 0
		next_seq = UInt8(0)
		crcCalc = crc8.crc8()

		stream = open('test.log','rb') # Replace with serial stream

		t0 = time.perf_counter()

		while 1:
			time.sleep(9/9600) # Simulate 9600 baud at max rate
			newbyte = stream.read(1);
			if newbyte != b'\x00':
				packetBuffer += newbyte
			else:
				t = (time.perf_counter()-t0)*1000 # Capture arrival time (ms)
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
						print("LENGTH ERROR")
						raise Exception('Length Error')

					crc_sent = packet_decoded[-1:] # Extract as 1 element array to match digest format
					crcCalc.update(packet_decoded[:-1]) 
					if crcCalc.digest() != crc_sent:
						print("CRC ERROR")
						raise Exception('CRC Error')


					data_bytes = packet_decoded[3:-1]
					if(p_type == 1):
						self.decode_GPS(t,data_bytes)
					elif(p_type == 2):
						self.decode_Alt(t,data_bytes)
					elif(p_type == 3):
						self.decode_Accel(t,data_bytes)
					else:
						print("Unrecognized packet type {}!".format(p_type))

				except Exception as e:
					print("Error : {}\n{}".format(packetBuffer,e))
					errors += 1

				packetBuffer = b'' # Prep for next packet





		# i=0
		# while True:
		# 	if i%1000 == 0:
		# 		self.telemetry.logEvents([
		# 				{"id":"gps", "timestamp":i, "latitude":40.0+random.uniform(-5,5), "longitude":-75.0+random.uniform(-5,5)}
		# 			])
		# 	self.telemetry.logEvents([
		# 			{"id":"accelx", "timestamp":i, "accelx":random.uniform(-1,1)},
		# 			{"id":"accely", "timestamp":i, "accely":random.uniform(-1,1)},
		# 			{"id":"accelz", "timestamp":i, "accelz":random.uniform(-1,1)},
		# 			{"id":"gyrox", "timestamp":i, "gyrox":random.uniform(-90,90)},
		# 			{"id":"gyroy", "timestamp":i, "gyroy":random.uniform(-90,90)},
		# 			{"id":"gyroz", "timestamp":i, "gyroz":random.uniform(-90,90)},
		# 			{"id":"altitude", "timestamp":i, "altitude":max(i - i**2/100000,0)+random.uniform(-10,10)}
		# 		])
		# 	i+=100
		# 	time.sleep(0.1)


if __name__ == '__main__':

	class Dummy:
		def logEvents(self,arr):
			print("Recieved {} events".format(len(arr)))

	decoder = DecoderThread(Dummy())
	decoder.start()
	while 1:
		time.sleep(0.1)
