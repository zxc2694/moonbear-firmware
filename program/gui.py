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
#		self.az = deque([0.0]*maxLen)
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
#		self.addToBuf(self.az, data[2])
# plot class
class AnalogPlot:
# constr
	def __init__(self, analogData):
# set plot to animated
		plt.ion()
		self.axline, = plt.plot(analogData.ax)
		self.ayline, = plt.plot(analogData.ay)
	#	self.azline, = plt.plot(analogData.az)
		plt.ylim([-90, 90]) # Vertical axis scale.
 
# update plot
	def update(self, analogData):
		self.axline.set_ydata(analogData.ax)
		self.ayline.set_ydata(analogData.ay)
	#	self.azline.set_ydata(analogData.az)
		plt.draw()
 
def main():
# expects 1 arg - serial port string
	if(len(sys.argv) != 2):
		print "Type:" 
		print "sudo chmod 777 /dev/ttyUSB0"
		print "python gui.py '/dev/ttyUSB0'"
		exit(1)
 
#strPort = '/dev/tty.usbserial-A7006Yqh'
	strPort = sys.argv[1];

# plot parameters
	analogData = AnalogData(200)  # Horizontal axis scale.
	analogPlot = AnalogPlot(analogData)

	print "plotting data..."

# open serial port
	ser = serial.Serial(strPort, 9600)
	while True:
		try:
			line = ser.readline()
			data = [float(val) for val in line.split()]
			print data[0] , data[1]# , data[2] #Show three data on the terminal
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
