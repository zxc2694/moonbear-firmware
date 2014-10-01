#include <stddef.h>
#include <stdlib.h>

#include "QuadCopterConfig.h"

void generate_package(IMU_package *package, uint8_t *buf)
{
	memcpy(&buf[0], &((package->roll)), sizeof(int16_t));
	memcpy(&buf[2], &((package->pitch)), sizeof(int16_t));
	memcpy(&buf[4], &((package->yaw)), sizeof(int16_t));
	memcpy(&buf[6], &((package->acc_x)), sizeof(int16_t));
	memcpy(&buf[8], &((package->acc_y)), sizeof(int16_t));
	memcpy(&buf[10], &((package->acc_z)), sizeof(int16_t));
	memcpy(&buf[12], &((package->gyro_x)), sizeof(int16_t));
	memcpy(&buf[14], &((package->gyro_y)), sizeof(int16_t));
	memcpy(&buf[16], &((package->gyro_z)), sizeof(int16_t));
}

void send_package(uint8_t *buf)
{
	int i = 0;

	while (buf[i] != '\0') {
		serial.putc(buf[i]);
		i++;
	}
}
