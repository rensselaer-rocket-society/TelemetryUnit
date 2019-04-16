from bisect import bisect_left, bisect_right

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

	def _logEvent(self, e):
		eventid = e["id"];
		if eventid not in self.eventlogs:
			self.eventlogs[eventid] = []
		self.eventlogs[eventid].append(Event.fromJSON(e))

	def logEvents(self, events):
		self.socket.emit("telemetry",events)
		for e in events:
			self._logEvent(e)

	def serviceRequest(self, eventtype, mint, maxt):
		if eventtype in self.eventlogs:
			return extractRange(self.eventlogs[eventtype], mint, maxt)
		return []

	def saveToDisk(filename):
		f = open(filename, "w");
		for vals in eventlogs["gps"]:
			f.write('1,' + str(vals.time) + ',' + str(vals.data['latitude']) + ',' + str(vals.data['longitude']) + '\n');
		for vals in eventlogs["altitude"]:
			f.write('2,' + str(vals.time) + ',' + str(vals.data["altitude"]) + '\n');
		for i in range(len(eventlogs["accelx"])):
			f.write('3,'+str(eventlogs["accelx"][i].time)+','+
				str(eventlogs["accelx"][i].data["accelx"]) + ',' +
				str(eventlogs["accely"][i].data["accely"]) + ',' +
				str(eventlogs["accelz"][i].data["accelz"]) + ',' +
				str(eventlogs["gyrox"][i].data["gyrox"]) + ',' +
				str(eventlogs["gyroy"][i].data["gyroy"]) + ',' +
				str(eventlogs["gyroz"][i].data["gyroz"]) + '\n'
			)

	def restoreFromDisk(filename):
		f = open(filename, "w");
		for event in f:
			vals = event.split(',')
			type = vals[0]
			t = int(vals[1])
			if type == '1': # GPS
				eventlogs["gps"].append(Event.fromJSON({
					"id": "gps",
					"timestamp": t,
					"latitude": float(vals[2]),
					"longitude": float(vals[3])
				}))
			elif type == '2': # Altimeter
				eventlogs['altitude'].append(Event.fromJSON({
					"id": "altitude",
					"timestamp": t,
					"altitude": float(vals[2])
				}))
			elif type == '3': # Accelerometer
				eventlogs['accelx'].append(Event.fromJSON({"id":"accelx", "timestamp":t, "accelx":float(vals[2])}))
				eventlogs['accely'].append(Event.fromJSON({"id":"accely", "timestamp":t, "accely":float(vals[3])}))
				eventlogs['accelz'].append(Event.fromJSON({"id":"accelz", "timestamp":t, "accelz":float(vals[4])}))
				eventlogs['gyrox'].append(Event.fromJSON({"id":"gyrox",  "timestamp":t, "gyrox": float(vals[5])}))
				eventlogs['gyroy'].append(Event.fromJSON({"id":"gyroy",  "timestamp":t, "gyroy": float(vals[6])}))
				eventlogs['gyroz'].append(Event.fromJSON({"id":"gyroz",  "timestamp":t, "gyroz": float(vals[7])}))