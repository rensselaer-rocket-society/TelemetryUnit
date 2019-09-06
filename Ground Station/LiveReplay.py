import threading
import time

EVENT_TYPES = ["accelx","accely","accelz","gyrox","gyroy","gyroz","gps","altitude","temp","battery"]

def timeMs():
	return int(time.time()*1000)

class ReplayThread(threading.Thread):
	def __init__(self, teleManager, looped):
		super(ReplayThread, self).__init__(daemon=True)
		self.looped = looped;
		self.telemetry = teleManager

	def newMessages(self,mint,maxt):
		events = []
		for val in EVENT_TYPES:
			events += self.telemetry.serviceRequest(val,mint,maxt)
		return events

	def doReplay(self, startTime):
		lastTime = -float("inf")
		endTime = self.telemetry.maxTime()
		while True:
			time.sleep(0.01) # Run every ~10 ms
			newTime = timeMs()-startTime
			self.telemetry.updateHistoryMask(None,newTime);
			newData = self.newMessages(lastTime+1,startTime)
			# print("{:d} new data points for time {:d}".format(len(newData),newTime))
			self.telemetry.socket.emit("telemetry",[e.data for e in newData])
			lastTime = newTime
			if newTime > endTime:
				break

	def run(self):
		self.telemetry.updateHistoryMask(None,-float('inf'));
		while True:
			time.sleep(5)
			self.doReplay(timeMs())
			if not self.looped:
				break