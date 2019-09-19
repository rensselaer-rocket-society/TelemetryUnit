import csv
import numpy as np
import os

def open_log(filename):
	f = csv.reader(open(filename))

	# Accumulation is faster in python lists
	imu = []
	alt = []
	bat = []
	gps = []

	for rowarr in ([float(x) for x in row if x!=""] for row in f):
		src = rowarr[0]
		if src==1:
			targetArr = gps
		elif src==2:
			targetArr = alt
		elif src==3:
			targetArr = imu
		elif src==4:
			targetArr = bat
		else:
			targetArr = None

		if targetArr is not None:
			targetArr.append(rowarr[1:])

	return _LogData([gps, alt, imu, bat])

def split_log(filename, suffix='_'):
	fin = open(filename)
	base,ext = os.path.splitext(filename)
	runID = 0
	lastT = float('inf')
	for line in fin:
		t = float(line.split(',',2)[1])
		if t < lastT:
			runID += 1
			outFile = open(base+suffix+str(runID)+ext,'w')
		outFile.write(line)
		lastT = t

class _LogData:
	def __init__(self, data):
		self.data = [np.copy(series) for series in data]

	def getIMU(self):
		return self.data[2]
	def getGPS(self):
		return self.data[0]
	def getBattery(self):
		return self.data[3]
	def getAltimeter(self):
		return self.data[1]
	def getTimeRange(self):
		allTimes = flatMap(lambda series: series[:,0], self.data)
		return np.amin(allTimes), np.amax(allTimes) 

	def extractRange(self, startTime, endTime):
		outputData = []
		for series in self.data:
			range_chunk = [row for row in series if (startTime <= row[0] <= endTime)]
			outputData.append(range_chunk)
		return _LogData(outputData)
