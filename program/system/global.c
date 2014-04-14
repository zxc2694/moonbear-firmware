#include "QuadCopterConfig.h"

global_struct global_var[] = {
	[PWM1_CCR] = {.name = "PWM1", .value = 0},
	[PWM2_CCR] = {.name = "PWM2", .value = 0},
	[PWM3_CCR] = {.name = "PWM3", .value = 0},
	[PWM4_CCR] = {.name = "PWM4", .value = 0},
	[PWM5_CCR] = {.name = "PWM5", .value = 0},

	[RC_EXP_PITCH] = {.name = "RC.pitch", .value = 0},
	[RC_EXP_ROLL] = {.name = "RC.roll", .value = 0},
	[RC_EXP_YAW] = {.name = "RC.yaw", .value = 0},
	[RC_EXP_THR] = {.name = "RC.throttle", .value = 0},

	[TRUE_PITCH] = {.name = "IMU.yaw", .value = 0},
	[TRUE_ROLL] = {.name = "IMU.roll", .value = 0},
	[TRUE_YAW] = {.name = "IMU.yaw", .value = 0},

	[PID_ROLL] = {.name = "PID.roll", .value = 0},
	[PID_PITCH] = {.name = "PID.pitch", .value = 0},
	[PID_YAW] = {.name = "PID.yaw", .value = 0},

	[NO_RC_SIGNAL_MSG] = {.name = "RC.status", .value = 0},

	[MOTOR1] = {.name = "motor1", .value = 0},
	[MOTOR2] = {.name = "motor2", .value = 0},
	[MOTOR3] = {.name = "motor3", .value = 0},
	[MOTOR4] = {.name = "motor4", .value = 0}
};

SYSTEM_STATUS sys_status = SYSTEM_UNINITIALIZED;
SD_STATUS SD_status;

xSemaphoreHandle serial_tx_wait_sem = NULL;
xQueueHandle serial_rx_queue = NULL;
