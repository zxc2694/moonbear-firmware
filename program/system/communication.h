#ifndef __COMMUNICATION_H
#define __COMMUNICATION_H

#include <stddef.h>
#include <stdlib.h>

typedef struct {
	int16_t roll, pitch, yaw;
	int16_t acc_x, acc_y, acc_z;
	int16_t gyro_x, gyro_y, gyro_z;
} IMU_package;

void generate_package(IMU_package *package, uint8_t *buf);
void send_package(uint8_t *buf);

#endif
