#include "QuadCopterConfig.h"

vs16 PWM_M1 = PWM_MOTOR_MIN;
vs16 PWM_M2 = PWM_MOTOR_MIN;
vs16 PWM_M3 = PWM_MOTOR_MIN;
vs16 PWM_M4 = PWM_MOTOR_MIN;

static __IO uint16_t rc_ctrl_timeout = 0;

void Update_RC_Control(int16_t *Roll, int16_t  *Pitch, int16_t  *Yaw, int16_t  *Thr, int16_t *safety)
{
	/*Get PWM3 Input capture to control trottle*/
	if ((system.variable[PWM3_CCR].value > MIN_PWM_INPUT) &&
	    (system.variable[PWM3_CCR].value < MAX_PWM_INPUT)) {

		*Thr = (PWM_MOTOR_MAX - PWM_MOTOR_MIN) / (MAX_PWM3_INPUT - MIN_PWM3_INPUT) *
		       (system.variable[PWM3_CCR].value - MIN_PWM3_INPUT) + PWM_MOTOR_MIN;
	}//若輸入訊號在正常範圍內 (大於MIN_PWM_INPUT、小於MAX_PWM_INPUT)，
	//則將PWM3(推力)訊號正規化(換算至PWM_MOTOR_MAX至MIN之間的大小)

	/*Get PWM1 Input capture to control roll*/
	if ((system.variable[PWM1_CCR].value > MIN_PWM_INPUT) &&
	    (system.variable[PWM1_CCR].value < MAX_PWM_INPUT)) {
#if USE_WFLY_CONTROLLER 		//依照不同遙控器做控制訊號上的調整
		*Roll = MAX_CTRL_ROLL / (MAX_PWM1_INPUT - MID_PWM1_INPUT) *
			(system.variable[PWM1_CCR].value - MID_PWM1_INPUT);
#endif

#if USE_DEVO_v10_CONTROLLER
			*Roll = -1*(MAX_CTRL_ROLL / (MAX_PWM1_INPUT - MID_PWM1_INPUT) *
			(system.variable[PWM1_CCR].value - MID_PWM1_INPUT));
#endif
	}

	/*Get PWM2 Input capture to control pitch*/
	if ((system.variable[PWM2_CCR].value > MIN_PWM_INPUT) &&
	    (system.variable[PWM2_CCR].value < MAX_PWM_INPUT)) {

		*Pitch = MAX_CTRL_PITCH / (MAX_PWM2_INPUT - MID_PWM2_INPUT) *
			 (system.variable[PWM2_CCR].value - MID_PWM2_INPUT);
	}

	/*Get PWM4 Input capture to control yaw*/
	if ((system.variable[PWM4_CCR].value > MIN_PWM_INPUT) &&
	    (system.variable[PWM4_CCR].value < MAX_PWM_INPUT)) {
#if USE_WFLY_CONTROLLER
		*Yaw = MAX_CTRL_YAW / (MAX_PWM4_INPUT - MID_PWM4_INPUT) *
		       (system.variable[PWM4_CCR].value - MID_PWM4_INPUT);
#endif

#if USE_DEVO_v10_CONTROLLER
		       *Yaw = -1*(MAX_CTRL_YAW / (MAX_PWM4_INPUT - MID_PWM4_INPUT) *
		       (system.variable[PWM4_CCR].value - MID_PWM4_INPUT));
#endif		
	}

	/*Get PWM5 Input capture to set safety switch*/
#if USE_WFLY_CONTROLLER
	if (system.variable[PWM5_CCR].value > (MAX_PWM5_INPUT + MIN_PWM5_INPUT) / 2)
		*safety = ENGINE_OFF;
	else
		*safety = ENGINE_ON;
#endif

#if USE_DEVO_v10_CONTROLLER
	if (system.variable[PWM5_CCR].value > (MAX_PWM5_INPUT + MIN_PWM5_INPUT) / 2)
		*safety = ENGINE_ON;
	else
		*safety = ENGINE_OFF;
#endif	

	Bound(*Roll, MIN_CTRL_ROLL, MAX_CTRL_ROLL);
	Bound(*Pitch, MIN_CTRL_PITCH, MAX_CTRL_PITCH);
	Bound(*Yaw, MIN_CTRL_YAW, MAX_CTRL_YAW);
	Bound(*Thr, PWM_MOTOR_MIN, PWM_MOTOR_MAX);
}

RC_State remote_signal_check()
{
	RC_State rc_state = GET_SIGNAL;

	/*Get PWM3 Input capture to control trottle*/
	if ((system.variable[PWM3_CCR].value > MIN_PWM_INPUT) &&
	    (system.variable[PWM3_CCR].value < MAX_PWM_INPUT)) {

		rc_state |= GET_SIGNAL;

	} else {
		rc_state |= NO_SIGNAL;
	}

	/*Get PWM1 Input capture to control roll*/
	if ((system.variable[PWM1_CCR].value > MIN_PWM_INPUT) &&
	    (system.variable[PWM1_CCR].value < MAX_PWM_INPUT)) {

		rc_state |= GET_SIGNAL;

	} else {

		rc_state |= NO_SIGNAL;
	}

	/*Get PWM2 Input capture to control pitch*/
	if ((system.variable[PWM2_CCR].value > MIN_PWM_INPUT) &&
	    (system.variable[PWM2_CCR].value < MAX_PWM_INPUT)) {

		rc_state |= GET_SIGNAL;

	} else {

		rc_state |= NO_SIGNAL;
	}

	/*Get PWM4 Input capture to control yaw*/
	if ((system.variable[PWM4_CCR].value > MIN_PWM_INPUT) &&
	    (system.variable[PWM4_CCR].value < MAX_PWM_INPUT)) {

		rc_state |= GET_SIGNAL;

	} else {

		rc_state |= NO_SIGNAL;
	}

	return rc_state;
}

