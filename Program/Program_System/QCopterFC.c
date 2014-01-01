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
#include "module_mpu9150.h"
#include "module_ms5611.h"

#include "test.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "algorithm_pid.h"
#include "algorithm_moveAve.h"
#include "algorithm_mathUnit.h"
#include "algorithm_quaternion.h"

#include "sys_manager.h"
#include "std.h"
#include "linenoise.h"
#include "shell.h"

xSemaphoreHandle TIM2_Semaphore = NULL;
xSemaphoreHandle TIM4_Semaphore = NULL;
xTaskHandle FlightControl_Handle = NULL;
xTaskHandle correction_task_handle = NULL;
volatile int16_t ACC_FIFO[3][256] = {{0}};
volatile int16_t GYR_FIFO[3][256] = {{0}};
volatile int16_t MAG_FIFO[3][256] = {{0}};

volatile int16_t MagDataX[8] = {0};
volatile int16_t MagDataY[8] = {0};
volatile uint32_t Correction_Time = 0;

Sensor_Mode SensorMode = Mode_GyrCorrect;

//#define SHELL_IS_EXIST

enum SYSTEM_STATUS {
	SYSTEM_UNINITIALIZED,
	SYSTEM_INITIALIZED
};

int System_Status = SYSTEM_UNINITIALIZED;

/*=====================================================================================================*/
#define PRINT_DEBUG(var1) printf("DEBUG PRINT"#var1"\r\n")
/*=====================================================================================================*/
void vApplicationTickHook(void)
{
}


void vApplicationStackOverflowHook(xTaskHandle pxTask, signed char *pcTaskName)
{
}

void vApplicationIdleHook(void)
{

}

void system_init(void)
{	
	LED_Config();
	KEY_Config();
	RS232_Config();
	Motor_Config();
	PWM_Capture_Config();
	Sensor_Config();
	nRF24L01_Config();


	PID_Init(&PID_Yaw);
	PID_Init(&PID_Roll);
	PID_Init(&PID_Pitch);

 	PID_Pitch.Kp = +4.0f;
        PID_Pitch.Ki = 0;//0.002f;
        PID_Pitch.Kd = +1.5f;

        PID_Roll.Kp = +4.0f;
        PID_Roll.Ki = 0;//0.002f;
        PID_Roll.Kd = 1.5f;

        PID_Yaw.Kp = +5.0f;
        PID_Yaw.Ki = +0.0f;
        PID_Yaw.Kd = +15.0f;

        Delay_10ms(200);

	u8 Sta = ERROR;


	//while (remote_signal_check() == NO_SIGNAL);

	if (KEY == 1) {
		LED_B = 0;
		Motor_Control(PWM_MOTOR_MAX, PWM_MOTOR_MAX, PWM_MOTOR_MAX, PWM_MOTOR_MAX);
	}

	while (KEY == 1);

	LED_B = 1;
	Motor_Control(PWM_MOTOR_MIN, PWM_MOTOR_MIN, PWM_MOTOR_MIN, PWM_MOTOR_MIN);


#ifndef NRF_NOT_EXIST
	/* nRF Check */
	while (Sta == ERROR)
		Sta = nRF_Check();
#endif

#ifndef SENSOR_NOT_EXIST
	/* Sensor Init */
	if (Sensor_Init() == SUCCESS)
		LED_G = 0;
#endif

	Delay_10ms(10);


	/* Wait Correction */
	//while (SensorMode != Mode_Algorithm);

	/* Lock */
	LED_R = 1;
	LED_G = 1;
	LED_B = 1;

	System_Status = SYSTEM_INITIALIZED;

	/* Clear the screen */
	//putstr("\x1b[H\x1b[2J");


}

