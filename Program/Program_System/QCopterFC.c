/*=====================================================================================================*/
/*=====================================================================================================*/
#include "stm32f4_system.h"
#include "QCopterFC.h"
#include "QCopterFC_ctrl.h"
#include "QCopterFC_ahrs.h"
#include "module_board.h"
#include "module_rs232.h"
#include "module_motor.h"
#include "module_sensor.h"
#include "module_nrf24l01.h"
#include "sys_manager.h"
#include "test.h"
/*=====================================================================================================*/
#define PRINT_DEBUG(var1) printf("DEBUG PRINT"#var1"\r\n")
/*=====================================================================================================*/
void set_nvic()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

}
void set_systick()
{

}
void System_Init(void)
{
	SystemInit();
	set_nvic();
	LED_Config();
	KEY_Config();
	RS232_Config();
	Motor_Config();
	PWM_Capture_Config();
	Sensor_Config();

	PID_Init(&PID_Yaw);
	PID_Init(&PID_Roll);
	PID_Init(&PID_Pitch);

	PID_Pitch.Kp = +4.0f;
	PID_Pitch.Ki = 0;//0.002f;
	PID_Pitch.Kd = +1.5f;

	PID_Roll.Kp  = +4.0f;
	PID_Roll.Ki  = 0;//0.002f;
	PID_Roll.Kd  = 1.5f;

	PID_Yaw.Kp   = +5.0f;
	PID_Yaw.Ki   = +0.0f;
	PID_Yaw.Kd   = +15.0f;

	Delay_10ms(200);
}

/*=====================================================================================================*/
/*=====================================================================================================*/
int main(void)
{
	u8 Sta = ERROR;
	FSM_Mode FSM_State = FSM_Rx;

	/* System Init */
	System_Init();
	test_printf();
	PRINT_DEBUG(555);
	while( remote_signal_check() == NO_SIGNAL);
	/* Throttle Config */
	if (KEY == 1) {
		LED_B = 0;
		Motor_Control(PWM_MOTOR_MAX, PWM_MOTOR_MAX, PWM_MOTOR_MAX, PWM_MOTOR_MAX);
	}

	while (KEY == 1);

	LED_B = 1;
	Motor_Control(PWM_MOTOR_MIN, PWM_MOTOR_MIN, PWM_MOTOR_MIN, PWM_MOTOR_MIN);


	/* Sensor Init */
	if (Sensor_Init() == SUCCESS)
		LED_G = 0;

	Delay_10ms(10);

	/* Systick Config */
	if (SysTick_Config(SystemCoreClock / SampleRateFreg)) { // SampleRateFreg = 500 Hz
		
		
		while (1);
	}

	/* Wait Correction */
	while (SensorMode != Mode_Algorithm);

	/* Lock */
	LED_R = 1;
	LED_G = 1;
	LED_B = 1;

	while (!KEY) {

	LED_B = ~LED_B;
	printf("\x1b[H\x1b[2J");


	printf("Roll = %f,Pitch = %f,Yaw = %f \r\n",
		AngE.Roll, AngE.Pitch, AngE.Yaw);
	printf("CH1 %f(%f),CH2 %f(%f),CH3 %f(%f),CH4 %f(%f),CH5 %f()\r\n",
		global_var[PWM1_CCR].param, global_var[RC_EXP_ROLL].param,
		global_var[PWM2_CCR].param, global_var[RC_EXP_PITCH].param,
		global_var[PWM3_CCR].param, global_var[RC_EXP_THR].param,
		global_var[PWM4_CCR].param, global_var[RC_EXP_YAW].param,
		global_var[PWM5_CCR].param);
	printf("PID Roll,%f,PID PITCH,%f,PID YAW,%f\r\n",
		global_var[PID_ROLL].param,
		global_var[PID_PITCH].param,
		global_var[PID_YAW].param);
	printf("MOTOR 1,%f,MOTOR 2,%f,MOTOR 3,%f,MOTOR 4,%f",
		global_var[MOTOR1].param,
		global_var[MOTOR2].param,
		global_var[MOTOR3].param,
		global_var[MOTOR4].param);
	Delay_10ms(20);	
		
	}

	LED_B = 1;


}
/*=====================================================================================================*/
/*=====================================================================================================*/
