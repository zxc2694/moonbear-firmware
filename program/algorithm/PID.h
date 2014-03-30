#ifndef __PID_H
#define __PID_H

#include "stm32f4xx.h"

typedef struct {
	float Kp;
	float Ki;
	float Kd;
	float Err0;
	float Err1;
	float Err2;
	float SumErr;
	float ZeroErr;
	float Output;
	float KiMax;
	float KiMin;
	float OutMax;
	float OutMin;
} PID_t;

extern PID_t PID_Yaw;
extern PID_t PID_Roll;
extern PID_t PID_Pitch;

void PID_Init(PID_t *PID, float Kp, float Ki, float Kd);

float PID_IncCal(PID_t *, float);
float PID_PosCal(PID_t *, float);
float PID_AHRS_Cal(PID_t *, float, float);
float PID_AHRS_CalYaw(PID_t *PID, float Angle, float Gyroscope);

#endif
