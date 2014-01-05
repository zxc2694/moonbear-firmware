#ifndef __QUADCOPTER_CONFIG_H
#define __QUADCOPTER_CONFIG_H

/* Hardware environments */
#define configFLIGHT_CONTROL_BOARD	1
#define configSD_BOARD			1

/* Status report functions */
#define configSTATUS_GUI		0
#define configSTATUS_SHELL		1

/* Includes */
//STM32
#include "stm32f4_system.h"
#include "stm32f4xx_conf.h"

//FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "sys_manager.h"
#include "std.h"

#include "sdio.h"

#include "QCopterFC.h"
#include "QCopterFC_ctrl.h"
#include "QCopterFC_ahrs.h"

#include "module_board.h"
#include "module_rs232.h"
#include "module_motor.h"
#include "module_sensor.h"
#include "module_nrf24l01.h"
#include "module_mpu9150.h"
#include "module_ms5611.h"

#include "algorithm_pid.h"
#include "algorithm_moveAve.h"
#include "algorithm_mathUnit.h"
#include "algorithm_quaternion.h"

/* Shell */
#include "linenoise.h"
#include "shell.h"

/* Unit test */
#include "test.h"

#endif
