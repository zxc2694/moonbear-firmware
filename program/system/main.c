/*=====================================================================================================*/
/*=====================================================================================================*/
#include "QuadCopterConfig.h"

xTaskHandle FlightControl_Handle = NULL;
xTaskHandle correction_task_handle = NULL;
xSemaphoreHandle sdio_semaphore = NULL;
volatile int16_t ACC_FIFO[3][256] = {{0}};
volatile int16_t GYR_FIFO[3][256] = {{0}};
volatile int16_t MAG_FIFO[3][256] = {{0}};

volatile int16_t MagDataX[8] = {0};
volatile int16_t MagDataY[8] = {0};
volatile uint32_t Correction_Time = 0;

Sensor_Mode SensorMode = Mode_GyrCorrect;
extern SYSTEM_STATUS sys_status;

/*=====================================================================================================*/
#define PRINT_DEBUG(var1) printf("DEBUG PRINT"#var1"\r\n")
/*=====================================================================================================*/
void vApplicationStackOverflowHook(xTaskHandle pxTask, signed char *pcTaskName)
{
	while(1) {
		LED_R = ~LED_R;
		vTaskDelay(200);
	}
}
void vApplicationMallocFailedHook( void )
{
	while(1) {
		LED_R = ~LED_R;
		vTaskDelay(200);
	}
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

#if configSD_BOARD
	SDIO_Config();
#endif

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

	Delay_10ms(10);

	Motor_Control(PWM_MOTOR_MIN, PWM_MOTOR_MIN, PWM_MOTOR_MIN, PWM_MOTOR_MIN);


#if configFLIGHT_CONTROL_BOARD
	/* nRF Check */
	while ( nRF_Check()== ERROR );

	/* Sensor Init */
	while(Sensor_Init() == ERROR);
#endif

	Delay_10ms(10);

	/* Lock */
	LED_R = 0;
	LED_G = 1;
	LED_B = 1;

	sys_status = SYSTEM_INITIALIZED;

}

void correction_task()
{
	ErrorStatus sensor_correct = ERROR;

	while (sys_status == SYSTEM_UNINITIALIZED);

	while ( sensor_correct == ERROR ) {

		while (SensorMode != Mode_Algorithm) {
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

			correct_sensor();

			vTaskDelay(2);
		}

		if( (AngE.Roll < 0.1) && (AngE.Pitch < 0.1) && (NumQ.q0<1)
		     && (NumQ.q1<1) && (NumQ.q2<1) && (NumQ.q3<1) ) {
			sensor_correct = SUCCESS ; 
		}
		else {
			SensorMode = Mode_GyrCorrect;
			sensor_correct = ERROR ;
		}

		LED_G = ~LED_G ;
		vTaskDelay(200);
	}
	LED_G = 0;
	vTaskResume(FlightControl_Handle);
	vTaskDelete(NULL);
}

void flightControl_task()
{
	//Waiting for system finish initialize
	while (sys_status == SYSTEM_UNINITIALIZED);
	sys_status = SYSTEM_FLIGHT_CONTROL;
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

		if (SensorMode == Mode_Algorithm) {

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
			global_var[TRUE_ROLL].param = AngE.Roll;
			global_var[TRUE_PITCH].param = AngE.Pitch;
			global_var[TRUE_YAW].param = AngE.Yaw;

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


			if (safety == 1) {
				Motor_Control(PWM_MOTOR_MIN, PWM_MOTOR_MIN, PWM_MOTOR_MIN, PWM_MOTOR_MIN);

			} else {
				Motor_Control(Final_M1, Final_M2, Final_M3, Final_M4);
			}

			vTaskDelay(2);


		}

	}
}

void statusReport_task()
{
	//Waiting for system finish initialize
	while (sys_status == SYSTEM_UNINITIALIZED);

	/* Show the Initialization message */
	printf("[System status]Initialized successfully!\n\r");

	while (1) {






		vTaskDelay(100);
	}
}

void check_task()
{
	//Waiting for system finish initialize
	while (sys_status == SYSTEM_UNINITIALIZED);

	while (remote_signal_check() == NO_SIGNAL);
	LED_B = 0;
	vTaskResume(correction_task_handle);
	while(sys_status != SYSTEM_FLIGHT_CONTROL);
	vTaskDelay(2000);
	LED_R = 1;
	LED_G = 1;
	LED_B = 1;
	vTaskDelete(NULL);

}

void nrf_sending_task()
{
	char buf[128]={0};
	nrf_package package;

	//Waiting for system finish initialize
	while (sys_status == SYSTEM_UNINITIALIZED);

	nRF_TX_Mode();
	while(1){
		package.roll = (int16_t)global_var[TRUE_ROLL].param*100;
		package.pitch  = (int16_t)global_var[TRUE_PITCH].param*100;
		package.yaw = (int16_t)global_var[TRUE_YAW].param*100;
		package.acc_x = Acc.X;
		package.acc_y = Acc.Y;
		package.acc_z = Acc.Z;
		package.gyro_x = Gyr.X;
		package.gyro_y = Gyr.Y;
		package.gyro_z = Gyr.Z;

		nrf_generate_package( &package, (uint8_t* )buf);
		nrf_send_package( (uint8_t* )buf );
	}

}
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	system_init();

	vSemaphoreCreateBinary(serial_tx_wait_sem);
	serial_rx_queue = xQueueCreate(1, sizeof(serial_msg));
	/*Create sdio run semaphore*/
	sdio_semaphore = xSemaphoreCreateBinary();
	xTaskCreate(check_task,
		    (signed portCHAR *) "Initial checking",
		    512, NULL,
		    tskIDLE_PRIORITY + 5, NULL);
	xTaskCreate(correction_task,
		    (signed portCHAR *) "Initial checking",
		    4096, NULL,
		    tskIDLE_PRIORITY + 9, &correction_task_handle);
#if configSD_BOARD
	 xTaskCreate(sdio_task,
	 	    (signed portCHAR *) "Using SD card",
	 	    512, NULL,
	 	    tskIDLE_PRIORITY + 6, NULL);
#endif




#if configSTATUS_SHELL
	xTaskCreate(shell_task,
		    (signed portCHAR *) "Shell",
		    2048, NULL,
		    tskIDLE_PRIORITY + 5, NULL);
#endif

	xTaskCreate(flightControl_task,
		    (signed portCHAR *) "Flight control",
		    4096, NULL,
		    tskIDLE_PRIORITY + 9, &FlightControl_Handle);
#if configSTATUS_GUI
	xTaskCreate(nrf_sending_task,
	(signed portCHAR *) "NRF_SENDING",
	1024, NULL,
	tskIDLE_PRIORITY + 5, NULL);
#endif
	vTaskSuspend(FlightControl_Handle);
	vTaskSuspend(correction_task_handle);

	vTaskStartScheduler();

	return 0;
}
/*=====================================================================================================*/
/*=====================================================================================================*/
