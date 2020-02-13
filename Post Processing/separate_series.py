from Processing.LogParser import open_log
import csv
import sys
import os

filename = sys.argv[1]
basePath = os.path.splitext(filename)[0]

allDat = open_log(filename)

writer = csv.writer(open(basePath+'_IMU.csv','w', newline=''))
writer.writerow(["Time (ms)", "Accel X (m/s2)", "Accel Y (m/s2)", "Accel Z (m/s2)", "Gyro X (°/s)", "Gyro Y (°/s)", "Gyro Z (°/s)"])
for line in allDat.getIMU():
	#print(line)
	writer.writerow(line)

writer = csv.writer(open(basePath+'_Altimeter.csv','w', newline=''))
writer.writerow(["Time (ms)", "Altitude (m)", "Temperature (°C)"])
for line in allDat.getAltimeter():
	writer.writerow(line)

writer = csv.writer(open(basePath+'_Battery.csv','w', newline=''))
writer.writerow(["Time (ms)", "Voltage (V)"])
for line in allDat.getBattery():
	writer.writerow(line)

writer = csv.writer(open(basePath+'_GPS.csv','w', newline=''))
writer.writerow(["Time (ms)", "Latitude (°)", "Longitude (°)"])
for line in allDat.getGPS():
	writer.writerow(line)