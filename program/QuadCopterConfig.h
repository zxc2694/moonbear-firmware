#ifndef __QUADCOPTER_CONFIG_H
#define __QUADCOPTER_CONFIG_H
/*RADIO CONTROL*/
#define USE_FUTABA_CONTROLLER 0
#define USE_WFLY_CONTROLLER 1
/* Hardware environments */
#define configMotor				1       //if you no connect motor -> set 0. 
/* Status report functions */
#define configSTATUS_SHELL		0
#define configSTATUS_GET_MOTORS	0
#define configSTATUS_GET_ROLL_PITCH	0
#define configSTATUS_GET_ROLL_PITCH_MOTORS 1

/* Quadrotor-Motorcycle mode */
#define QuadrotorMotorcycle 1

/* Includes */
//FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

//System
#include "sys_manager.h"
#include "std.h"
#include "ahrs.h"
#include "rc_control.h"
#include "sensor_correct.h"
#include "communication.h"

//Algorithm
#include "algorithm_pid.h"
#include "algorithm_moveAve.h"
#include "algorithm_quaternion.h"
#include "algorithm_mathUnit.h"

//Driver
#include "stm32f4_system.h"
#include "stm32f4xx_conf.h"
#include "stm32f4_delay.h"
#include "stm32f4_flash.h"
#include "stm32f4_i2c.h"
#include "stm32f4_spi.h"

//Module
#include "module_ak8975.h"
#include "module_board.h"
#include "module_motor.h"
#include "module_mpu6050.h"
#include "module_mpu9150.h"
#include "module_ms5611.h"
#include "module_rs232.h"
#include "module_sensor.h"
#include "stm32f4_sdio.h"

//Shell
#include "linenoise.h"
#include "shell.h"
#include "parser.h"
#include "status_monitor.h"

#endif
