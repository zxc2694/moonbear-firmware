#include "QuadCopterConfig.h"

global_struct global_var[] = {
	[PWM1_CCR] = {.param_name = "PWM1_CCR", .param = 0},
	[PWM2_CCR] = {.param_name = "PWM2_CCR", .param = 0},
	[PWM3_CCR] = {.param_name = "PWM3_CCR", .param = 0},
	[PWM4_CCR] = {.param_name = "PWM4_CCR", .param = 0},
	[PWM5_CCR] = {.param_name = "PWM5_CCR", .param = 0},
	[RC_EXP_ROLL] = {.param_name = "RC Expect Roll", .param = 0},
	[RC_EXP_PITCH] = {.param_name = "RC Expect Pitch", .param = 0},
	[RC_EXP_YAW] = {.param_name = "RC Expect YAW", .param = 0},
	[RC_EXP_THR] = {.param_name = "RC Expect THROTTLE", .param = 0},
	[TRUE_ROLL] = {.param_name = "IMU ROLL", .param = 0},
	[TRUE_PITCH] = {.param_name = "IMU PITCH", .param = 0},
	[TRUE_YAW] = {.param_name = "IMU YAW", .param = 0},
	[NO_RC_SIGNAL_MSG] = {.param_name = "Show current RC control signal status", .param = 0},
	[OPERATE_ROLL] = {.param_name = "Show current PID ROLL output", .param = 0},
	[OPERATE_PITCH] = {.param_name = "Show current PID PITCH output", .param = 0},
	[OPERATE_YAW] = {.param_name = "Show current PID YAW output", .param = 0},
	[OPERATE_THR] = {.param_name = "Show current PID THR output", .param = 0},
	[MOTOR1] = {.param_name = "Show current Motor 1 CCR", .param = 0},
	[MOTOR2] = {.param_name = "Show current Motor 2 CCR", .param = 0},
	[MOTOR3] = {.param_name = "Show current Motor 3 CCR", .param = 0},
	[MOTOR4] = {.param_name = "Show current Motor 4 CCR", .param = 0},
	[test1] = {.param_name = "test", .param = 0},
	[test2] = {.param_name = "test", .param = 0},
	[test3] = {.param_name = "test", .param = 0},
	[test4] = {.param_name = "test", .param = 0},
	[Write_PWM_Motor1]= {.param_name = "Write_PWM_Motor1", .param = 0},
	[Write_PWM_Motor2]= {.param_name = "Write_PWM_Motor2", .param = 0},
	[Write_PWM_Motor3]= {.param_name = "Write_PWM_Motor3", .param = 0},
	[Write_PWM_Motor4]= {.param_name = "Write_PWM_Motor4", .param = 0}

};
SYSTEM_STATUS sys_status = SYSTEM_UNINITIALIZED;
xSemaphoreHandle serial_tx_wait_sem = NULL;
xQueueHandle serial_rx_queue = NULL;
SYSTEM_STATUS set_PWM_Motors = SYSTEM_UNINITIALIZED;