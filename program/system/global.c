#include "QuadCopterConfig.h"

global_struct global_var[] = {
	[PWM1_CCR] = {.param_name = "PWM1", .param = 0},
	[PWM2_CCR] = {.param_name = "PWM2", .param = 0},
	[PWM3_CCR] = {.param_name = "PWM3", .param = 0},
	[PWM4_CCR] = {.param_name = "PWM4", .param = 0},
	[PWM5_CCR] = {.param_name = "PWM5", .param = 0},

	[RC_EXP_PITCH] = {.param_name = "RC.pitch", .param = 0},
	[RC_EXP_ROLL] = {.param_name = "RC.roll", .param = 0},
	[RC_EXP_YAW] = {.param_name = "RC.yaw", .param = 0},
	[RC_EXP_THR] = {.param_name = "RC.throttle", .param = 0},

	[TRUE_PITCH] = {.param_name = "IMU.yaw", .param = 0},
	[TRUE_ROLL] = {.param_name = "IMU.roll", .param = 0},
	[TRUE_YAW] = {.param_name = "IMU.yaw", .param = 0},

	[PID_ROLL] = {.param_name = "PID.roll", .param = 0},
	[PID_PITCH] = {.param_name = "PID.pitch", .param = 0},
	[PID_YAW] = {.param_name = "PID.yaw", .param = 0},

	[NO_RC_SIGNAL_MSG] = {.param_name = "RC.status", .param = 0},

	[MOTOR1] = {.param_name = "motor1", .param = 0},
	[MOTOR2] = {.param_name = "motor2", .param = 0},
	[MOTOR3] = {.param_name = "motor3", .param = 0},
	[MOTOR4] = {.param_name = "motor4", .param = 0}
};

SYSTEM_STATUS sys_status = SYSTEM_UNINITIALIZED;
SD_STATUS SD_status;

xSemaphoreHandle serial_tx_wait_sem = NULL;
xQueueHandle serial_rx_queue = NULL;
