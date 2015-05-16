// IMU校正
// MPU9150 : 加速規、陀螺儀、電子羅盤
// MS5611 (Baro): 氣壓計
// AKB8975 : 電子羅盤

#include "QuadCopterConfig.h"
extern Sensor_Mode SensorMode;
extern SensorAcc Acc;
extern SensorGyr Gyr;
extern SensorMag Mag;
extern SensorTemp Temp;

volatile int16_t ACC_FIFO[3][256] = {{0}};
volatile int16_t GYR_FIFO[3][256] = {{0}};
volatile int16_t MAG_FIFO[3][256] = {{0}};

volatile int16_t MagDataX[8] = {0};
volatile int16_t MagDataY[8] = {0};
volatile uint32_t Correction_Time = 0;

void sensor_read()
{	// 讀取IMU數值

	uint8_t IMU_Buf[20] = {0};
	static uint8_t BaroCnt = 0;

	/* 500Hz, Read Sensor ( Accelerometer, Gyroscope, Magnetometer ) */
	MPU9150_Read(IMU_Buf);
	// 讀取加速規、陀螺儀、電子羅盤之值

	BaroCnt++;//100Hz, Read Barometer

	if (BaroCnt == SampleRateFreg / 100) {
		MS5611_Read(&Baro, MS5611_D1_OSR_4096);
		BaroCnt = 0;
	}// 每讀取前述三個感測器 "SampleRateFreg / 100" 次，讀取氣壓計一次
	
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
		//從buffer讀取IMU之值	

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

}

void correct_sensor() 
{	// 校正IMU
#define MovegAveFIFO_Size 250

	switch (SensorMode) {

	/************************** Mode_CorrectGyr **************************************/
	case Mode_GyrCorrect:
		/* Simple Moving Average */
		Gyr.X = (s16)MoveAve_SMA(Gyr.X, GYR_FIFO[0], MovegAveFIFO_Size);
		Gyr.Y = (s16)MoveAve_SMA(Gyr.Y, GYR_FIFO[1], MovegAveFIFO_Size);
		Gyr.Z = (s16)MoveAve_SMA(Gyr.Z, GYR_FIFO[2], MovegAveFIFO_Size);
		//將上述讀取出來的陀螺儀數值作平均

		Correction_Time++;  // 等待 FIFO 填滿空值 or 填滿靜態資料

		if (Correction_Time == SampleRateFreg) {
			Gyr.OffsetX += (Gyr.X - GYR_X_OFFSET);  // 角速度為 0dps
			Gyr.OffsetY += (Gyr.Y - GYR_Y_OFFSET);  // 角速度為 0dps
			Gyr.OffsetZ += (Gyr.Z - GYR_Z_OFFSET);  // 角速度為 0dps

			Correction_Time = 0;
			SensorMode = Mode_AccCorrect;
		}

		break;

	/************************** Mode_CorrectAcc **************************************/
	case Mode_AccCorrect:
		/* Simple Moving Average */
		Acc.X = (s16)MoveAve_SMA(Acc.X, ACC_FIFO[0], MovegAveFIFO_Size);
		Acc.Y = (s16)MoveAve_SMA(Acc.Y, ACC_FIFO[1], MovegAveFIFO_Size);
		Acc.Z = (s16)MoveAve_SMA(Acc.Z, ACC_FIFO[2], MovegAveFIFO_Size);
		//將上述讀取出來的加速規數值作平均

		Correction_Time++;  // 等待 FIFO 填滿空值 or 填滿靜態資料

		if (Correction_Time == SampleRateFreg) {
			Acc.OffsetX += (Acc.X - ACC_X_OFFSET);  // 重力加速度為 0g
			Acc.OffsetY += (Acc.Y - ACC_Y_OFFSET);  // 重力加速度為 0g
			Acc.OffsetZ += (Acc.Z - ACC_Z_OFFSET);  // 重力加速度為 1g

			Correction_Time = 0;
			SensorMode = Mode_Quaternion; // Mode_MagCorrect;
		}

		break;

	/************************** Algorithm Mode **************************************/
	case Mode_Quaternion:
		/* To Physical */
		/* 將數值轉換為現實世界中單位 */
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

		AngE.Pitch = toDeg(atan2f(Acc.TrueY, Acc.TrueZ));
		AngE.Roll  = toDeg(-asinf(Acc.TrueX));
		AngE.Yaw   = toDeg(atan2f(Mag.TrueX, Mag.TrueY)) + 180.0f;
		Quaternion_ToNumQ(&NumQ, &AngE);

		SensorMode = Mode_Algorithm;
		break;
	}

}

