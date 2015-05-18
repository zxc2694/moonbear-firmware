#include "QuadCopterConfig.h"

#define PRINT_DEBUG(var1) serial.printf("DEBUG PRINT"#var1"\r\n")

xTaskHandle FlightControl_Handle = NULL;
xTaskHandle correction_task_handle = NULL;
xTaskHandle watch_task_handle = NULL;

Sensor_Mode SensorMode = Mode_GyrCorrect;

void vApplicationStackOverflowHook(xTaskHandle pxTask, signed char *pcTaskName)
{	// to prevent overflow from happening

	while (1) {
		LED_Toggle(LED_R);
		vTaskDelay(200);
	}
}
void vApplicationMallocFailedHook(void)
{	// 當 malloc failed 發生, 卡死在迴圈內並閃紅燈 (LED_Toggle)

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
	//進行GPIO、USART等等serial connection的初始化等工作，函數來自serial.c
	Motor_Config();
	PWM_Capture_Config();

	//IMU Config
	Sensor_Config();
	//IMU初始配置程式

#if configNRF  		// 若configNRF為1才執行調用
	nRF24L01_Config();
#endif	

#if configSD
	SD_Init();
#endif	

	PID_Init(&PID_Pitch, 4.0, 0.0, 1.5);
	PID_Init(&PID_Roll, 4.0, 0.0, 1.5);
	PID_Init(&PID_Yaw, 5.0, 0.0, 15.0);
	//設定PID

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
	
	system.status = SYSTEM_INITIALIZED;
		//至此初始化完成

}

void correction_task()  		//校正sensor
{
	ErrorStatus sensor_correct = ERROR;

	while (system.status == SYSTEM_UNINITIALIZED);

	while (sensor_correct == ERROR) {

		while (SensorMode != Mode_Algorithm) {		

			sensor_read();
			//讀取感測器資料
			correct_sensor();
			//校正感測器資料
			vTaskDelay(2);
		}

		if ((AngE.Roll < 0.1) && (AngE.Pitch < 0.1) && (NumQ.q0 < 1)
		    && (NumQ.q1 < 1) && (NumQ.q2 < 1) && (NumQ.q3 < 1)) {
			sensor_correct = SUCCESS ;
		} else {
			SensorMode = Mode_GyrCorrect;
			sensor_correct = ERROR ;
		}//若尤拉角的roll、pitch、四元數的q0、q1、q2、q3皆為門檻值以下，代表校正完成，
		//不然就將SensorMode設定回Mode_GyrCorrect並顯示ERROR

		LED_Toggle(LED_G);
		//校正過程結束後使LED亮綠燈
		vTaskDelay(200);
	}

	SetLED(LED_G, DISABLE);
	vTaskResume(FlightControl_Handle);
	//開啟FlightControl task
	vTaskDelete(NULL);
	//刪除自己本身的這個task
}

void flightControl_task()  		//飛行控制
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
			//從感測器獲取訊號並經過濾波處理後計算出roll、pitch、yaw、throttle、safety訊號
			
			/* Motor Ctrl */
			Final_M1 = Thr + Pitch - Roll + Yaw + adjustmentMotor1; //moonbear: - Yaw
			Final_M2 = Thr + Pitch + Roll - Yaw + adjustmentMotor2; //moonbear: + Yaw
			Final_M3 = Thr - Pitch + Roll + Yaw + adjustmentMotor3; //moonbear: - Yaw
			Final_M4 = Thr - Pitch - Roll - Yaw + adjustmentMotor4; //moonbear: + Yaw
			//將前述所得之roll、pitch、yaw、throttle、safety計算

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
			}// 對馬達下達輸出之PWM指令

			vTaskDelay(2);
		}
	}
}

void check_task()	//檢查是否要用到遙控器、並叫醒correction_task_handle
{
	//Waiting for system finish initialize
	while (system.status != SYSTEM_INITIALIZED);

#if configRemoteControl
	while (remote_signal_check() == NO_SIGNAL); //If there is no receiver but need to test IMU, please mask.
#endif
	
	SetLED(LED_B, DISABLE);
	vTaskResume(correction_task_handle);

	while (system.status != SYSTEM_FLIGHT_CONTROL);
	//檢察系統是否在飛行控制SYSTEM_FLIGHT_CONTROL狀態下，不是則卡死

	vTaskDelay(2000);
	SetLED(LED_R, ENABLE);
	SetLED(LED_G, ENABLE);
	SetLED(LED_B, ENABLE);
	//enable所有LED燈
	vTaskDelete(NULL);
	//刪除自己本身的這個task

}

void nrf_sending_task() 			//將資料經由nrf傳輸出去
{
#if configSTATUS_NRF
	char buf[128] = {0};
	nrf_package package;

	//Waiting for system finish initialize
	while (system.status == SYSTEM_UNINITIALIZED);

	nRF_TX_Mode(); 
	//將程式設定為傳輸(T)mode

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
		//將roll、pitch、yaw、加速規、陀螺儀資料紀錄成一個package

		nrf_generate_package(&package, (uint8_t *)buf);
		nrf_send_package((uint8_t *)buf);
		//並將此package壓至buffer內
	}
