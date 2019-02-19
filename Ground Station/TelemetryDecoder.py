import threading
import time

class DecoderThread(threading.Thread):
	def __init__(self, socket):
		super(DecoderThread, self).__init__()
		self.socket = socket

	def run(self):
		while True:
			self.socket.emit("telemetry",{"name":"GPS", "value":10})
			time.sleep(1)