void AHRS_and_RC_updata(int16_t *Thr, int16_t *Pitch, int16_t *Roll, int16_t *Yaw, int16_t *safety)
{
	int16_t Exp_Thr = 0, Exp_Pitch = 0, Exp_Roll = 0, Exp_Yaw = 0;
	int16_t Safety = *safety ;


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

	system.variable[ACCX].value = Acc.TrueX;
	system.variable[ACCY].value = Acc.TrueY;
	system.variable[ACCZ].value = Acc.TrueZ;

	system.variable[GYROX].value = Gyr.TrueX;
	system.variable[GYROY].value = Gyr.TrueY;
	system.variable[GYROZ].value = Gyr.TrueZ;
	
	system.variable[MAGX].value = Mag.TrueX;
	system.variable[MAGY].value = Mag.TrueY;
	system.variable[MAGZ].value = Mag.TrueZ;

/* Get Attitude Angle */
	AHRS_Update();
	//計算四元數誤差以及將角度轉換為尤拉角，並使用互補式濾波器處理訊號
	system.variable[TRUE_ROLL].value = AngE.Roll;
	system.variable[TRUE_PITCH].value = AngE.Pitch;
	system.variable[TRUE_YAW].value = AngE.Yaw;

	/*Get RC Control*/
	Update_RC_Control(&Exp_Roll, &Exp_Pitch, &Exp_Yaw, &Exp_Thr, &Safety);
	//將輸入訊號依照遙控器型號不同分別處理，並計算出roll、pitch、yaw、throttle、safety訊號
	system.variable[RC_EXP_THR].value  = Exp_Thr;
	system.variable[RC_EXP_ROLL].value = Exp_Roll;
	system.variable[RC_EXP_PITCH].value = Exp_Pitch;
	system.variable[RC_EXP_YAW].value = Exp_Yaw;
	/* Get ZeroErr */
	PID_Pitch.ZeroErr = (float)((s16)Exp_Pitch);
	PID_Roll.ZeroErr  = (float)((s16)Exp_Roll);
	PID_Yaw.ZeroErr   = (float)((s16)Exp_Yaw) + 180.0f;

	/* PID */
	*Roll  = (s16)PID_AHRS_Cal(&PID_Roll,   AngE.Roll,  Gyr.TrueX);
	*Pitch = (s16)PID_AHRS_Cal(&PID_Pitch,  AngE.Pitch, Gyr.TrueY);
	*Yaw   = (s16)(PID_Yaw.Kd * Gyr.TrueZ) + 3 * (s16)Exp_Yaw;
	*Thr   = (s16)Exp_Thr;
	Bound(*Yaw, -90, 90);
   system.variable[PID_ROLL].value = *Roll;
	system.variable[PID_PITCH].value = *Pitch;
	system.variable[PID_YAW].value = *Yaw;

	if (AngE.Yaw > 180.0f) 
		AngE.Yaw=AngE.Yaw-360;
//	system.variable[TEST1].value = AngE.Yaw;

	*safety = Safety;
}

#define LOOP_DT  (0.00025f)

void PID_vertical_Zd(vertical_pid_t* PID_control, float *vertical_data)
{
	/* 高度控制程式 */
	if( PID_control -> controller_status == CONTROLLER_ENABLE){

		float error = (PID_control -> setpoint) - (*vertical_data);

		float P = error * (PID_control -> kp);

		PID_control -> integral += (error * (PID_control -> ki)) * LOOP_DT;

		PID_control -> integral = bound_float(PID_control -> integral,-20.0f,+20.0f);

		(PID_control -> output) = P+(PID_control -> integral);

		PID_control -> output = bound_float(PID_control -> output,PID_control -> out_min,PID_control -> out_max);

	}else{

		PID_control -> integral = 0.0f;
		PID_control -> output =0.0f;
	}
}