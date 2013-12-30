/* Standard C lib includes */
#include <stddef.h>
#include <string.h>

/* Linenoise and shell includes */
#include "shell.h"
#include "linenoise.h"
#include "parser.h"

#include "module_rs232.h"
#include "algorithm_quaternion.h"
#include "sys_manager.h"

#include "FreeRTOS.h"
#include "task.h"

int completion_disable = 0;
int history_disable = 0;

/* Shell Command handlers */
void unknown_cmd(char parameter[][MAX_CMD_LEN], int par_cnt);
void clear(char parameter[][MAX_CMD_LEN], int par_cnt);
void monitor(char parameter[][MAX_CMD_LEN], int par_cnt);
void ps(char parameter[][MAX_CMD_LEN], int par_cnt);

/* The identifier of the command */
enum SHELL_CMD_ID {
        unknown_cmd_ID,
	clear_ID,
        monitor_ID,
	ps_ID,
        SHELL_CMD_CNT
};

//First string don't need to store anything for unknown commands
command_list shellCmd_list[SHELL_CMD_CNT] = {
	CMD_DEF(unknown_cmd),
	CMD_DEF(clear),
	CMD_DEF(monitor),
	CMD_DEF(ps)
};

/**** Shell task **********************************************************************/
void shell_linenoise_completion(const char *buf, linenoiseCompletions *lc) {
	if(completion_disable)
		return;

	int i; //i = 1 to ignore the "UNKNOWN_COMMAND" string
	for(i = 1;i < SHELL_CMD_CNT;i++) {
		if(buf[0] == shellCmd_list[i].str[0])
			linenoiseAddCompletion(lc, shellCmd_list[i].str);
	}
}

void shell_task()
{
        linenoiseSetCompletionCallback(shell_linenoise_completion);

        /* Clear the screen */
        printf("\x1b[H\x1b[2J");
	/* Show the prompt messages */
        printf("[System status]Initialized successfully!\n\r");
        printf("Please type \"help\" to get more informations\n\r");

        while(1) {
		command_data shell_cd = {.par_cnt = 0};

	        char *shell_str = linenoise("Quadcopter Shell > ");
                commandExec(shell_str, &shell_cd, shellCmd_list, SHELL_CMD_CNT);

		linenoiseHistoryAdd(shell_str);
        }
}
/**** Customize command function ******************************************************/
void unknown_cmd(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	printf("Command not found\n\r");
}

void clear(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	linenoiseClearScreen();
}

void ps(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	signed char buf[256] = {'\0'};

	vTaskList(buf);

	//TODO:replace the hardcode by using sprintf()
	printf("Name          State   Priority  Stack Num\n\r");
	printf("*****************************************\n\r");
	printf("%s", buf);
}


enum MONITOR_INTERNAL_CMD {
	MONITOR_UNKNOWN,
	MONITOR_QUIT,
	MONITOR_RESUME,
	MONITOR_CMD_CNT
};

char monitor_cmd[MONITOR_CMD_CNT - 1][MAX_CMD_LEN] = {"quit", "resume"};

int monitorInternalCmdIndentify(char *command)
{
	int i;
	for(i = 0; i < (MONITOR_CMD_CNT - 1); i++) {
		if(strcmp(command ,monitor_cmd[i]) == 0) {
			return i + 1;
		}
	}

	return MONITOR_UNKNOWN;
}

void monitor(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	while(1) {
		/* Clean the screen */
		printf("\x1b[H\x1b[2J");

		/* Welcome Messages */
		printf("QuadCopter Status Monitor\n\r");
		printf("Copyleft - NCKU Open Source Work of 2013 Embedded system class\n\r");
		printf("**************************************************************\n\r");

		printf("PID Parameters\n\r");
		printf("Kp \t: %d\n\rKi\t: %d\n\rKd\t: %d\n\r", 0, 0, 0);

		printf("--------------------------------------------------------------\n\r");

		printf("Copter Attitudes <true value>\n\r");
		printf("Pitch\t: %d\n\rRoll\t: %d\n\rYaw\t: %d\n\r", AngE.Pitch, AngE.Roll, AngE.Yaw);

		printf("--------------------------------------------------------------\n\r");

		#define MOTOR_STATUS "Off"
		printf("Radio Control Messages\n\r");
		printf("Pitch(expect)\t: %d\n\rRoll(expect)\t: %d\n\rYaw(expect)\t: %d\n\r", 
			global_var[RC_EXP_PITCH].param, global_var[RC_EXP_ROLL].param, 
			global_var[RC_EXP_YAW].param);

		printf("Throttle\t: %d\n\r", global_var[RC_EXP_THR].param);
		printf("Engine\t\t: %s\n\r", MOTOR_STATUS);

		printf("--------------------------------------------------------------\n\r");

		#define LED_STATUS "Off"
		printf("LED lights\n\r");
		printf("LED1\t: %s\n\rLED4\t: %s\n\rLED3\t: %s\n\rLED4\t: %s\n\r", LED_STATUS, LED_STATUS, LED_STATUS, LED_STATUS);

		printf("**************************************************************\n\r\n\r");

		printf("[Please press <Space> to refresh the status]\n\r");
		printf("[Please press <Enter> to enable the command line]");
	
		int monitor_cmd = 0;		
		char key_pressed = serial.getch();

		while(monitor_cmd != MONITOR_QUIT && monitor_cmd != MONITOR_RESUME) {
			if(key_pressed == ENTER) {
				/* Clean and move up two lines*/
				printf("\x1b[0G\x1b[0K\x1b[0A\x1b[0G\x1b[0K");
	
				while(monitor_cmd != MONITOR_QUIT && monitor_cmd != MONITOR_RESUME) {
					char *command_str;

					command_str = linenoise("(monitor) ");
				
					/* Check if it is internal command */
					/* The Internal command means that need not call
					   any functions but handle at here
					 */
					monitor_cmd = monitorInternalCmdIndentify(command_str);

					/* Check if it is not an Internal command */
					if(monitor_cmd == MONITOR_UNKNOWN) {
						/* FIXME:External Commands, need to call other functions */
					}

					printf("\x1b[0A");
				}
			} else if(key_pressed == SPACE) {
				break;
			} else {
				key_pressed = serial.getch();
			}			
		}

		if(monitor_cmd == MONITOR_QUIT)
			break;
	}

	/* Clean the screen */
        printf("\x1b[H\x1b[2J");
}


