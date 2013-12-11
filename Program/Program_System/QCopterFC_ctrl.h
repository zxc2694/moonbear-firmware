

#ifndef __QCOPTER_CTRL_H
#define __QCOPTER_CTRL_H

#include "stm32f4xx.h"
#include "algorithm_pid.h"
/*=====================================================================================================*/
#define MIN_PWM_INPUT 10000

#define MAX_PWM1_INPUT 22591.0
#define MIN_PWM1_INPUT 12715.0

#define MAX_PWM2_INPUT 23180.0
#define MIN_PWM2_INPUT 13290.0

#define MAX_PWM3_INPUT 23179.0
#define MIN_PWM3_INPUT 13285.0

#define MAX_PWM4_INPUT 23230.0
#define MIN_PWM4_INPUT 13344.0

#define MAX_PWM5_INPUT 24870.0
#define MIN_PWM5_INPUT 11588.0

#define MAX_CTRL_ROLL 90
#define MIN_CTRL_ROLL -90

#define MAX_CTRL_PITCH 90
#define MIN_CTRL_PITCH -90

#define MAX_CTRL_YAW 90
#define MIN_CTRL_YAW -90



/*=====================================================================================================*/
void CTRL_FlightControl(void);
void Update_RC_Control(int16_t *Roll, int16_t  *Pitch, int16_t  *Yaw, int16_t  *Thr, uint8_t *safety); 
/*=====================================================================================================*/
/*=====================================================================================================*/
extern vs16 PWM_M1;
extern vs16 PWM_M2;
extern vs16 PWM_M3;
extern vs16 PWM_M4;
/*=====================================================================================================*/
/*=====================================================================================================*/
#endif
