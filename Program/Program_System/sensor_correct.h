
#ifndef __SENSOR_CORRECT_H
#define __SENSOR_CORRECT_H
#include "stm32f4_system.h"
void correct_sensor(int16_t (*ACC_FIFO)[256], int16_t (*GYR_FIFO)[256], int16_t (*MAG_FIFO)[256],
	int16_t* MagDataX, int16_t* MagDataY, uint32_t Correction_Time);
#endif