#endif

}

void lcd_task()  		//LCD ili9341 用來顯示飛行器的資料
{
	while (system.status != SYSTEM_INITIALIZED);

	TM_ILI9341_Init();
	TM_ILI9341_Rotate(TM_ILI9341_Orientation_Landscape_2);
	TM_ILI9341_Fill(ILI9341_COLOR_MAGENTA);
	TM_ILI9341_Puts(50, 15, "ICLab - Quadrotor", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
	TM_ILI9341_Puts(20, 50, "1. Roll:", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_MAGENTA);
	TM_ILI9341_Puts(20, 80, "2. Pitch:", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_MAGENTA);
	TM_ILI9341_Puts(20, 110, "3. Yaw:", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_MAGENTA);
	TM_ILI9341_Puts(20, 140, "4. Height:", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_MAGENTA);
	vTaskDelay(50);
//while(1);
	while(1){

		int LCD_Roll = AngE.Roll;
		int LCD_Pitch = AngE.Pitch;
		int LCD_Yaw = AngE.Yaw;
		int LCD_Roll_Tens, LCD_Pitch_Tens, LCD_Yaw_Tens, LCD_Roll_Digits, LCD_Pitch_Digits, LCD_Yaw_Digits;

		if(LCD_Roll<0){
			LCD_Roll = LCD_Roll *(-1);
			TM_ILI9341_Putc(140, 50, '-', &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_MAGENTA);
		}
		else
			TM_ILI9341_Putc(140, 50, ' ', &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_MAGENTA);

		if(LCD_Pitch<0){
			LCD_Pitch = LCD_Pitch *(-1);
			TM_ILI9341_Putc(140, 80, '-', &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_MAGENTA);
		}
		else
			TM_ILI9341_Putc(140, 80, ' ', &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_MAGENTA);

		if(LCD_Yaw<0){
			LCD_Yaw = LCD_Yaw *(-1);
			TM_ILI9341_Putc(140, 110, '-', &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_MAGENTA);
		}
		else
			TM_ILI9341_Putc(140, 110, ' ', &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_MAGENTA);

		/* 從int轉換為轉char */
	    LCD_Roll_Tens = LCD_Roll * 0.1;		//取roll角度的十位數
	    LCD_Roll_Digits = LCD_Roll % 10;	//取roll角度的個位數
	    LCD_Pitch_Tens = LCD_Pitch * 0.1; 
	    LCD_Pitch_Digits = LCD_Pitch % 10;
	    LCD_Yaw_Tens = LCD_Yaw * 0.1; 
	    LCD_Yaw_Digits = LCD_Yaw % 10;

	    /* 從char轉換為Ascii */
	    LCD_Roll_Tens = LCD_Roll_Tens + 48; // 0~9 加上48 為 Ascii
	    LCD_Pitch_Tens = LCD_Pitch_Tens + 48;
	    LCD_Yaw_Tens = LCD_Yaw_Tens + 48;
	    LCD_Roll_Digits = LCD_Roll_Digits + 48; 
	    LCD_Pitch_Digits = LCD_Pitch_Digits + 48;
	    LCD_Yaw_Digits = LCD_Yaw_Digits + 48;

	    /* 印出字串在螢幕  X,   Y,    string,         字體大小,           字體顏色,             背景顏色        */
		TM_ILI9341_Putc(150, 50, LCD_Roll_Tens, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_MAGENTA);
	    TM_ILI9341_Putc(150, 80, LCD_Pitch_Tens, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_MAGENTA);
	    TM_ILI9341_Putc(150, 110, LCD_Yaw_Tens, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_MAGENTA);
	    TM_ILI9341_Putc(160, 50, LCD_Roll_Digits, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_MAGENTA);
	    TM_ILI9341_Putc(160, 80, LCD_Pitch_Digits, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_MAGENTA);
	    TM_ILI9341_Putc(160, 110, LCD_Yaw_Digits, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_MAGENTA);
	}
}

int main(void) 		//主程式
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	//設定FreeRTOS系統之中斷優先順序為Group 4
	system_init();
	//系統初始化

	vSemaphoreCreateBinary(serial_tx_wait_sem);
	//以Semaphore設定USART Tx, Rx初始設定
	serial_rx_queue = xQueueCreate(1, sizeof(serial_msg));

	/* IMU Initialization, Attitude Correction Flight Control */
	xTaskCreate(check_task,
		    (signed portCHAR *) "Initial checking",
		    512, NULL,
		    tskIDLE_PRIORITY + 5, NULL);  	//一開始有被喚醒即NULL
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

	/* Support LCD ili9341 */
	xTaskCreate(lcd_task,
		    (signed portCHAR *) "stm32f429's LCD handler",
		    1024, NULL,
		    tskIDLE_PRIORITY + 5, NULL);

	vTaskSuspend(FlightControl_Handle);
	vTaskSuspend(correction_task_handle);
	vTaskSuspend(watch_task_handle);

	vTaskStartScheduler();

	return 0;
}

