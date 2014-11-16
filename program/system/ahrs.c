#include "QuadCopterConfig.h"
/*=====================================================================================================*/
/*=====================================================================================================*/
#define Kp 15.0f
#define Ki 0.020f//0.02f

/*=====================================================================================================*/
/*=====================================================================================================*/
void AHRS_Update(void)
{
	float tempX = 0, tempY = 0;
	float Normalize;
	float gx, gy, gz;

	float ErrX, ErrY, ErrZ;
	float AccX, AccY, AccZ;
	float GyrX, GyrY, GyrZ;

	static float AngZ_Temp = 0.0f;
	static float exInt = 0.0f, eyInt = 0.0f, ezInt = 0.0f;

	gx = 2.0f * (NumQ.q1 * NumQ.q3 - NumQ.q0 * NumQ.q2);
	gy = 2.0f * (NumQ.q0 * NumQ.q1 + NumQ.q2 * NumQ.q3);
	gz = NumQ.q0 * NumQ.q0 - NumQ.q1 * NumQ.q1 - NumQ.q2 * NumQ.q2 + NumQ.q3 * NumQ.q3;

	Normalize = invSqrtf(squa(Acc.TrueX) + squa(Acc.TrueY) + squa(Acc.TrueZ));
	AccX = Acc.TrueX * Normalize;
	AccY = Acc.TrueY * Normalize;
	AccZ = Acc.TrueZ * Normalize;

	ErrX = (AccY * gz - AccZ * gy);
	ErrY = (AccZ * gx - AccX * gz);
	ErrZ = (AccX * gy - AccY * gx);

	exInt = exInt + ErrX * Ki;
	eyInt = eyInt + ErrY * Ki;
	ezInt = ezInt + ErrZ * Ki;

	GyrX = toRad(Gyr.TrueX);
	GyrY = toRad(Gyr.TrueY);
	GyrZ = toRad(Gyr.TrueZ);

	GyrX = GyrX + Kp * ErrX + exInt;
	GyrY = GyrY + Kp * ErrY + eyInt;
	GyrZ = GyrZ + Kp * ErrZ + ezInt;

	Quaternion_RungeKutta(&NumQ, GyrX, GyrY, GyrZ, SampleRateHelf);
	Quaternion_Normalize(&NumQ);
	Quaternion_ToAngE(&NumQ, &AngE);

	tempX    = (Mag.X * arm_cos_f32(Mag.EllipseSita) + Mag.Y * arm_sin_f32(Mag.EllipseSita)) / Mag.EllipseB;
	tempY    = (-Mag.X * arm_sin_f32(Mag.EllipseSita) + Mag.Y * arm_cos_f32(Mag.EllipseSita)) / Mag.EllipseA;
	AngE.Yaw = atan2f(tempX, tempY);

	AngE.Pitch = toDeg(AngE.Pitch);
	AngE.Roll  = toDeg(AngE.Roll);
	AngE.Yaw   = toDeg(AngE.Yaw) + 180.0f;

	/* 互補濾波 Complementary Filter */
#define CF_A 0.9f
#define CF_B 0.1f
	AngZ_Temp = AngZ_Temp + GyrZ * SampleRate;
	AngZ_Temp = CF_A * AngZ_Temp + CF_B * AngE.Yaw;

	if (AngZ_Temp > 360.0f)
		AngE.Yaw = AngZ_Temp - 360.0f;
	else if (AngZ_Temp < 0.0f)
		AngE.Yaw = AngZ_Temp + 360.0f;
	else
		AngE.Yaw = AngZ_Temp;
}
/*=====================================================================================================*/
/*=====================================================================================================*/
