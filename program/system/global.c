#include "QuadCopterConfig.h"

global_t variable[] = {
	[PWM1_CCR] = {.name = "pwm1"},
	[PWM2_CCR] = {.name = "pwm2"},
	[PWM3_CCR] = {.name = "pwm3"},
	[PWM4_CCR] = {.name = "pwm4"},
	[PWM5_CCR] = {.name = "pwm5"},

	[MOTOR1] = {.name = "motor1"},
	[MOTOR2] = {.name = "motor2"},
	[MOTOR3] = {.name = "motor3"},
	[MOTOR4] = {.name = "motor4"},

	[NO_RC_SIGNAL_MSG] = {.name = "rc.status"},

	[RC_EXP_PITCH] = {.name = "rc.pitch"},
	[RC_EXP_ROLL] = {.name = "rc.roll"},
	[RC_EXP_YAW] = {.name = "rc.yaw"},
	[RC_EXP_THR] = {.name = "rc.throttle"},

	[TRUE_PITCH] = {.name = "imu.pitch"},
	[TRUE_ROLL] = {.name = "imu.roll"},
	[TRUE_YAW] = {.name = "imu.yaw"},

	[PID_PITCH] = {.name = "pid.pitch"},
	[PID_ROLL] = {.name = "pid.roll"},
	[PID_YAW] = {.name = "pid.yaw"}
};

system_t system = {
	.variable = variable,
	.var_count = SYS_VAR_CNT,
	.status = SYSTEM_UNINITIALIZED
};

status_t SD_status;

xSemaphoreHandle serial_tx_wait_sem = NULL;
xQueueHandle serial_rx_queue = NULL;
