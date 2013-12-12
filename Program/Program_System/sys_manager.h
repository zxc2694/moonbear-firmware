
#ifndef __SYS_MANAGER_H
#define __SYS_MANAGER_H

#include "stm32f4xx.h"
enum 
{
	PWM1_CCR=0,
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
	GLOABAL_PARAM_COUNT
};


typedef struct global_struct
{
	
	char *param_name;
	__IO float param;

}global_struct;

global_struct global_var[GLOABAL_PARAM_COUNT];

#endif
