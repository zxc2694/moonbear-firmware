/* Standard C lib includes */
#include <stddef.h>
#include <string.h>

/* Linenoise and shell includes */
#include "shell.h"
#include "linenoise.h"
#include "parser.h"

#include "module_rs232.h"
#include "algorithm_quaternion.h"
#include "algorithm_pid.h"
#include "sys_manager.h"

#include "FreeRTOS.h"
#include "task.h"

/* Shell Command handlers */
void monitor_unknown_cmd(char parameter[][MAX_CMD_LEN], int par_cnt);
void monitor_help(char parameter[][MAX_CMD_LEN], int par_cnt);
void monitor_set(char parameter[][MAX_CMD_LEN], int par_cnt);

/* The identifier of the command */
enum MONITOR_CMD_ID {
	unknown_cmd_ID,
	help_ID,
	set_ID,
	MONITOR_CMD_CNT
};

//First string don't need to store anything for unknown commands
command_list monitorCmd_list[MONITOR_CMD_CNT] = {
	CMD_DEF(unknown_cmd, monitor),
	CMD_DEF(help, monitor),
	CMD_DEF(set, monitor)
};

/* Internal commands */
enum MONITOR_INTERNAL_CMD {
	MONITOR_UNKNOWN,
	MONITOR_QUIT,
	MONITOR_RESUME,
	MONITOR_IT_CMD_CNT
};

char monitor_cmd[MONITOR_IT_CMD_CNT - 1][MAX_CMD_LEN] = {"quit", "resume"};
int monitor_it_cmd;


int monitorInternalCmdIndentify(char *command)
{
	int i;

	for (i = 0; i < (MONITOR_IT_CMD_CNT - 1); i++) {
		if (strcmp(command , monitor_cmd[i]) == 0) {
			return i + 1;
		}
	}

	return MONITOR_UNKNOWN;
}

void shell_monitor(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	int last_rc_exp_pitch = global_var[RC_EXP_PITCH].param;
	int last_rc_exp_roll = global_var[RC_EXP_ROLL].param;
	int last_rc_exp_yaw = global_var[RC_EXP_YAW].param;

	while (1) {
		/* Clean the screen */
		printf("\x1b[H\x1b[2J");

		/* Welcome Messages */
		printf("QuadCopter Status Monitor\n\r");
		printf("Copyleft - NCKU Open Source Work of 2013 Embedded system class\n\r");
		printf("Please type \"help\" to get more informations\n\r");
		printf("**************************************************************\n\r");

		printf("PID\tPitch\tRoll\tYow\n\r");
		printf("Kp\t%f\t%f\t%f\n\r", PID_Pitch.Kp, PID_Roll.Kp, PID_Yaw.Kp);
		printf("Ki\t%f\t%f\t%f\n\r", PID_Pitch.Ki, PID_Roll.Ki, PID_Yaw.Ki);
		printf("Kd\t%f\t%f\t%f\n\r", PID_Pitch.Kd, PID_Roll.Kd, PID_Yaw.Kd);

		printf("--------------------------------------------------------------\n\r");

		printf("Copter Attitudes <true value>\n\r");
		printf("Pitch\t: %d\n\rRoll\t: %d\n\rYaw\t: %d\n\r", AngE.Pitch, AngE.Roll, AngE.Yaw);

		printf("--------------------------------------------------------------\n\r");

#define MOTOR_STATUS "Off"
		printf("RC Messages\tCurrent\tLast\n\r");
		printf("Pitch(expect)\t%d\t%d\n\r", global_var[RC_EXP_PITCH].param, last_rc_exp_pitch);
		printf("Roll(expect)\t%d\t%d\n\r", global_var[RC_EXP_ROLL].param, last_rc_exp_roll);
		printf("Yaw(expect)\t%d\t%d\n\r", global_var[RC_EXP_YAW].param, last_rc_exp_yaw);

		printf("Throttle\t%d\n\r", global_var[RC_EXP_THR].param);
		printf("Engine\t\t%s\n\r", MOTOR_STATUS);

		printf("--------------------------------------------------------------\n\r");

#define LED_STATUS "Off"
		printf("LED lights\n\r");
		printf("LED1\t: %s\n\rLED2\t: %s\n\rLED3\t: %s\n\rLED4\t: %s\n\r", LED_STATUS, LED_STATUS, LED_STATUS, LED_STATUS);

		printf("**************************************************************\n\r\n\r");

		printf("[Please press <Space> to refresh the status]\n\r");
		printf("[Please press <Enter> to enable the command line]");

		char key_pressed = serial.getch();
		monitor_it_cmd = 0;

		while (monitor_it_cmd != MONITOR_QUIT && monitor_it_cmd != MONITOR_RESUME) {
			if (key_pressed == ENTER) {
				/* Clean and move up two lines*/
				printf("\x1b[0G\x1b[0K\x1b[0A\x1b[0G\x1b[0K");

				while (monitor_it_cmd != MONITOR_QUIT && monitor_it_cmd != MONITOR_RESUME) {
					char *command_str;

					command_str = linenoise("(monitor) ");

					/* Check if it is internal command */
					/* The Internal command means that need not call
					   any functions but handle at here
					 */
					monitor_it_cmd = monitorInternalCmdIndentify(command_str);

					/* Check if it is not an Internal command */
					if (monitor_it_cmd == MONITOR_UNKNOWN) {
						/* FIXME:External Commands, need to call other functions */
						command_data monitor_cd = {.par_cnt = 0};
						commandExec(command_str, &monitor_cd, monitorCmd_list, MONITOR_CMD_CNT);

					} else {
						printf("\x1b[0A");
					}
				}

			} else if (key_pressed == SPACE) {
				break;

			} else {
				key_pressed = serial.getch();
			}
		}

		/* Exit the moniter if user type command "quit" */
		if (monitor_it_cmd == MONITOR_QUIT)
			break;

		/* Update the record of RC expect attitudes */
		last_rc_exp_pitch = global_var[RC_EXP_PITCH].param;
		last_rc_exp_roll = global_var[RC_EXP_ROLL].param;
		last_rc_exp_yaw = global_var[RC_EXP_YAW].param;

	}

	/* Clean the screen */
	printf("\x1b[H\x1b[2J");
}

