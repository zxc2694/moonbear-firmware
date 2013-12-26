/*=====================================================================================================*/
/*=====================================================================================================*/
#include "stm32f4_system.h"
#include "stm32f4_i2c.h"
#include "QCopterFC.h"
#include "QCopterFC_ctrl.h"
#include "QCopterFC_ahrs.h"
#include "module_board.h"
#include "module_motor.h"
#include "module_sensor.h"
#include "module_mpu9150.h"
#include "module_ms5611.h"
#include "algorithm_pid.h"
#include "algorithm_moveAve.h"
#include "algorithm_mathUnit.h"
#include "algorithm_quaternion.h"
#include "sys_manager.h"
#include "std.h"
#include "sensor_correct.h"
/*=====================================================================================================*/
/*=====================================================================================================*/

vs16 Tmp_PID_KP;
vs16 Tmp_PID_KI;
vs16 Tmp_PID_KD;
vs16 Tmp_PID_Pitch;

static __IO uint16_t pwm1_previous_value = 0;
static __IO uint16_t pwm2_previous_value = 0;
static __IO uint32_t pwm3_previous_value = 0;
static __IO uint32_t pwm4_previous_value = 0;
static __IO uint32_t pwm5_previous_value = 0;
static __IO uint8_t pwm1_is_rising = 1;
static __IO uint8_t pwm2_is_rising = 1;
static __IO uint8_t pwm3_is_rising = 1;
static __IO uint8_t pwm4_is_rising = 1;
static __IO uint8_t pwm5_is_rising = 1;

volatile int16_t ACC_FIFO[3][256] = {{0}};
volatile int16_t GYR_FIFO[3][256] = {{0}};
volatile int16_t MAG_FIFO[3][256] = {{0}};

volatile int16_t MagDataX[8] = {0};
volatile int16_t MagDataY[8] = {0};
volatile uint32_t Correction_Time = 0;

