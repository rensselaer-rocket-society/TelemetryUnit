import threading
import time

import random

class DecoderThread(threading.Thread):
	def __init__(self, socket):
		super(DecoderThread, self).__init__()
		self.socket = socket

	def run(self):
		i=0
		while True:
			self.socket.emit("telemetry",[
					{"id":"gpslat", "timestamp":i, "latitude":40.0+random.uniform(-5,5)},
					{"id":"gpslng", "timestamp":i, "longitude":-75.0+random.uniform(-5,5)}
				])
			i+=100
			time.sleep(0.1)