void correction_task()
{
	while(System_Status == SYSTEM_UNINITIALIZED);

	while(SensorMode != Mode_Algorithm)
	{
	LED_B = ~LED_B; //task indicator
	uint8_t IMU_Buf[20] = {0};

	static uint8_t BaroCnt = 0;


	/* 500Hz, Read Sensor ( Accelerometer, Gyroscope, Magnetometer ) */
	MPU9150_Read(IMU_Buf);

	/* 100Hz, Read Barometer */
	BaroCnt++;

	if (BaroCnt == SampleRateFreg / 100) {
		MS5611_Read(&Baro, MS5611_D1_OSR_4096);
		BaroCnt = 0;
	}

	Acc.X  = (s16)((IMU_Buf[0]  << 8) | IMU_Buf[1]);
	Acc.Y  = (s16)((IMU_Buf[2]  << 8) | IMU_Buf[3]);
	Acc.Z  = (s16)((IMU_Buf[4]  << 8) | IMU_Buf[5]);
	Temp.T = (s16)((IMU_Buf[6]  << 8) | IMU_Buf[7]);
	Gyr.X  = (s16)((IMU_Buf[8]  << 8) | IMU_Buf[9]);
	Gyr.Y  = (s16)((IMU_Buf[10] << 8) | IMU_Buf[11]);
	Gyr.Z  = (s16)((IMU_Buf[12] << 8) | IMU_Buf[13]);
	Mag.X  = (s16)((IMU_Buf[15] << 8) | IMU_Buf[14]);
	Mag.Y  = (s16)((IMU_Buf[17] << 8) | IMU_Buf[16]);
	Mag.Z  = (s16)((IMU_Buf[19] << 8) | IMU_Buf[18]);

	/* Offset */
	Acc.X -= Acc.OffsetX;
	Acc.Y -= Acc.OffsetY;
	Acc.Z -= Acc.OffsetZ;
	Gyr.X -= Gyr.OffsetX;
	Gyr.Y -= Gyr.OffsetY;
	Gyr.Z -= Gyr.OffsetZ;
	Mag.X *= Mag.AdjustX;
	Mag.Y *= Mag.AdjustY;
	Mag.Z *= Mag.AdjustZ;

	correct_sensor( );

	vTaskDelay(2);
	}
	vTaskResume(FlightControl_Handle);
	vTaskDelete(NULL);
}