Sensor_Mode SensorMode = Mode_GyrCorrect;
/*=====================================================================================================*/
/*=====================================================================================================*/
void SysTick_Handler(void)
{
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

	correct_sensor( );

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
		PID_Roll.ZeroErr  = (float)((s16)Exp_Roll );
		PID_Yaw.ZeroErr   = (float)((s16)Exp_Yaw) + 180.0f;

		/* PID */
		Roll  = (s16)PID_AHRS_Cal(&PID_Roll,   AngE.Roll,  Gyr.TrueX);
		Pitch = (s16)PID_AHRS_Cal(&PID_Pitch,  AngE.Pitch, Gyr.TrueY);
//      Yaw   = (s16)PID_AHRS_CalYaw(&PID_Yaw, AngE.Yaw,   Gyr.TrueZ);
		Yaw   = (s16)(PID_Yaw.Kd * Gyr.TrueZ) + 3*(s16)Exp_Yaw;
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

	
	}
}
void TIM2_IRQHandler(void)
{
	uint32_t current[3];
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICStructInit(&TIM_ICInitStructure);

	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) == SET) {
		/* Clear TIM1 Capture compare interrupt pending bit */
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);

		if (pwm3_is_rising) {
		 	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
		 	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;

		 	/* Get the Input Capture value */
		 	pwm3_previous_value = TIM_GetCapture1(TIM2);
		 	pwm3_is_rising = 0;

		 } else {
		 	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
		 	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;

		 	//Get the Input Capture value 
		 	current[0] =  TIM_GetCapture1(TIM2);

		 	if (current[0] > pwm3_previous_value)
		 		global_var[PWM3_CCR].param =  current[0] - pwm3_previous_value;
		 	else if (current[0] < pwm3_previous_value)
		 		global_var[PWM3_CCR].param = 0xFFFF - pwm3_previous_value + current[0] ;

		 	pwm3_is_rising = 1;
		 }

		 TIM_ICInit(TIM2, &TIM_ICInitStructure);

	}

	if (TIM_GetITStatus(TIM2, TIM_IT_CC2) == SET) {
		/* Clear TIM1 Capture compare interrupt pending bit */
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);

		if (pwm4_is_rising) {
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
			TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;

			/* Get the Input Capture value */
			pwm4_previous_value = TIM_GetCapture2(TIM2);
			pwm4_is_rising = 0;

		} else {
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
			TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;

			/* Get the Input Capture value */
			current[1] =  TIM_GetCapture2(TIM2);

			if (current[1] > pwm4_previous_value)
				global_var[PWM4_CCR].param =  current[1] - pwm4_previous_value;
			else if (current[1] < pwm4_previous_value)
				global_var[PWM4_CCR].param = 0xFFFF - pwm4_previous_value + current[1] ;

			pwm4_is_rising = 1;


		}

		TIM_ICInit(TIM2, &TIM_ICInitStructure);
	}

	if (TIM_GetITStatus(TIM2, TIM_IT_CC3) == SET) {
		/* Clear TIM1 Capture compare interrupt pending bit */
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);

		if (pwm5_is_rising) {
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
			TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;

			/* Get the Input Capture value */
			pwm5_previous_value = TIM_GetCapture3(TIM2);
			pwm5_is_rising = 0;

		} else {
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
			TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;

			/* Get the Input Capture value */
			current[2] =  TIM_GetCapture3(TIM2);

			if (current[2] > pwm5_previous_value)
				global_var[PWM5_CCR].param =  current[2] - pwm5_previous_value;
			else if (current[2] < pwm5_previous_value)
				global_var[PWM5_CCR].param = 0xFFFF - pwm5_previous_value + current[2] ;

			pwm5_is_rising = 1;


		}

		TIM_ICInit(TIM2, &TIM_ICInitStructure);
	}
}
void TIM4_IRQHandler(void)
{
	uint16_t current[2];
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICStructInit(&TIM_ICInitStructure);

	if (TIM_GetITStatus(TIM4, TIM_IT_CC1) == SET) {
		/* Clear TIM1 Capture compare interrupt pending bit */
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);

		if (pwm1_is_rising) {
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
			TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;

			/* Get the Input Capture value */
			pwm1_previous_value = TIM_GetCapture1(TIM4);
			pwm1_is_rising = 0;

		} else {
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
			TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;

			/* Get the Input Capture value */
			current[0] =  TIM_GetCapture1(TIM4);

			if (current[0] > pwm1_previous_value)
				global_var[PWM1_CCR].param =  current[0] - pwm1_previous_value;
			else if (current[0] < pwm1_previous_value)
				global_var[PWM1_CCR].param = 0xFFFF - pwm1_previous_value + current[0] ;

			pwm1_is_rising = 1;
		}

		TIM_ICInit(TIM4, &TIM_ICInitStructure);

	}

	if (TIM_GetITStatus(TIM4, TIM_IT_CC2) == SET) {
		/* Clear TIM1 Capture compare interrupt pending bit */
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC2);

		if (pwm2_is_rising) {
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
			TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;

			/* Get the Input Capture value */
			pwm2_previous_value = TIM_GetCapture2(TIM4);
			pwm2_is_rising = 0;

		} else {
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
			TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;

			/* Get the Input Capture value */
			current[1] =  TIM_GetCapture2(TIM4);

			if (current[1] > pwm2_previous_value)
				global_var[PWM2_CCR].param =  current[1] - pwm2_previous_value;
			else if (current[1] < pwm2_previous_value)
				global_var[PWM2_CCR].param = 0xFFFF - pwm2_previous_value + current[1] ;

			pwm2_is_rising = 1;


		}

		TIM_ICInit(TIM4, &TIM_ICInitStructure);


	}

}
/*=====================================================================================================*/
/*=====================================================================================================*/
void DMA1_Stream0_IRQHandler(void)
{
	I2C_RX_DMA_IRQ();
}
/*=====================================================================================================*/
/*=====================================================================================================*/
void DMA1_Stream6_IRQHandler(void)
{
	I2C_TX_DMA_IRQ();
}
/*=====================================================================================================*/
/*=====================================================================================================*/
void HardFault_Handler(void)
{
	while (1);
}
void MemManage_Handler(void)
{
	while (1);
}
void BusFault_Handler(void)
{
	while (1);
}
void UsageFault_Handler(void)
{
	while (1);
}
void SVC_Handler(void)
{
	while (1);
}
void DebugMon_Handler(void)
{
	while (1);
}
void PendSV_Handler(void)
{
	while (1);
}
void NMI_Handler(void)
{
	while (1);
}
/*=====================================================================================================*/
/*=====================================================================================================*/
