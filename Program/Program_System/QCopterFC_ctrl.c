/*=====================================================================================================*/
/*=====================================================================================================*/
#include "stm32f4_system.h"
#include "QCopterFC_ctrl.h"
#include "QCopterFC_transport.h"
#include "module_board.h"
#include "module_motor.h"
#include "algorithm_pid.h"
#include "sys_manager.h"
#include "QCopterFC_ctrl.h"
/*=====================================================================================================*/
/*=====================================================================================================*/
vs16 PWM_M1 = PWM_MOTOR_MIN;
vs16 PWM_M2 = PWM_MOTOR_MIN;
vs16 PWM_M3 = PWM_MOTOR_MIN;
vs16 PWM_M4 = PWM_MOTOR_MIN;

/*=====================================================================================================*/
/*=====================================================================================================*/
void CTRL_FlightControl(void)
{
	u16 TempM1 = 0;
	u16 TempM2 = 0;
	u16 TempM3 = 0;
	u16 TempM4 = 0;
	u16 TempThr = 0;

	if (KEYR_L == 0) {
		TempM1 = PWM_MOTOR_MIN;
		TempM2 = PWM_MOTOR_MIN;
		TempM3 = PWM_MOTOR_MIN;
		TempM4 = PWM_MOTOR_MIN;
		PWM_M1 = TempM1;
		PWM_M2 = TempM2;
		PWM_M3 = TempM3;
		PWM_M4 = TempM4;

	} else if (KEYL_J == 0) {
		TempThr = PWM_MOTOR_MIN + JSR_Z * 0.30379746835443037974683544303797f - 70;

		if (TempThr < PWM_MOTOR_MIN)
			TempThr = PWM_MOTOR_MIN;

		if (TempThr > PWM_MOTOR_MAX)
			TempThr = PWM_MOTOR_MAX;

		TempM1 = TempThr;
		TempM2 = TempThr;
		TempM3 = TempThr;
		TempM4 = TempThr;
		PWM_M1 = TempM1;
		PWM_M2 = TempM2;
		PWM_M3 = TempM3;
		PWM_M4 = TempM4;
	}
}
/*=====================================================================================================*/
/*=====================================================================================================*/
void Update_RC_Control(int16_t *Roll, int16_t  *Pitch, int16_t  *Yaw, int16_t  *Thr, uint8_t *safety)
{
	if( global_var[PWM3_CCR].param > MIN_PWM_INPUT){

		*Thr = (PWM_MOTOR_MAX - PWM_MOTOR_MIN) / (MAX_PWM3_INPUT - MIN_PWM3_INPUT) * 
			(global_var[PWM3_CCR].param - MIN_PWM3_INPUT) + PWM_MOTOR_MIN;
	}

	if( global_var[PWM1_CCR].param > MIN_PWM_INPUT){

		*Roll = (MAX_CTRL_ROLL - MIN_CTRL_ROLL) / (MAX_PWM1_INPUT - MIN_PWM1_INPUT) * 
			(global_var[PWM1_CCR].param - MIN_PWM1_INPUT) +  MIN_CTRL_ROLL;
	}

	if( global_var[PWM2_CCR].param > MIN_PWM_INPUT){

		*Pitch = (MAX_CTRL_PITCH - MIN_CTRL_PITCH) / (MAX_PWM2_INPUT - MIN_PWM2_INPUT) * 
			(global_var[PWM2_CCR].param - MIN_PWM2_INPUT) + MIN_CTRL_PITCH;
	}

	if( global_var[PWM4_CCR].param > MIN_PWM_INPUT){	

		*Yaw = (MAX_CTRL_YAW- MIN_CTRL_YAW) / (MAX_PWM4_INPUT - MIN_PWM4_INPUT) * 
			(global_var[PWM4_CCR].param - MIN_PWM4_INPUT) + MIN_CTRL_YAW;

	}

	if( global_var[PWM5_CCR].param > (MAX_PWM5_INPUT - MIN_PWM5_INPUT)/2 )
		*safety = 1;

}
