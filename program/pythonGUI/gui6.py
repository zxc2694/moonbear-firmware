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

		self.m1 = deque([0.0]*maxLen)
		self.m2 = deque([0.0]*maxLen)
		self.m3 = deque([0.0]*maxLen)
		self.m4 = deque([0.0]*maxLen)		
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
		assert(len(data) == 6)
		self.addToBuf(self.ax, data[0])
		self.addToBuf(self.ay, data[1])

		self.addToBuf(self.m1, data[2])
		self.addToBuf(self.m2, data[3])
		self.addToBuf(self.m3, data[4])
		self.addToBuf(self.m4, data[5])
#		self.addToBuf(self.ay, data[1])
# plot class
class AnalogPlot:
# constr
	def __init__(self, analogData):
# set plot to animated
		plt.ion()
		plt.figure(figsize=(9,8))
		plt.subplot(211)
		self.axline, = plt.plot(analogData.ax,label="Roll",color="red")
		self.ayline, = plt.plot(analogData.ay,label="Pitch",color="blue")
		plt.xlabel("Time")
		plt.ylabel("Angle(-90~+90)")
		plt.title("Quadcopter attitude")
		plt.legend()		#Show label figure.
		plt.ylim([-90, 90]) # Vertical axis scale.
		plt.grid()


		plt.subplot(212)
		self.m1line, = plt.plot(analogData.m1,label="motor4 -PWM9",color="red")
		self.m2line, = plt.plot(analogData.m2,label="motor3 -PWM10",color="orange")
		self.m3line, = plt.plot(analogData.m3,label="motor2 -PWM11",color="green")
		self.m4line, = plt.plot(analogData.m4,label="motor1 -PWM12",color="blue")

		plt.xlabel("Time")
		plt.ylabel("PWM")
		plt.legend()		
		plt.ylim([750, 1300]) 
		plt.grid()
 
# update plot
	def update(self, analogData):
		self.axline.set_ydata(analogData.ax)
		self.ayline.set_ydata(analogData.ay)
		plt.draw()

		self.m1line.set_ydata(analogData.m1)
		self.m2line.set_ydata(analogData.m2)
		self.m3line.set_ydata(analogData.m3)
		self.m4line.set_ydata(analogData.m4)
		#plt.draw()
 
def main():
# expects 1 arg - serial port string
	if(len(sys.argv) != 2):
		print "Type:" 
		print "sudo chmod 777 /dev/ttyUSB0"
		print "python gui6.py '/dev/ttyUSB0'"
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
				print data[0] , data[1] , data[2] , data[3] , data[4] , data[5]
				if(len(data) == 6):
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