void flightControl_task()
{
	//Waiting for system finish initialize
	while(System_Status == SYSTEM_UNINITIALIZED);

	while (1) {
	GPIO_ToggleBits(GPIOC, GPIO_Pin_7);
	uint8_t IMU_Buf[20] = {0};

	int16_t Final_M1 = 0;
	int16_t Final_M2 = 0;
	int16_t Final_M3 = 0;
	int16_t Final_M4 = 0;

	int16_t Thr = 0, Pitch = 0, Roll = 0, Yaw = 0;
	int16_t Exp_Thr = 0, Exp_Pitch = 0, Exp_Roll = 0, Exp_Yaw = 0;
	uint8_t safety = 0;
	

	static uint8_t BaroCnt = 0;



	/* 500Hz, Read Sensor ( Accelerometer, Gyroscope, Magnetometer ) */
	MPU9150_Read(IMU_Buf);

	/* 100Hz, Read Barometer */
	BaroCnt++;

	if (BaroCnt == SampleRateFreg / 100) {
		MS5611_Read(&Baro, MS5611_D1_OSR_4096);
		BaroCnt = 0;
	}

	Acc.X  = (s16)((IMU_Buf[0]  << 8) | IMU_Buf[1]);
	Acc.Y  = (s16)((IMU_Buf[2]  << 8) | IMU_Buf[3]);
	Acc.Z  = (s16)((IMU_Buf[4]  << 8) | IMU_Buf[5]);
	Temp.T = (s16)((IMU_Buf[6]  << 8) | IMU_Buf[7]);
	Gyr.X  = (s16)((IMU_Buf[8]  << 8) | IMU_Buf[9]);
	Gyr.Y  = (s16)((IMU_Buf[10] << 8) | IMU_Buf[11]);
	Gyr.Z  = (s16)((IMU_Buf[12] << 8) | IMU_Buf[13]);
	Mag.X  = (s16)((IMU_Buf[15] << 8) | IMU_Buf[14]);
	Mag.Y  = (s16)((IMU_Buf[17] << 8) | IMU_Buf[16]);
	Mag.Z  = (s16)((IMU_Buf[19] << 8) | IMU_Buf[18]);

	/* Offset */
	Acc.X -= Acc.OffsetX;
	Acc.Y -= Acc.OffsetY;
	Acc.Z -= Acc.OffsetZ;
	Gyr.X -= Gyr.OffsetX;
	Gyr.Y -= Gyr.OffsetY;
	Gyr.Z -= Gyr.OffsetZ;
	Mag.X *= Mag.AdjustX;
	Mag.Y *= Mag.AdjustY;
	Mag.Z *= Mag.AdjustZ;

	if ( SensorMode == Mode_Algorithm ){

		/* 加權移動平均法 Weighted Moving Average */
		Acc.X = (s16)MoveAve_WMA(Acc.X, ACC_FIFO[0], 8);
		Acc.Y = (s16)MoveAve_WMA(Acc.Y, ACC_FIFO[1], 8);
		Acc.Z = (s16)MoveAve_WMA(Acc.Z, ACC_FIFO[2], 8);
		Gyr.X = (s16)MoveAve_WMA(Gyr.X, GYR_FIFO[0], 8);
		Gyr.Y = (s16)MoveAve_WMA(Gyr.Y, GYR_FIFO[1], 8);
		Gyr.Z = (s16)MoveAve_WMA(Gyr.Z, GYR_FIFO[2], 8);
		Mag.X = (s16)MoveAve_WMA(Mag.X, MAG_FIFO[0], 64);
		Mag.Y = (s16)MoveAve_WMA(Mag.Y, MAG_FIFO[1], 64);
		Mag.Z = (s16)MoveAve_WMA(Mag.Z, MAG_FIFO[2], 64);

	/* To Physical */
		Acc.TrueX = Acc.X * MPU9150A_4g;      // g/LSB
		Acc.TrueY = Acc.Y * MPU9150A_4g;      // g/LSB
		Acc.TrueZ = Acc.Z * MPU9150A_4g;      // g/LSB
		Gyr.TrueX = Gyr.X * MPU9150G_2000dps; // dps/LSB
		Gyr.TrueY = Gyr.Y * MPU9150G_2000dps; // dps/LSB
		Gyr.TrueZ = Gyr.Z * MPU9150G_2000dps; // dps/LSB
		Mag.TrueX = Mag.X * MPU9150M_1200uT;  // uT/LSB
		Mag.TrueY = Mag.Y * MPU9150M_1200uT;  // uT/LSB
		Mag.TrueZ = Mag.Z * MPU9150M_1200uT;  // uT/LSB
		Temp.TrueT = Temp.T * MPU9150T_85degC; // degC/LSB

		/* Get Attitude Angle */
		AHRS_Update();


		/*Get RC Control*/
		Update_RC_Control(&Exp_Roll, &Exp_Pitch, &Exp_Yaw, &Exp_Thr, &safety);
		global_var[RC_EXP_THR].param  = Exp_Thr;
		global_var[RC_EXP_ROLL].param = Exp_Roll;
		global_var[RC_EXP_PITCH].param = Exp_Pitch;
		global_var[RC_EXP_YAW].param = Exp_Yaw;
		/* Get ZeroErr */
		PID_Pitch.ZeroErr = (float)((s16)Exp_Pitch);
		PID_Roll.ZeroErr  = (float)((s16)Exp_Roll);
		PID_Yaw.ZeroErr   = (float)((s16)Exp_Yaw) + 180.0f;

		/* PID */
		Roll  = (s16)PID_AHRS_Cal(&PID_Roll,   AngE.Roll,  Gyr.TrueX);
		Pitch = (s16)PID_AHRS_Cal(&PID_Pitch,  AngE.Pitch, Gyr.TrueY);
//      Yaw   = (s16)PID_AHRS_CalYaw(&PID_Yaw, AngE.Yaw,   Gyr.TrueZ);
		Yaw   = (s16)(PID_Yaw.Kd * Gyr.TrueZ) + 3 * (s16)Exp_Yaw;
		Thr   = (s16)Exp_Thr;

		global_var[PID_ROLL].param = Roll;
		global_var[PID_PITCH].param = Pitch;
		global_var[PID_YAW].param = Yaw;


		/* Motor Ctrl */
		Final_M1 = Thr + Pitch - Roll - Yaw;
		Final_M2 = Thr + Pitch + Roll + Yaw;
		Final_M3 = Thr - Pitch + Roll - Yaw;
		Final_M4 = Thr - Pitch - Roll + Yaw;
		
		global_var[MOTOR1].param = Final_M1;
		global_var[MOTOR2].param = Final_M2;
		global_var[MOTOR3].param = Final_M3;
		global_var[MOTOR4].param = Final_M4;

		Bound(Final_M1, PWM_MOTOR_MIN, PWM_MOTOR_MAX);
		Bound(Final_M2, PWM_MOTOR_MIN, PWM_MOTOR_MAX);
		Bound(Final_M3, PWM_MOTOR_MIN, PWM_MOTOR_MAX);
		Bound(Final_M4, PWM_MOTOR_MIN, PWM_MOTOR_MAX);


		if ( safety == 1 ){
			Motor_Control(PWM_MOTOR_MIN, PWM_MOTOR_MIN, PWM_MOTOR_MIN, PWM_MOTOR_MIN);
		}else{
			Motor_Control(Final_M1, Final_M2, Final_M3, Final_M4);
		}
		vTaskDelay(2);

	
	}

	}
}


