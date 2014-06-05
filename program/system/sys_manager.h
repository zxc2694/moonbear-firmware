#ifndef __SYS_MANAGER_H
#define __SYS_MANAGER_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "stm32f4xx.h"

enum {
	PWM1_CCR = 0,
	PWM2_CCR,
	PWM3_CCR,
	PWM4_CCR,
	PWM5_CCR,
	RC_EXP_ROLL,
	RC_EXP_PITCH,
	RC_EXP_YAW,
	RC_EXP_THR,
	TRUE_ROLL,
	TRUE_PITCH,
	TRUE_YAW,
	NO_RC_SIGNAL_MSG,
	OPERATE_ROLL,
	OPERATE_PITCH,
	OPERATE_YAW,
	OPERATE_THR,
	MOTOR1,
	MOTOR2,
	MOTOR3,
	MOTOR4,
	test1,
	test2,
	test3,
	test4,
	Write_PWM_Motor1,
	Write_PWM_Motor2,
	Write_PWM_Motor3,
	Write_PWM_Motor4,
	BAROMETER,
	GLOABAL_PARAM_COUNT
};
typedef __IO enum {
	SYSTEM_UNINITIALIZED = 0,
	SYSTEM_INITIALIZED,
	SYSTEM_CORRECTION_SENSOR,
	SYSTEM_FLIGHT_CONTROL
} SYSTEM_STATUS;

typedef __IO enum {
	SD_READY = 0,
	SD_UNREADY, 
	SD_SAVE,
	SD_UNSAVE,
	SD_ERSAVE
} SD_STATUS;


typedef struct global_struct {

	char *param_name;
	__IO float param;

} global_struct;

typedef struct {
	char ch;
} serial_msg;

global_struct global_var[GLOABAL_PARAM_COUNT];



extern xSemaphoreHandle serial_tx_wait_sem;
extern xQueueHandle serial_rx_queue;
extern SD_STATUS SDstatus;
extern SD_STATUS SDcondition;
extern SYSTEM_STATUS set_PWM_Motors;
#endif
