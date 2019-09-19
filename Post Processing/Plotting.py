import matplotlib.pyplot as plt
from Processing.LogParser import open_log

allDat = open_log('LaunchData.csv')
alt = allDat.getAltimeter()
imu = allDat.getIMU()

plt.plot(imu[:,0],imu[:,1])
plt.show()