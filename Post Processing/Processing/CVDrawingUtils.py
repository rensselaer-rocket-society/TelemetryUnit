import cv2
import numpy as np

def boxLine(frame,dir,fix,start,end,color,thickness=1,**rect_args):
	side1 = (thickness-1)//2
	side2 = thickness-1-side1
	if dir=='horiz':
		cv2.rectangle(frame,(start,fix-side1),(end,fix+side2),color,-1,**rect_args)
	elif dir=='vert':
		cv2.rectangle(frame,(fix-side1,start),(fix+side2,end),color,-1,**rect_args)

CV_WHITE = (255,)*3
CV_LIGHT_GRAY = (64,)*3

class _CVGraphSeries:
	def __init__(self,time,data,color,thickness,**style):
		self.time = time;
		self.data = data;
		self.style = style;
		self.style['thickness'] = thickness;
		self.style['color'] = color;

	def plot(self,frame,graph,startt,length):
		pixelEnd = min(graph.width,int(length*graph.tscale)+1)
		lastrow = None
		for col in range(pixelEnd):
			t = col/graph.tscale + startt
			if col == pixelEnd:
				daty = self.data[np.searchsorted(self.time,t,'right')] # Use latest value (less jumpy for long timescales)
			else:
				daty = np.interp(t,self.time,self.data)
			row = int((graph.ymax-daty)*graph.yscale)

			if col > 0: # Need at least 2 points to start drawing line
				cv2.line(frame,
					(graph.origin[0]+col-1,graph.origin[1]+lastrow),
					(graph.origin[0]+col,graph.origin[1]+row),
					lineType=cv2.LINE_AA,**self.style)

			lastrow = row



class CVGraph:
	def __init__(self,bounds,duration,ygrid,axisStyle={'color':CV_WHITE,'thickness':3},gridStyle={'color':CV_LIGHT_GRAY,'thickness':1},textColor=CV_WHITE):
		self.width = bounds[2]
		self.height = bounds[3]
		self.origin = (bounds[0],bounds[1])
		self.tscale = self.width/duration # ms to px

		self.textColor = textColor
		self.gridStyle = gridStyle
		self.axisStyle = axisStyle

		ygrid.sort()
		self.ygrid = ygrid
		self.ymin = min(ygrid[0],0)
		self.ymax = max(ygrid[-1],0)
		self.yscale = self.height/(self.ymax-self.ymin) # units to px

		self.series = []

	def addSeries(self,time,data,color,thickness=2,**style):
		self.series.append(_CVGraphSeries(time,data,color,thickness,**style))

	def drawStatic(self,staticbuffer,offX,offY):
		for gridline in self.ygrid:
			pixelH = int((self.ymax-gridline)*self.yscale);
			cv2.putText(staticbuffer,str(gridline),(offX-25,offY+pixelH+5),cv2.FONT_HERSHEY_SIMPLEX,0.33,self.textColor,1,cv2.LINE_AA)
			cv2.line(staticbuffer,(offX,offY+pixelH),(offX+self.width,offY+pixelH),**self.gridStyle)

		boxLine(staticbuffer,'vert',offX,offY,offY+self.height,**self.axisStyle)
		boxLine(staticbuffer,'horiz',offY+int(self.ymax*self.yscale),offX,offX+self.width,**self.axisStyle)

	def drawData(self,frame,startt,endt):
		for series in self.series:
			series.plot(frame,self,startt,endt)

