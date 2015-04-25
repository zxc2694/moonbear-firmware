################################################################################
# File name: gui3.py
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
		self.v1 = deque([0.0]*maxLen)
		self.v2 = deque([0.0]*maxLen)	
		self.v3 = deque([0.0]*maxLen)
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
		assert(len(data) == 3)
		self.addToBuf(self.v1, data[0])
		self.addToBuf(self.v2, data[1])
		self.addToBuf(self.v3, data[2])
# plot class
class AnalogPlot:
# constr
	def __init__(self, analogData):
# set plot to animated
		plt.ion()
		plt.figure(figsize=(9,8))


		self.v1line, = plt.plot(analogData.v1,label="Magnetometer_X",color="red")
		self.v2line, = plt.plot(analogData.v2,label="Magnetometer_Y",color="orange")
		self.v3line, = plt.plot(analogData.v3,label="Magnetometer_Z",color="green")


		plt.xlabel("Time")
		plt.ylabel("PWM range")
		plt.title("Measure Magnetometer")
		plt.legend()		#Show label figure.
		plt.ylim([-300, 300]) # Vertical axis scale.
#TEST	plt.ylim([-90, 90]) # Vertical axis scale.
		plt.grid()

 
# update plot
	def update(self, analogData):
		self.v1line.set_ydata(analogData.v1)
		self.v2line.set_ydata(analogData.v2)
		self.v3line.set_ydata(analogData.v3)
		plt.draw()
 
def main():
# expects 1 arg - serial port string
	if(len(sys.argv) != 2):
		print "Type:" 
		print "sudo chmod 777 /dev/ttyUSB0"
		print "python gui3_magnetometer.py '/dev/ttyUSB0'"
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
				print data[0] , data[1] ,data[2] 
				if(len(data) == 3):
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
