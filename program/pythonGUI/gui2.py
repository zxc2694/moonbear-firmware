################################################################################
# File name: gui.py
#
# Function: Display three data from stm32f4 using Python (matplotlib)
#	    The three data is roll, pith, yall angle of quadcopter attitude.
#
# Reference:http://electronut.in/plotting-real-time-data-from-arduino-using-python/
#
################################################################################

import sys, serial
import numpy as np
from time import sleep
from collections import deque
from matplotlib import pyplot as plt
 
# class that holds analog data for N samples
class AnalogData:
# constr
	def __init__(self, maxLen):
		self.ax = deque([0.0]*maxLen)
		self.ay = deque([0.0]*maxLen)	
		self.maxLen = maxLen
 
# ring buffer
	def addToBuf(self, buf, val):
		if len(buf) < self.maxLen:
			buf.append(val)
		else:
			buf.pop()
		buf.appendleft(val)
 
#Add new data
	def add(self, data):      
		assert(len(data) == 2)
		self.addToBuf(self.ax, data[0])
		self.addToBuf(self.ay, data[1])
# plot class
class AnalogPlot:
# constr
	def __init__(self, analogData):
# set plot to animated
		plt.ion()
		plt.figure(figsize=(9,8))
		self.axline, = plt.plot(analogData.ax,label="Roll",color="red")
		self.ayline, = plt.plot(analogData.ay,label="Pitch",color="blue")
		plt.xlabel("Time")
		plt.ylabel("Angle(-90~+90)")
		plt.title("Quadcopter attitude")
		plt.legend()		#Show label figure.
		plt.ylim([-90, 90]) # Vertical axis scale.
		plt.grid()

 
# update plot
	def update(self, analogData):
		self.axline.set_ydata(analogData.ax)
		self.ayline.set_ydata(analogData.ay)
		plt.draw()
 
def main():
# expects 1 arg - serial port string
	if(len(sys.argv) != 2):
		print "Type:" 
		print "sudo chmod 777 /dev/ttyUSB0"
		print "python gui2.py '/dev/ttyUSB0'"
		exit(1)
 
#strPort = '/dev/tty.usbserial-A7006Yqh'
	strPort = sys.argv[1];

# plot parameters
	analogData = AnalogData(200)  # Horizontal axis scale.
	analogPlot = AnalogPlot(analogData)

	print "plotting data..."
	a = 1
# open serial port
	ser = serial.Serial(strPort, 9600)
	while True:
		try:
			line = ser.readline()
			data = [float(val) for val in line.split()]
			if (a < 10):
				a = a + 1
			else:
				print data[0] , data[1] 
				if(len(data) == 2):
					analogData.add(data)
					analogPlot.update(analogData)
		except KeyboardInterrupt:
			print "exiting"
			break
	# close serial
	ser.flush()
	ser.close()
	 
# call main
if __name__ == '__main__':
	main()
