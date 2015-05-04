#include "QuadCopterConfig.h"

#define PRINT_DEBUG(var1) serial.printf("DEBUG PRINT"#var1"\r\n")

xTaskHandle FlightControl_Handle = NULL;
xTaskHandle correction_task_handle = NULL;
xTaskHandle watch_task_handle = NULL;

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

#if configNRF
	nRF24L01_Config();
#endif	

	PID_Init(&PID_Pitch, 4.0, 0.0, 1.5);
	PID_Init(&PID_Roll, 4.0, 0.0, 1.5);
	PID_Init(&PID_Yaw, 5.0, 0.0, 15.0);

	Delay_10ms(10);

	Motor_Control(PWM_MOTOR_MIN, PWM_MOTOR_MIN, PWM_MOTOR_MIN, PWM_MOTOR_MIN);

#if configNRF
	/* nRF Check */
	while (nRF_Check() == ERROR);
#endif	

	/* Sensor Init */
	while (Sensor_Init() == ERROR);

	Delay_10ms(10);

	/* Lock */
	SetLED(LED_R, DISABLE);
	SetLED(LED_G, ENABLE);
	SetLED(LED_B, ENABLE);

#if configSD
	if (system.status != SYSTEM_ERROR_SD)
#endif		
		system.status = SYSTEM_INITIALIZED;

}

void correction_task()
{
	ErrorStatus sensor_correct = ERROR;

	while (system.status == SYSTEM_UNINITIALIZED);

	while (sensor_correct == ERROR) {

		while (SensorMode != Mode_Algorithm) {		

			sensor_read();
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

		int16_t Final_M1 = 0;
		int16_t Final_M2 = 0;
		int16_t Final_M3 = 0;
		int16_t Final_M4 = 0;

		int16_t Thr = 0, Pitch = 0, Roll = 0, Yaw = 0;
		int16_t safety = 0;

		sensor_read();
		if (SensorMode == Mode_Algorithm) {
			
			AHRS_and_RC_updata(&Thr, &Pitch, &Roll, &Yaw, &safety);
			
			/* Motor Ctrl */
			Final_M1 = Thr + Pitch - Roll + Yaw + adjustmentMotor1; //moonbear: - Yaw
			Final_M2 = Thr + Pitch + Roll - Yaw + adjustmentMotor2; //moonbear: + Yaw
			Final_M3 = Thr - Pitch + Roll + Yaw + adjustmentMotor3; //moonbear: - Yaw
			Final_M4 = Thr - Pitch - Roll - Yaw + adjustmentMotor4; //moonbear: + Yaw

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

#if configRemoteControl
	while (remote_signal_check() == NO_SIGNAL); //If there is no receiver but need to test IMU, please mask.
#endif
	
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
#if configSTATUS_NRF
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
#endif

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

#if configNRF
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

