/*=====================================================================================================*/
/*=====================================================================================================*/
#include "QuadCopterConfig.h"

xTaskHandle FlightControl_Handle = NULL;
xTaskHandle correction_task_handle = NULL;
xSemaphoreHandle sdio_semaphore = NULL;

extern Sensor_Mode SensorMode;
extern SYSTEM_STATUS sys_status;

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
	PID_Init(&PID_Yaw);
	PID_Init(&PID_Roll);
	PID_Init(&PID_Pitch);

	PID_Pitch.Kp = +2.8f;	//4.0f * 0.7 = 2.8f
	PID_Pitch.Ki = 0;
	PID_Pitch.Kd = +1.05f;	//1.5f * 0.7 = 1.05f

	PID_Roll.Kp = +2.8f;	//4.0f * 0.7 = 2.8f
	PID_Roll.Ki = 0;
	PID_Roll.Kd = +1.05f ;	//1.5f * 0.7 = 1.05f

	PID_Yaw.Kp = +3.5f ;	//5.0f * 0.7 = 3.5f
	PID_Yaw.Ki = 0;
	PID_Yaw.Kd = +10.5f;	//15.0f * 0.7=10.5f

#if QuadrotorMotorcycle
	PID_Pitch.Kp = +5.0f;	
	PID_Pitch.Ki = 0;
	PID_Pitch.Kd = +10.0f;	

	PID_Roll.Kp = +5.0f;	
	PID_Roll.Ki = 0;
	PID_Roll.Kd = +10.0f ;	

	PID_Yaw.Kp = +3.5f ;	
	PID_Yaw.Ki = 0;
	PID_Yaw.Kd = +10.5f;
#endif		

	Motor_Control(PWM_MOTOR_MIN, PWM_MOTOR_MIN, PWM_MOTOR_MIN, PWM_MOTOR_MIN);
	while(Sensor_Init() == ERROR); //Sensor Init
	Delay_10ms(10);

	LED_R = 0;
	LED_G = 1;
	LED_B = 1;

	sys_status = SYSTEM_INITIALIZED;
	set_PWM_Motors = SYSTEM_UNINITIALIZED;
}

void correction_task()
{
	ErrorStatus sensor_correct = ERROR;
	while (sys_status == SYSTEM_UNINITIALIZED);
	while ( sensor_correct == ERROR ) {
		while (SensorMode != Mode_Algorithm) {
			sensor_read();
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
		int16_t Final_M1 = 0, Final_M2 = 0, Final_M3 = 0, Final_M4 = 0;
		int16_t Thr = 0, Pitch = 0, Roll = 0, Yaw = 0;		
		int16_t safety = 0;

		sensor_read();
		if (SensorMode == Mode_Algorithm) {
			AHRS_and_RC_updata(&Thr, &Pitch, &Roll, &Yaw, &safety);

			Final_M1 = Thr + Pitch - Roll + Yaw; //moonbear: - Yaw
			Final_M2 = Thr + Pitch + Roll - Yaw; //moonbear: + Yaw
			Final_M3 = Thr - Pitch + Roll + Yaw; //moonbear: - Yaw
			Final_M4 = Thr - Pitch - Roll - Yaw; //moonbear: + Yaw

#if QuadrotorMotorcycle
			Final_M1 = 110 + Thr + Pitch - Roll + Yaw; 
			Final_M2 = 110 + Thr + Pitch + Roll - Yaw; 
			Final_M3 = 110 + Thr - Pitch + Roll + Yaw; 
			Final_M4 = 110 + Thr - Pitch - Roll - Yaw; 
#endif				
			Bound(Final_M1, PWM_MOTOR_MIN, PWM_MOTOR_MAX);
			Bound(Final_M2, PWM_MOTOR_MIN, PWM_MOTOR_MAX);
			Bound(Final_M3, PWM_MOTOR_MIN, PWM_MOTOR_MAX);
			Bound(Final_M4, PWM_MOTOR_MIN, PWM_MOTOR_MAX);

			global_var[MOTOR1].param = Final_M1;
			global_var[MOTOR2].param = Final_M2;
			global_var[MOTOR3].param = Final_M3;
			global_var[MOTOR4].param = Final_M4;

			if (safety == 1) {
				Motor_Control(PWM_MOTOR_MIN, PWM_MOTOR_MIN, PWM_MOTOR_MIN, PWM_MOTOR_MIN);
			} 
			else {		
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

void check_task() //protect switch
{
	//Waiting for system finish initialize
	while (sys_status == SYSTEM_UNINITIALIZED);

#if configMotor
	while (remote_signal_check() == NO_SIGNAL); //If there is no receiver but need to test IMU, please mask.
#endif
	LED_B = 0;
	vTaskResume(correction_task_handle);
	while(sys_status != SYSTEM_FLIGHT_CONTROL);
	vTaskDelay(2000);
	LED_R = 1;
	LED_G = 1;
	LED_B = 1;
	vTaskDelete(NULL);
}

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	system_init();
	vSemaphoreCreateBinary(serial_tx_wait_sem);
	serial_rx_queue = xQueueCreate(1, sizeof(serial_msg));

	xTaskCreate(check_task,
		    (signed portCHAR *) "Protect switch",
		    512, NULL,
		    tskIDLE_PRIORITY + 5, NULL);
	xTaskCreate(correction_task,
		    (signed portCHAR *) "Initial checking",
		    4096, NULL,
		    tskIDLE_PRIORITY + 9, &correction_task_handle);
	xTaskCreate(shell_task,
		    (signed portCHAR *) "Shell",
		    2048, NULL,
		    tskIDLE_PRIORITY + 5, NULL);

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
