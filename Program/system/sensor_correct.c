#include "QuadCopterConfig.h"
extern Sensor_Mode SensorMode;
extern SensorAcc Acc;
extern SensorGyr Gyr;
extern SensorMag Mag;
extern SensorTemp Temp;

extern volatile int16_t ACC_FIFO[3][256];
extern volatile int16_t GYR_FIFO[3][256];
extern volatile int16_t MAG_FIFO[3][256];

extern volatile int16_t MagDataX[8];
extern volatile int16_t MagDataY[8];
extern volatile uint32_t Correction_Time;

void correct_sensor()
{
	float Ellipse[5] = {0};
#define MovegAveFIFO_Size 250

	switch (SensorMode) {

	/************************** Mode_CorrectGyr **************************************/
	case Mode_GyrCorrect:
		/* Simple Moving Average */
		Gyr.X = (s16)MoveAve_SMA(Gyr.X, GYR_FIFO[0], MovegAveFIFO_Size);
		Gyr.Y = (s16)MoveAve_SMA(Gyr.Y, GYR_FIFO[1], MovegAveFIFO_Size);
		Gyr.Z = (s16)MoveAve_SMA(Gyr.Z, GYR_FIFO[2], MovegAveFIFO_Size);

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

		Correction_Time++;  // 等待 FIFO 填滿空值 or 填滿靜態資料

		if (Correction_Time == SampleRateFreg) {
			Acc.OffsetX += (Acc.X - ACC_X_OFFSET);  // 重力加速度為 0g
			Acc.OffsetY += (Acc.Y - ACC_Y_OFFSET);  // 重力加速度為 0g
			Acc.OffsetZ += (Acc.Z - ACC_Z_OFFSET);  // 重力加速度為 1g

			Correction_Time = 0;
			SensorMode = Mode_Quaternion; // Mode_MagCorrect;
		}

		break;

		/************************** Mode_CorrectMag **************************************/
#define MagCorrect_Ave    100
#define MagCorrect_Delay  600   // DelayTime : SampleRate * 600

	case Mode_MagCorrect:
		Correction_Time++;

		switch ((u16)(Correction_Time / MagCorrect_Delay)) {
		case 0:

			MagDataX[0] = (s16)MoveAve_WMA(Mag.X, MAG_FIFO[0], MagCorrect_Ave);
			MagDataY[0] = (s16)MoveAve_WMA(Mag.Y, MAG_FIFO[1], MagCorrect_Ave);
			break;

		case 1:

			MagDataX[1] = (s16)MoveAve_WMA(Mag.X, MAG_FIFO[0], MagCorrect_Ave);
			MagDataY[1] = (s16)MoveAve_WMA(Mag.Y, MAG_FIFO[1], MagCorrect_Ave);
			break;

		case 2:

			MagDataX[2] = (s16)MoveAve_WMA(Mag.X, MAG_FIFO[0], MagCorrect_Ave);
			MagDataY[2] = (s16)MoveAve_WMA(Mag.Y, MAG_FIFO[1], MagCorrect_Ave);
			break;

		case 3:

			MagDataX[3] = (s16)MoveAve_WMA(Mag.X, MAG_FIFO[0], MagCorrect_Ave);
			MagDataY[3] = (s16)MoveAve_WMA(Mag.Y, MAG_FIFO[1], MagCorrect_Ave);
			break;

		case 4:

			MagDataX[4] = (s16)MoveAve_WMA(Mag.X, MAG_FIFO[0], MagCorrect_Ave);
			MagDataY[4] = (s16)MoveAve_WMA(Mag.Y, MAG_FIFO[1], MagCorrect_Ave);
			break;

		case 5:

			MagDataX[5] = (s16)MoveAve_WMA(Mag.X, MAG_FIFO[0], MagCorrect_Ave);
			MagDataY[5] = (s16)MoveAve_WMA(Mag.Y, MAG_FIFO[1], MagCorrect_Ave);
			break;

		case 6:

			MagDataX[6] = (s16)MoveAve_WMA(Mag.X, MAG_FIFO[0], MagCorrect_Ave);
			MagDataY[6] = (s16)MoveAve_WMA(Mag.Y, MAG_FIFO[1], MagCorrect_Ave);
			break;

		case 7:

			MagDataX[7] = (s16)MoveAve_WMA(Mag.X, MAG_FIFO[0], MagCorrect_Ave);
			MagDataY[7] = (s16)MoveAve_WMA(Mag.Y, MAG_FIFO[1], MagCorrect_Ave);
			break;

		default:

			EllipseFitting(Ellipse, MagDataX, MagDataY, 8);
			Mag.EllipseSita = Ellipse[0];
			Mag.EllipseX0   = Ellipse[1];
			Mag.EllipseY0   = Ellipse[2];
			Mag.EllipseA    = Ellipse[3];
			Mag.EllipseB    = Ellipse[4];

			Correction_Time = 0;
			SensorMode = Mode_Quaternion;
			break;
		}

		break;

	/************************** Algorithm Mode **************************************/
	case Mode_Quaternion:
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

		Ellipse[3] = (Mag.X * arm_cos_f32(Mag.EllipseSita) + Mag.Y * arm_sin_f32(Mag.EllipseSita)) / Mag.EllipseB;
		Ellipse[4] = (-Mag.X * arm_sin_f32(Mag.EllipseSita) + Mag.Y * arm_cos_f32(Mag.EllipseSita)) / Mag.EllipseA;

		AngE.Pitch = toDeg(atan2f(Acc.TrueY, Acc.TrueZ));
		AngE.Roll  = toDeg(-asinf(Acc.TrueX));
		AngE.Yaw   = toDeg(atan2f(Ellipse[3], Ellipse[4])) + 180.0f;

		Quaternion_ToNumQ(&NumQ, &AngE);

		SensorMode = Mode_Algorithm;
		break;
	}

}
