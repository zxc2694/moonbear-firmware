#include "QuadCopterConfig.h"

#define PRINT_DEBUG(var1) serial.printf("DEBUG PRINT"#var1"\r\n")

xTaskHandle FlightControl_Handle = NULL;
xTaskHandle correction_task_handle = NULL;
xTaskHandle watch_task_handle = NULL;

volatile int16_t ACC_FIFO[3][256] = {{0}};
volatile int16_t GYR_FIFO[3][256] = {{0}};
volatile int16_t MAG_FIFO[3][256] = {{0}};

volatile int16_t MagDataX[8] = {0};
volatile int16_t MagDataY[8] = {0};
volatile uint32_t Correction_Time = 0;

Sensor_Mode SensorMode = Mode_GyrCorrect;

void vApplicationStackOverflowHook(xTaskHandle pxTask, signed char *pcTaskName)
{
	while (1) {
		LED_Toggle(LED_R);
		vTaskDelay(200);
	}
}
void vApplicationMallocFailedHook(void)
{
	while (1) {
		LED_Toggle(LED_R);
		vTaskDelay(200);
	}
}

void vApplicationIdleHook(void)
{
}

void system_init(void)
{
	LED_Config();
	Serial_Config(Serial_Baudrate);
	Motor_Config();
	PWM_Capture_Config();

	//IMU Config
	Sensor_Config();
	nRF24L01_Config();

	//SD Config
	if ((SD_status = SD_Init()) != SD_OK)
		system.status = SYSTEM_ERROR_SD;

	PID_Init(&PID_Pitch, 4.0, 0.0, 1.5);
	PID_Init(&PID_Roll, 4.0, 0.0, 1.5);
	PID_Init(&PID_Yaw, 5.0, 0.0, 15.0);

	Delay_10ms(10);

	Motor_Control(PWM_MOTOR_MIN, PWM_MOTOR_MIN, PWM_MOTOR_MIN, PWM_MOTOR_MIN);

	/* nRF Check */
	while (nRF_Check() == ERROR);

	/* Sensor Init */
	while (Sensor_Init() == ERROR);

	Delay_10ms(10);

	/* Lock */
	SetLED(LED_R, DISABLE);
	SetLED(LED_G, ENABLE);
	SetLED(LED_B, ENABLE);

	//Check if no error
	if (system.status != SYSTEM_ERROR_SD)
		system.status = SYSTEM_INITIALIZED;

}

void correction_task()
{
	ErrorStatus sensor_correct = ERROR;

	while (system.status == SYSTEM_UNINITIALIZED);

	while (sensor_correct == ERROR) {

		while (SensorMode != Mode_Algorithm) {
			uint8_t IMU_Buf[20] = {0};

			static uint8_t BaroCnt = 0;

			/* 500Hz, Read Sensor ( Accelerometer, Gyroscope, Magnetometer ) */
			MPU9150_Read(IMU_Buf);

#ifdef Use_Barometer
			/* 100Hz, Read Barometer */
			BaroCnt++;

			if (BaroCnt == SampleRateFreg / 100) {
				MS5611_Read(&Baro, MS5611_D1_OSR_4096);
				BaroCnt = 0;
			}

#endif

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

		if ((AngE.Roll < 0.1) && (AngE.Pitch < 0.1) && (NumQ.q0 < 1)
		    && (NumQ.q1 < 1) && (NumQ.q2 < 1) && (NumQ.q3 < 1)) {
			sensor_correct = SUCCESS ;

		} else {
			SensorMode = Mode_GyrCorrect;
			sensor_correct = ERROR ;
		}

		LED_Toggle(LED_G);
		vTaskDelay(200);
	}

	SetLED(LED_G, DISABLE);
	vTaskResume(FlightControl_Handle);
	vTaskDelete(NULL);
}

void flightControl_task()
{
	//Waiting for system finish initialize
	while (system.status != SYSTEM_INITIALIZED);

	system.status = SYSTEM_FLIGHT_CONTROL;

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

#ifdef Use_Barometer
		/* 100Hz, Read Barometer */
		BaroCnt++;

		if (BaroCnt == SampleRateFreg / 100) {
			MS5611_Read(&Baro, MS5611_D1_OSR_4096);
			BaroCnt = 0;
		}

#endif
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
			system.variable[TRUE_ROLL].value = AngE.Roll;
			system.variable[TRUE_PITCH].value = AngE.Pitch;
			system.variable[TRUE_YAW].value = AngE.Yaw;

			/*Get RC Control*/
			Update_RC_Control(&Exp_Roll, &Exp_Pitch, &Exp_Yaw, &Exp_Thr, &safety);
			system.variable[RC_EXP_THR].value  = Exp_Thr;
			system.variable[RC_EXP_ROLL].value = Exp_Roll;
			system.variable[RC_EXP_PITCH].value = Exp_Pitch;
			system.variable[RC_EXP_YAW].value = Exp_Yaw;
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

			system.variable[PID_ROLL].value = Roll;
			system.variable[PID_PITCH].value = Pitch;
			system.variable[PID_YAW].value = Yaw;


			/* Motor Ctrl */
			Final_M1 = Thr + Pitch - Roll - Yaw;
			Final_M2 = Thr + Pitch + Roll + Yaw;
			Final_M3 = Thr - Pitch + Roll - Yaw;
			Final_M4 = Thr - Pitch - Roll + Yaw;

			system.variable[MOTOR1].value = Final_M1;
			system.variable[MOTOR2].value = Final_M2;
			system.variable[MOTOR3].value = Final_M3;
			system.variable[MOTOR4].value = Final_M4;

			Bound(Final_M1, PWM_MOTOR_MIN, PWM_MOTOR_MAX);
			Bound(Final_M2, PWM_MOTOR_MIN, PWM_MOTOR_MAX);
			Bound(Final_M3, PWM_MOTOR_MIN, PWM_MOTOR_MAX);
			Bound(Final_M4, PWM_MOTOR_MIN, PWM_MOTOR_MAX);


			if (safety == ENGINE_OFF) {
				system.variable[MOTOR1].value = PWM_MOTOR_MIN;
				system.variable[MOTOR2].value = PWM_MOTOR_MIN;
				system.variable[MOTOR3].value = PWM_MOTOR_MIN;
				system.variable[MOTOR4].value = PWM_MOTOR_MIN;
				Motor_Control(PWM_MOTOR_MIN, PWM_MOTOR_MIN, PWM_MOTOR_MIN, PWM_MOTOR_MIN);

			} else {
				Motor_Control(Final_M1, Final_M2, Final_M3, Final_M4);
			}

			vTaskDelay(2);


		}

	}
}

