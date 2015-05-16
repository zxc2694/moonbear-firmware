#ifndef __IMU_CORRECT_H
#define __IMU_CORRECT_H

#include "stm32f4_system.h"

#define CONTROLLER_DISABLE 0
#define CONTROLLER_ENABLE 1

typedef struct vertical_pid_t {
	float kp,ki,kd;
	float integral;
	float setpoint;
	float output;
	float out_max, out_min;
	float integral_max;
	uint8_t controller_status;

} vertical_pid_t;

void sensor_read();
void correct_sensor();
void AHRS_and_RC_updata(int16_t *Thr, int16_t *Pitch, int16_t *Roll, int16_t *Yaw, int16_t *safety);
void PID_vertical_Zd(vertical_pid_t* , float *vertical_data);
#endif
