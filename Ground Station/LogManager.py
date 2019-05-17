from bisect import bisect_left, bisect_right
from threading import Lock

class Event:
	def __init__(self, time, obj):
		self.time = time
		self.data = obj
	@classmethod
	def fromJSON(cls, obj):
		return Event(obj["timestamp"], obj)
	#Comparisons to allow bisect to do search by timestamp
	def __lt__(self, e2):
		return self.time < e2.time;
	def __le__(self, e2):
		return self.time <= e2.time;
	def __eq__(self, e2):
		return self.time == e2.time;
	def __ne__(self, e2):
		return self.time != e2.time;
	def __ge__(self, e2):
		return self.time >= e2.time;
	def __gt__(self, e2):
		return self.time > e2.time;


def extractRange(l, t1, t2):
	minIndex = bisect_left(l, Event(t1,None))
	maxIndex = bisect_right(l, Event(t2,None))
	return l[minIndex:maxIndex]

class TelemetryManager:

	def __init__(self, socket):
		self.eventlogs = {}
		self.socket = socket
		self.datalock = Lock()
		self.fileiolock = Lock()

	def _logEvent(self, e):
		eventid = e["id"];
		if eventid not in self.eventlogs:
			self.eventlogs[eventid] = []
		self.datalock.acquire() # Needed to ensure that a concurrent flush to disk does not run into reallocation problems
		self.eventlogs[eventid].append(Event.fromJSON(e))
		self.datalock.release()

	def logEvents(self, events):
		self.socket.emit("telemetry",events)
		for e in events:
			self._logEvent(e)

	def serviceRequest(self, eventtype, mint, maxt):
		if eventtype in self.eventlogs:
			return extractRange(self.eventlogs[eventtype], mint, maxt)
		return []

	def saveToDisk(self,filename):
		self.fileiolock.acquire()
		f = open(filename, "w");
		for i in range(len(self.eventlogs.get("gps",[]))):
			self.datalock.acquire() # Reacquire each iteration to ensure realtime data can still come through
			vals = self.eventlogs["gps"][i]
			self.datalock.release()
			f.write('1,' + str(vals.time) + ',' + str(vals.data['latitude']) + ',' + str(vals.data['longitude']) + '\n');
		for i in range(len(self.eventlogs.get("altitude",[]))):
			self.datalock.acquire()
			vals = self.eventlogs["altitude"][i]
			self.datalock.release()
			f.write('2,' + str(vals.time) + ',' + str(vals.data["altitude"]) + '\n');
		for i in range(len(self.eventlogs.get("accelx",[]))):
			self.datalock.acquire() # Only compute string under lock
			outstr = '3,'+str(self.eventlogs["accelx"][i].time)+','+ \
				str(self.eventlogs["accelx"][i].data["accelx"]) + ',' + \
				str(self.eventlogs["accely"][i].data["accely"]) + ',' + \
				str(self.eventlogs["accelz"][i].data["accelz"]) + ',' + \
				str(self.eventlogs["gyrox"][i].data["gyrox"]) + ',' + \
				str(self.eventlogs["gyroy"][i].data["gyroy"]) + ',' + \
				str(self.eventlogs["gyroz"][i].data["gyroz"]) + '\n'
			self.datalock.release()
			f.write(outstr)
		self.fileiolock.release()

	def restoreFromDisk(self,filename):
		self.fileiolock.acquire()
		f = open(filename, "r");
		for event in f:
			vals = event.split(',')
			type = vals[0]
			t = int(vals[1])
			if type == '1': # GPS
				self._logEvent({
					"id": "gps",
					"timestamp": t,
					"latitude": float(vals[2]),
					"longitude": float(vals[3])
				})
			elif type == '2': # Altimeter
				self._logEvent({
					"id": "altitude",
					"timestamp": t,
					"altitude": float(vals[2])
				})
			elif type == '3': # Accelerometer (can't use logEvents as that would broadcast)
				self._logEvent({"id":"accelx", "timestamp":t, "accelx":float(vals[2])})
				self._logEvent({"id":"accely", "timestamp":t, "accely":float(vals[3])})
				self._logEvent({"id":"accelz", "timestamp":t, "accelz":float(vals[4])})
				self._logEvent({"id":"gyrox",  "timestamp":t, "gyrox": float(vals[5])})
				self._logEvent({"id":"gyroy",  "timestamp":t, "gyroy": float(vals[6])})
				self._logEvent({"id":"gyroz",  "timestamp":t, "gyroz": float(vals[7])})
		self.fileiolock.release()