/**** Customize command function ******************************************************/
void monitor_unknown_cmd(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	printf("\x1b[0A\x1b[0G\x1b[0K");
	printf("[Unknown command:Please press any key to resume...]");

	serial.getch();
	printf("\x1b[0G\x1b[0K");
}

void monitor_help(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	printf("\x1b[H\x1b[2J");
	printf("QuadCopter Status Monitor Manual\n\r");
	printf("****************************************************************************************\n\r");

	printf("\n\rDiscription:\n\r");
	printf("The monitor support reporting and setting the information of the QuadCopter in real time\n\r");
	printf("\n\r----------------------------------------------------------------------------------------\n\r");

	printf("*To refresh the status, please press [Space]\n\r");
	printf("*To modify the settings, please press [Enter] to enable the commandline\n\r");
	printf("----------------------------------------------------------------------------------------\n\r");

	printf("\n\rAll Commands:\n\r");

	printf("\n\rset [parameter] [value] / set update\n\r");
	printf("-Set the parameters of the QuadCopter(*The settings will change after type \"set update\")\n\r");

	printf("\n\r----------------------------------------------------------------------------------------\n\r");
	printf("Modifiable parameter list:\n\r\n\r");
	printf("pitch.kp  pitch.ki  pitch.kd\n\r");
	printf("roll.kp   roll.ki   roll.kd\n\r");
	printf("yaw.kp    yaw.ki    yaw.kd\n\r");
	printf("LED1  LED2\n\r");
	printf("LED3  LED4\n\r");
	printf("----------------------------------------------------------------------------------------\n\r");

	printf("\n\rresume\n\r");
	printf("-Disable the commandline and resume to status report mode\n\r");

	printf("\n\rquit\n\r");
	printf("-Quit the QuadCopter Status Monitor\n\r");

	printf("\n\r****************************************************************************************\n\r");

	printf("\n\r[Please press q to quit the manual]");

	/* Exit */
	char ch = serial.getch();

	while (ch != 'q' && ch != 'Q')
		ch = serial.getch();

	monitor_it_cmd = MONITOR_RESUME;
}

void monitor_set(char parameter[][MAX_CMD_LEN], int par_cnt)
{
}
