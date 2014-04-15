#include "QuadCopterConfig.h"

global_t variable[] = {
	[PWM1_CCR] = {.name = "PWM1"},
	[PWM2_CCR] = {.name = "PWM2"},
	[PWM3_CCR] = {.name = "PWM3"},
	[PWM4_CCR] = {.name = "PWM4"},
	[PWM5_CCR] = {.name = "PWM5"},

	[MOTOR1] = {.name = "motor1"},
	[MOTOR2] = {.name = "motor2"},
	[MOTOR3] = {.name = "motor3"},
	[MOTOR4] = {.name = "motor4"},

	[NO_RC_SIGNAL_MSG] = {.name = "RC.status"},

	[RC_EXP_PITCH] = {.name = "RC.pitch"},
	[RC_EXP_ROLL] = {.name = "RC.roll"},
	[RC_EXP_YAW] = {.name = "RC.yaw"},
	[RC_EXP_THR] = {.name = "RC.throttle"},

	[TRUE_PITCH] = {.name = "IMU.yaw"},
	[TRUE_ROLL] = {.name = "IMU.roll"},
	[TRUE_YAW] = {.name = "IMU.yaw"},

	[PID_PITCH] = {.name = "PID.pitch"},
	[PID_YAW] = {.name = "PID.yaw"}
};

system_t system = {
	.variable = variable,
	.status = SYSTEM_UNINITIALIZED
};

status_t SD_status;

xSemaphoreHandle serial_tx_wait_sem = NULL;
xQueueHandle serial_rx_queue = NULL;
