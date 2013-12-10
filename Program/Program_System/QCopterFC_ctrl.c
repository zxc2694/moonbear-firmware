/*=====================================================================================================*/
/*=====================================================================================================*/
#include "stm32f4_system.h"
#include "QCopterFC_ctrl.h"
#include "QCopterFC_transport.h"
#include "module_board.h"
#include "module_motor.h"
#include "algorithm_pid.h"
/*=====================================================================================================*/
/*=====================================================================================================*/
vs16 PWM_M1 = PWM_MOTOR_MIN;
vs16 PWM_M2 = PWM_MOTOR_MIN;
vs16 PWM_M3 = PWM_MOTOR_MIN;
vs16 PWM_M4 = PWM_MOTOR_MIN;

extern __IO uint16_t PWM1_InputCaptureValue;
extern __IO uint16_t PWM2_InputCaptureValue;
extern __IO uint16_t PWM3_InputCaptureValue;
extern __IO uint16_t PWM4_InputCaptureValue;
extern __IO uint16_t PWM5_InputCaptureValue;
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
void Update_RC_Control(s16 *Roll, s16 *Pitch, s16 *Yaw, s16 *Thr)
{

	*Thr = (PWM_MOTOR_MAX - PWM_MOTOR_MIN) / (23179.0 - 13285.0) * (PWM3_InputCaptureValue - 13285) + 800;
	//TODO: Control Roll,Pich, Yaw angle
	*Roll = (180) / (22591.0 - 12715.0) * (PWM1_InputCaptureValue - 12715) + (-90);
	*Pitch = (180) / (23180.0 - 13290.0) * (PWM2_InputCaptureValue - 13290) + (-90);
	*Yaw = (180) / (23230.0 - 13344.0) * (PWM4_InputCaptureValue - 13344) + (-90);
}
