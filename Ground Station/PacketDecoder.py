import threading
import time

import random

class DecoderThread(threading.Thread):
	def __init__(self, socket):
		super(DecoderThread, self).__init__(daemon=True)
		self.socket = socket

	def run(self):
		i=0
		while True:
			if i%1000 == 0:
				self.socket.emit("telemetry",[
						{"id":"gps", "timestamp":i, "latitude":40.0+random.uniform(-5,5), "longitude":-75.0+random.uniform(-5,5)}
					])
			self.socket.emit("telemetry",[
					{"id":"accelx", "timestamp":i, "accelx":random.uniform(-1,1)},
					{"id":"accely", "timestamp":i, "accely":random.uniform(-1,1)},
					{"id":"accelz", "timestamp":i, "accelz":random.uniform(-1,1)},
					{"id":"gyrox", "timestamp":i, "gyrox":random.uniform(-90,90)},
					{"id":"gyroy", "timestamp":i, "gyroy":random.uniform(-90,90)},
					{"id":"gyroz", "timestamp":i, "gyroz":random.uniform(-90,90)},
					{"id":"altitude", "timestamp":i, "altitude":max(i - i**2/100000,0)+random.uniform(-10,10)}
				])
			i+=100
			time.sleep(0.1)