void check_task()
{
	//Waiting for system finish initialize
	while (system.status != SYSTEM_INITIALIZED);

	while (remote_signal_check() == NO_SIGNAL);

	SetLED(LED_B, DISABLE);
	vTaskResume(correction_task_handle);

	while (system.status != SYSTEM_FLIGHT_CONTROL);

	vTaskDelay(2000);
	SetLED(LED_R, ENABLE);
	SetLED(LED_G, ENABLE);
	SetLED(LED_B, ENABLE);
	vTaskDelete(NULL);

}

void nrf_sending_task()
{
	char buf[128] = {0};
	nrf_package package;

	//Waiting for system finish initialize
	while (system.status == SYSTEM_UNINITIALIZED);

	nRF_TX_Mode();

	while (1) {
		package.roll = (int16_t)system.variable[TRUE_ROLL].value * 100;
		package.pitch  = (int16_t)system.variable[TRUE_PITCH].value * 100;
		package.yaw = (int16_t)system.variable[TRUE_YAW].value * 100;
		package.acc_x = Acc.X;
		package.acc_y = Acc.Y;
		package.acc_z = Acc.Z;
		package.gyro_x = Gyr.X;
		package.gyro_y = Gyr.Y;
		package.gyro_z = Gyr.Z;

		nrf_generate_package(&package, (uint8_t *)buf);
		nrf_send_package((uint8_t *)buf);
	}

}

void error_handler_task()
{
	while (system.status != SYSTEM_ERROR_SD || system.status == SYSTEM_UNINITIALIZED) {
		if (system.status == SYSTEM_INITIALIZED)
			vTaskDelete(NULL);
	}

	/* Clear the screen */
	serial.printf("\x1b[H\x1b[2J");

	if (SD_status != SD_OK) {
		serial.printf("[System status]SD Initialized failed!\n\r");
		serial.printf("Please Insert the SD card correctly then reboot the QuadCopter!");
	}

	while (1);
}

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	system_init();

	vSemaphoreCreateBinary(serial_tx_wait_sem);
	serial_rx_queue = xQueueCreate(1, sizeof(serial_msg));

	/* IMU Initialization, Attitude Correction Flight Control */
	xTaskCreate(check_task,
		    (signed portCHAR *) "Initial checking",
		    512, NULL,
		    tskIDLE_PRIORITY + 5, NULL);
	xTaskCreate(correction_task,
		    (signed portCHAR *) "System correction",
		    4096, NULL,
		    tskIDLE_PRIORITY + 9, &correction_task_handle);

	xTaskCreate(flightControl_task,
		    (signed portCHAR *) "Flight control",
		    4096, NULL,
		    tskIDLE_PRIORITY + 9, &FlightControl_Handle);

	/* QuadCopter Developing Shell, Ground Station Software */
	xTaskCreate(shell_task,
		    (signed portCHAR *) "Shell",
		    2048, NULL,
		    tskIDLE_PRIORITY + 7, NULL);

#if configSTATUS_GUI
	xTaskCreate(nrf_sending_task,
		    (signed portCHAR *) "NRF Sending",
		    1024, NULL,
		    tskIDLE_PRIORITY + 5, NULL);
#endif

	/* Shell command handling task */
	xTaskCreate(watch_task,
		    (signed portCHAR *) "Watch",
		    1024, NULL,
		    tskIDLE_PRIORITY + 7, &watch_task_handle);

	/* System error handler*/
	xTaskCreate(error_handler_task,
		    (signed portCHAR *) "Error handler",
		    512, NULL,
		    tskIDLE_PRIORITY + 7, NULL);

	vTaskSuspend(FlightControl_Handle);
	vTaskSuspend(correction_task_handle);
	vTaskSuspend(watch_task_handle);

	vTaskStartScheduler();

	return 0;
}