void statusReport_task()
{
	//Waiting for system finish initialize
	while(System_Status == SYSTEM_UNINITIALIZED);
		/* Show the Initialization message */
	printf("[System status]Initialized successfully!\n\r");	
	
	while (1) {
		printf("Roll = %f,Pitch = %f,Yaw = %f \r\n",
		       AngE.Roll, AngE.Pitch, AngE.Yaw);
		printf("CH1 %f(%f),CH2 %f(%f),CH3 %f(%f),CH4 %f(%f),CH5 %f()\r\n",
		       global_var[PWM1_CCR].param, global_var[RC_EXP_ROLL].param,
		       global_var[PWM2_CCR].param, global_var[RC_EXP_PITCH].param,
		       global_var[PWM3_CCR].param, global_var[RC_EXP_THR].param,
		       global_var[PWM4_CCR].param, global_var[RC_EXP_YAW].param,
		       global_var[PWM5_CCR].param);
		printf("\r\n");

		vTaskDelay(500);
	}
}

/*=====================================================================================================*/

void check_task()
{
	//Waiting for system finish initialize
	while(System_Status == SYSTEM_UNINITIALIZED);

	while (remote_signal_check() == NO_SIGNAL);
	vTaskResume(correction_task_handle);
	vTaskDelete(NULL);

}

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	system_init();
	vSemaphoreCreateBinary(TIM2_Semaphore);
	vSemaphoreCreateBinary(TIM4_Semaphore);
	vSemaphoreCreateBinary(serial_tx_wait_sem);
	serial_rx_queue = xQueueCreate(1, sizeof(serial_msg));
	xTaskCreate(check_task,
		(signed portCHAR *) "Initial checking",
		512, NULL,
		tskIDLE_PRIORITY + 5, NULL);
	xTaskCreate(correction_task,
		(signed portCHAR *) "Initial checking",
		4096, NULL,
		tskIDLE_PRIORITY + 5, &correction_task_handle);

#ifndef SHELL_IS_EXIST
	xTaskCreate(statusReport_task,
		(signed portCHAR *) "Status report",
		512, NULL,
		tskIDLE_PRIORITY + 5, NULL);
#endif

#ifdef SHELL_IS_EXIST
	xTaskCreate(shell_task,
		(signed portCHAR *) "Shell",
		2048, NULL,
		tskIDLE_PRIORITY + 5, NULL);
#endif

	xTaskCreate(flightControl_task,
		(signed portCHAR *) "Flight control",
		4096, NULL,
		tskIDLE_PRIORITY + 9, &FlightControl_Handle);

	vTaskSuspend(FlightControl_Handle);
	vTaskSuspend(correction_task_handle);
	vTaskStartScheduler();

	return 0;
}
/*=====================================================================================================*/
/*=====================================================================================================*/
