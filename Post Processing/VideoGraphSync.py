import cv2
import sys
import os
import subprocess
import numpy as np
from Processing.LogParser import open_log
from Processing.CVDrawingUtils import CVGraph

MI_TO_M = 1609.344
G_TO_MSS = 9.81

GW = 500
GH = 300
GB = 50
COLOR_CYAN = (255,255,0)
COLOR_GREEN = (0,255,0)
COLOR_BLACK = (0,)*3
COLOR_GRAY = (64,)*3
COLOR_YELLOW = (0,255,255)
COLOR_BLUE = (255,0,0)
GRAPH_STYLE = {
	'axisStyle': {
		'color': COLOR_CYAN,
		'thickness': 3
	},
	'gridStyle': {
		'color': COLOR_GRAY,
		'thickness': 1
	},
	'textColor':COLOR_CYAN
}
SERIES_STYLE = {
	'thickness': 2
}

tempfile = '__temp__.mp4'

BOX_H = 2*GB+GH
BOX_W = 2*GB+GW

print("Reading Video File...")	

vid_in = cv2.VideoCapture(sys.argv[1])

w = int(vid_in.get(cv2.CAP_PROP_FRAME_WIDTH))
h = int(vid_in.get(cv2.CAP_PROP_FRAME_HEIGHT))
size = (w,h)
framerate = vid_in.get(cv2.CAP_PROP_FPS);


fourcc = cv2.VideoWriter_fourcc(*'mp4v')
vid_out = cv2.VideoWriter(tempfile, fourcc, framerate, size)


vidStart = 1026.54e3-53.2e3
vidEnd = vidStart + 88e3

print("Reading Data File...")

data = open_log(sys.argv[2]).extractRange(vidStart,vidEnd)

print("Preparing Graphs...")

altGraph = CVGraph((GB,h-GB-GH,GW,GH),vidEnd-vidStart,np.arange(0,3.1,0.5),**GRAPH_STYLE)
altDat = data.getAltimeter()
altGraph.addSeries(altDat[:,0],(altDat[:,1]-230)/MI_TO_M, COLOR_GREEN,**SERIES_STYLE)

accelGraph = CVGraph((w-GB-GW,h-GB-GH,GW,GH),vidEnd-vidStart,np.arange(-16,17,4),**GRAPH_STYLE)
imuDat = data.getIMU()
accelGraph.addSeries(imuDat[:,0],imuDat[:,1]/G_TO_MSS,COLOR_GREEN,**SERIES_STYLE)
accelGraph.addSeries(imuDat[:,0],imuDat[:,2]/G_TO_MSS,COLOR_YELLOW,**SERIES_STYLE)
accelGraph.addSeries(imuDat[:,0],imuDat[:,3]/G_TO_MSS,COLOR_BLUE,**SERIES_STYLE)

altGraphStatic = np.full((BOX_H,BOX_W,3),64)
cv2.rectangle(altGraphStatic,(0,4),(BOX_W-4,BOX_H),COLOR_BLACK,-1)
altGraph.drawStatic(altGraphStatic,GB,GB)

accelGraphStatic = np.full((BOX_H,BOX_W,3),64)
cv2.rectangle(accelGraphStatic,(4,4),(BOX_W,BOX_H),COLOR_BLACK,-1)
accelGraph.drawStatic(accelGraphStatic,GB,GB)

print("Processing Frames...")

time = 0
good, frame = vid_in.read()
lastProg = -1
while(good):

	frame[-BOX_H:,:BOX_W] = altGraphStatic
	frame[-BOX_H:,-BOX_W:] = accelGraphStatic
	altGraph.drawData(frame,vidStart,time)
	accelGraph.drawData(frame,vidStart,time)

	vid_out.write(frame)
	good, frame = vid_in.read()

	time += 1000/framerate

	progress = int(50*time/(vidEnd-vidStart))
	if progress != lastProg:
		print('='*progress + ('>' if progress<50 else '') + ' '*max(0,49-progress) + '|', end='\r')
	lastProg = progress

print()

vid_out.release()

print("Restoring Audio...")

subprocess.check_call(['ffmpeg','-i', tempfile, '-i', 'LiftoffAudio.aac', '-c', 'copy', sys.argv[3]])

os.unlink(tempfile)

print("Done!")