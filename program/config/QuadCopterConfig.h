#ifndef __QUADCOPTER_CONFIG_H
#define __QUADCOPTER_CONFIG_H

/*Radio control*/
#define USE_FUTABA_CONTROLLER		0
#define USE_WFLY_CONTROLLER		1
#define USE_DEVO_v10_CONTROLLER 0
/* Hardware environments */
#define configRemoteControl				0       //if you no connect motor -> set 0. 
#define configSD				0       //if you no connect SD card -> set 0.
#define configNRF				0       //if you no connect nRF module -> set 0.
/* Status report functions */
#define configSTATUS_NRF		0
#define configSTATUS_SHELL		1
#define configSTATUS_GET_MOTORS	0
#define configSTATUS_GET_ROLL_PITCH	0
#define configSTATUS_GET_ROLL_PITCH_YAW	0
#define configSTATUS_GET_ROLL_PITCH_MOTORS 0
/* gui3 */
#define configSTATUS_GET_MAG 0
#define configSTATUS_GET_ACC 0
#define configSTATUS_GET_GYRO 0

/*motot PWM parameter adjustment*/
#define adjustmentMotor1 0	//100
#define adjustmentMotor2 0	//100
#define adjustmentMotor3 0	//105
#define adjustmentMotor4 0	// -55

/* Set baudrate */
#define Serial_Baudrate 9600

/* Includes */
//FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

//FatFS
#include "diskio.h"
#include "ff.h"

//Algorithm
#include "PID.h"
#include "moving_average.h"
#include "quaternion.h"
#include "_math.h"

//System
#include "global.h"
#include "std.h"
#include "AHRS.h"
#include "remote_control.h"
#include "IMU_correct.h"
#include "communication.h"

//Driver
#include "stm32f4_system.h"
#include "stm32f4xx_conf.h"
#include "delay.h"
#include "flash.h"
#include "i2c.h"
#include "spi.h"
#include "sdio.h"
#include "led.h"
#include "motor.h"
#include "mpu9150.h"
#include "ms5611.h"
#include "nrf24l01.h"
#include "serial.h"
#include "sensor.h"

//Shell
#include "linenoise.h"
#include "shell.h"
#include "parser.h"
#include "watch.h"

//Debugger
#include "test.h"

//LCD
#include "tm_stm32f4_ili9341.h"
#include "tm_stm32f4_fonts.h"
#include "tm_stm32f4_spi.h"

#endif
