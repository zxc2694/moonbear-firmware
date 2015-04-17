#ifndef __IMU_CORRECT_H
#define __IMU_CORRECT_H

#include "stm32f4_system.h"

void sensor_read();
void correct_sensor();
void AHRS_and_RC_updata(int16_t *Thr, int16_t *Pitch, int16_t *Roll, int16_t *Yaw, int16_t *safety);
#endif
