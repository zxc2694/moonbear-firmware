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
static __IO uint16_t rc_ctrl_timeout = 0;
/*=====================================================================================================*/
/*=====================================================================================================*/
/*=====================================================================================================*/
/*=====================================================================================================*/
void Update_RC_Control(int16_t *Roll, int16_t  *Pitch, int16_t  *Yaw, int16_t  *Thr, uint8_t *safety)
{
	RC_State rc_state = GET_SIGNAL;
	/*Get PWM3 Input capture to control trottle*/
	if( global_var[PWM1_CCR].param > MIN_PWM_INPUT){

		*Thr = (PWM_MOTOR_MAX - PWM_MOTOR_MIN) / (MAX_PWM1_INPUT - MIN_PWM1_INPUT) * 
			(global_var[PWM1_CCR].param - MIN_PWM1_INPUT) + PWM_MOTOR_MIN;

		rc_state |= GET_SIGNAL;

	} else {
		
		rc_state |= NO_SIGNAL;

	}
	/*Get PWM1 Input capture to control roll*/
	// if( global_var[PWM1_CCR].param > MIN_PWM_INPUT){

	// 	*Roll = (MAX_CTRL_ROLL - MIN_CTRL_ROLL) / (MAX_PWM1_INPUT - MIN_PWM1_INPUT) * 
	// 		(global_var[PWM1_CCR].param - MIN_PWM1_INPUT) +  MIN_CTRL_ROLL;

	// 	rc_state |= GET_SIGNAL;

	// } else {

	// 	rc_state |= NO_SIGNAL;

	// }
	// /*Get PWM2 Input capture to control pitch*/
	// if( global_var[PWM2_CCR].param > MIN_PWM_INPUT){

	// 	*Pitch = (MAX_CTRL_PITCH - MIN_CTRL_PITCH) / (MAX_PWM2_INPUT - MIN_PWM2_INPUT) * 
	// 		(global_var[PWM2_CCR].param - MIN_PWM2_INPUT) + MIN_CTRL_PITCH;

	// 	rc_state |= GET_SIGNAL;

	// } else {

	// 	rc_state |= NO_SIGNAL;
	// }
	// /*Get PWM4 Input capture to control yaw*/
	// if( global_var[PWM4_CCR].param > MIN_PWM_INPUT){	

	// 	*Yaw = (MAX_CTRL_YAW- MIN_CTRL_YAW) / (MAX_PWM4_INPUT - MIN_PWM4_INPUT) * 
	// 		(global_var[PWM4_CCR].param - MIN_PWM4_INPUT) + MIN_CTRL_YAW;

	// 	rc_state |= GET_SIGNAL;

	// } else {

	// 	rc_state |= NO_SIGNAL;
		
	// }
	// /*Get PWM5 Input capture to set safety switch*/
	if( global_var[PWM5_CCR].param > (MAX_PWM5_INPUT - MIN_PWM5_INPUT)/2 )
		*safety = 1;
	else
		*safety = 0;

	if ( rc_state == NO_SIGNAL)
		rc_ctrl_timeout++;

	if ( rc_ctrl_timeout >= 500)
		global_var[NO_RC_SIGNAL_MSG].param = 1;

}
