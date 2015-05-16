// IMU校正
//可參考https://github.com/CYACAcademic/BalancedCar

#include "QuadCopterConfig.h"

#define Kp 15.0f
#define Ki 0.020f//0.02f

void AHRS_Init(Quaternion *pNumQ, EulerAngle *pAngE)
{
	pNumQ->q0 = 1.0f;
	pNumQ->q1 = 0.0f;
	pNumQ->q2 = 0.0f;
	pNumQ->q3 = 0.0f;

	pAngE->Pitch = 0.0f;
	pAngE->Roll  = 0.0f;
	pAngE->Yaw   = 0.0f;
}

void AHRS_Update(void)
{
	float tempX = 0, tempY = 0;
	float Normalize;
	float gx, gy, gz;
// float hx, hy, hz;
// float wx, wy, wz;
// float bx, bz;
	float ErrX, ErrY, ErrZ;
	float AccX, AccY, AccZ;
	float GyrX, GyrY, GyrZ;
// float MegX, MegY, MegZ;
	float /*Mq11, Mq12, */Mq13,/* Mq21, Mq22, */Mq23,/* Mq31, Mq32, */Mq33;

	static float AngZ_Temp = 0.0f;
	static float exInt = 0.0f, eyInt = 0.0f, ezInt = 0.0f;
	static int initYaw = 0, set=0;

	//   Mq11 = NumQ.q0*NumQ.q0 + NumQ.q1*NumQ.q1 - NumQ.q2*NumQ.q2 - NumQ.q3*NumQ.q3;
	//   Mq12 = 2.0f*(NumQ.q1*NumQ.q2 + NumQ.q0*NumQ.q3);
	Mq13 = 2.0f * (NumQ.q1 * NumQ.q3 - NumQ.q0 * NumQ.q2);
	//   Mq21 = 2.0f*(NumQ.q1*NumQ.q2 - NumQ.q0*NumQ.q3);
	//   Mq22 = NumQ.q0*NumQ.q0 - NumQ.q1*NumQ.q1 + NumQ.q2*NumQ.q2 - NumQ.q3*NumQ.q3;
	Mq23 = 2.0f * (NumQ.q0 * NumQ.q1 + NumQ.q2 * NumQ.q3);
	//   Mq31 = 2.0f*(NumQ.q0*NumQ.q2 + NumQ.q1*NumQ.q3);
	//   Mq32 = 2.0f*(NumQ.q2*NumQ.q3 - NumQ.q0*NumQ.q1);
	Mq33 = NumQ.q0 * NumQ.q0 - NumQ.q1 * NumQ.q1 - NumQ.q2 * NumQ.q2 + NumQ.q3 * NumQ.q3;
	//計算四元數M旋轉矩陣之元素

	Normalize = invSqrtf(squa(Acc.TrueX) + squa(Acc.TrueY) + squa(Acc.TrueZ));
	//產生正規化參數
	AccX = Acc.TrueX * Normalize;
	AccY = Acc.TrueY * Normalize;
	AccZ = Acc.TrueZ * Normalize;

// Normalize = invSqrtf(squa(Meg.TrueX) + squa(Meg.TrueY) + squa(Meg.TrueZ));
// MegX = Meg.TrueX*Normalize;
// MegY = Meg.TrueY*Normalize;
// MegZ = Meg.TrueZ*Normalize;

	gx = Mq13;
	gy = Mq23;
	gz = Mq33;

// hx = MegX*Mq11 + MegY*Mq21 + MegZ*Mq31;
// hy = MegX*Mq12 + MegY*Mq22 + MegZ*Mq32;
// hz = MegX*Mq13 + MegY*Mq23 + MegZ*Mq33;

// bx = sqrtf(squa(hx) + squa(hy));
// bz = hz;

// wx = bx*Mq11 + bz*Mq13;
// wy = bx*Mq21 + bz*Mq23;
// wz = bx*Mq31 + bz*Mq33;

	ErrX = (AccY * gz - AccZ * gy)/* + (MegY*wz - MegZ*wy)*/;
	ErrY = (AccZ * gx - AccX * gz)/* + (MegZ*wx - MegX*wz)*/;
	ErrZ = (AccX * gy - AccY * gx)/* + (MegX*wy - MegY*wx)*/;

	exInt = exInt + ErrX * Ki;
	eyInt = eyInt + ErrY * Ki;
	ezInt = ezInt + ErrZ * Ki;

	GyrX = toRad(Gyr.TrueX);
	GyrY = toRad(Gyr.TrueY);
	GyrZ = toRad(Gyr.TrueZ);

	GyrX = GyrX + Kp * ErrX + exInt;
	GyrY = GyrY + Kp * ErrY + eyInt;
	GyrZ = GyrZ + Kp * ErrZ + ezInt;
	//感測器之PI小回授計算

	Quaternion_RungeKutta(&NumQ, GyrX, GyrY, GyrZ, SampleRateHelf);
	//使用RungeKutta法更新四元數
	Quaternion_Normalize(&NumQ);
	Quaternion_ToAngE(&NumQ, &AngE);

	AngE.Pitch = toDeg(AngE.Pitch);
	AngE.Roll  = toDeg(AngE.Roll);

	while ((AngE.Yaw != 0) && (set < 16)){
		if (set ==15){
			initYaw = toDeg(atan2f(Mag.TrueX, Mag.TrueY));				
		}
		set++;		
	}
	AngE.Yaw  = -(toDeg(atan2f(Mag.TrueX, Mag.TrueY)) - initYaw); //加入負號，讓Yaw的旋轉角度與與IMU一致


	/* 互補濾波 Complementary Filter */
#define CF_A 0.1f
#define CF_B 0.9f
	AngZ_Temp = AngZ_Temp + GyrZ * SampleRate;
	AngZ_Temp = CF_A * AngZ_Temp + CF_B * AngE.Yaw;
	if (AngZ_Temp > 360.0f)
		AngE.Yaw = AngZ_Temp - 360.0f;
	else if (AngZ_Temp < 0.0f)
		AngE.Yaw = AngZ_Temp + 360.0f;
	else
		AngE.Yaw = AngZ_Temp;
}

