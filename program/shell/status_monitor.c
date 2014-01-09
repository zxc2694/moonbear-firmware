/* Standard C lib includes */
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include <string.h>

#include "QuadCopterConfig.h"

#define CMD_EXECUTED 0
#define CMD_UNEXECUTED 1

#define PAR_DEF(PAR, PAR_STR, INT_ORG, FLT_ORG) \
	[PAR] = {.par_str = #PAR_STR, .int_origin = INT_ORG, .flt_origin = FLT_ORG, .par_is_changed = 0}

/* Shell Command handlers */
void monitor_unknown_cmd(char parameter[][MAX_CMD_LEN], int par_cnt);
void monitor_help(char parameter[][MAX_CMD_LEN], int par_cnt);
void monitor_set(char parameter[][MAX_CMD_LEN], int par_cnt);
void monitor_reset(char parameter[][MAX_CMD_LEN], int par_cnt);

/* The identifier of the command */
enum MONITOR_CMD_ID {
	unknown_cmd_ID,
	help_ID,
	set_ID,
	reset_ID,
	MONITOR_CMD_CNT
};

//First string don't need to store anything for unknown commands
command_list monitorCmd_list[MONITOR_CMD_CNT] = {
	CMD_DEF(unknown_cmd, monitor),
	CMD_DEF(help, monitor),
	CMD_DEF(set, monitor),
	CMD_DEF(reset, monitor)
};

/* Internal commands */
enum MONITOR_INTERNAL_CMD {
	MONITOR_UNKNOWN,
	MONITOR_QUIT,
	MONITOR_RESUME,
	MONITOR_IT_CMD_CNT
};

/* The buffer of the settings */
enum PAR_SETTING {
	PITCH_KP,
	PITCH_KI,
	PITCH_KD,
	ROLL_KP,
	ROLL_KI,
	ROLL_KD,
	YAW_KP,
	YAW_KI,
	YAW_KD,
	PARAMETER_CNT
};

typedef struct {
	char par_str[MAX_PAR_CNT];
	int *int_origin;
	float *flt_origin;
	int int_buf;
	float flt_buf;
	int par_is_changed; //The flag shows that the settings is changed or not
} parameter_data;

parameter_data par_data[PARAMETER_CNT] = {
	PAR_DEF(PITCH_KP, pitch.kp, 0, &(PID_Pitch.Kp)),
	PAR_DEF(PITCH_KI, pitch.ki, 0, &(PID_Pitch.Ki)),
	PAR_DEF(PITCH_KD, pitch.kd, 0, &(PID_Pitch.Kd)),
	PAR_DEF(ROLL_KP, roll.kp, 0, &(PID_Roll.Kp)),
	PAR_DEF(ROLL_KI, roll.ki, 0, &(PID_Roll.Ki)),
	PAR_DEF(ROLL_KD, roll.kd, 0, &(PID_Roll.Kd)),
	PAR_DEF(YAW_KP, yaw.kp, 0, &(PID_Yaw.Kp)),	
	PAR_DEF(YAW_KI, yaw.ki, 0, &(PID_Yaw.Ki)),
	PAR_DEF(YAW_KD, ywa.kd, 0, &(PID_Yaw.Kd))
};

/* Flags */
int par_is_changed = 0; //The flag shows that the settings is changed or not
int unsaved_print_cnt = 0;

/* Monitor Internal Command */
char monitor_cmd[MONITOR_IT_CMD_CNT - 1][MAX_CMD_LEN] = {"quit", "resume"};
int monitor_it_cmd;

int monitorInternalCmdIndentify(char *command)
{
	int i;
	for(i = 0; i < (MONITOR_IT_CMD_CNT - 1); i++) {
		if(strcmp(command ,monitor_cmd[i]) == 0) {
			return i + 1;
		}
	}

	return MONITOR_UNKNOWN;
}

void monitor_linenoise_completion(const char *buf, linenoiseCompletions *lc)
{
        int i,j ; //i = 1 to ignore the "UNKNOWN_COMMAND" string

        for (i = 1; i < MONITOR_CMD_CNT; i++) {
                if (buf[0] == monitorCmd_list[i].str[0])
                        linenoiseAddCompletion(lc, monitorCmd_list[i].str);
        }

	
	for(j = 0; j < MONITOR_IT_CMD_CNT; j++) {
		if (buf[0] == monitor_cmd[j][0])
			linenoiseAddCompletion(lc, monitor_cmd[j]);
	}
}

void clean_line(int line_cnt)
{
	int i;
	for(i = 0; i < line_cnt; i++) {
		printf("\x1b[0G\x1b[0K\x1b[0A");
	}
	printf("\x1b[0G\x1b[0K");
}

int print_unsaved_setting()
{
	if(par_is_changed == 1) {
		printf("Unsaved Settings (use \"set update\") to enable the settings)\n\r");

		int i, unsaved_cnt = 0;
		for(i = 0; i < PARAMETER_CNT; i++) {
			if(par_data[i].par_is_changed == 1) {
				if(par_data[i].int_origin == 0) {
					/* Data is a float */
					printf("%s: %f -> %f\n\r", par_data[i].par_str, *(par_data[i].flt_origin), par_data[i].flt_buf);
				} else {
					/* Data is a int */
					printf("%s: %d -> %f\n\r", par_data[i].par_str, *(par_data[i].int_origin), par_data[i].int_buf);
				}
				unsaved_cnt++;
			}	
		}


		printf("\n\r");
		return (unsaved_cnt + 2);
	}
	return 0;
}

void print_error_msg(char *error_msg)
{
	int i, new_line_cnt = 0;	
	for(i = 0; i < strlen(error_msg); i++) {
		if(error_msg[i] == '\n')
			new_line_cnt++;
	}

	printf("%s", error_msg);
	printf("[Please press any key to resume...]");

	serial.getch();
	clean_line(new_line_cnt + 1);
}

void shell_monitor(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	int last_rc_exp_pitch = global_var[RC_EXP_PITCH].param;
	int last_rc_exp_roll = global_var[RC_EXP_ROLL].param;
	int last_rc_exp_yaw = global_var[RC_EXP_YAW].param;

	while(1) {	
		linenoiseSetCompletionCallback(monitor_linenoise_completion);

		/* Clean the screen */
		printf("\x1b[H\x1b[2J");

		/* Welcome Messages */
		printf("QuadCopter Status Monitor\n\r");
		printf("Copyleft - NCKU Open Source Work of 2013 Embedded system class\n\r");
		printf("Please type \"help\" to get more informations\n\r");
		printf("**************************************************************\n\r");

		printf("PID\tPitch\tRoll\tYaw\n\r");
		printf("Kp\t%f\t%f\t%f\n\r", PID_Pitch.Kp, PID_Roll.Kp, PID_Yaw.Kp);
		printf("Ki\t%f\t%f\t%f\n\r", PID_Pitch.Ki, PID_Roll.Ki, PID_Yaw.Ki);
		printf("Kd\t%f\t%f\t%f\n\r", PID_Pitch.Kd, PID_Roll.Kd, PID_Yaw.Kd);

		printf("--------------------------------------------------------------\n\r");

		printf("Copter Attitudes <true value>\n\r");
		printf("Pitch\t: %f\n\rRoll\t: %f\n\rYaw\t: %f\n\r", AngE.Pitch, AngE.Roll, AngE.Yaw);

		printf("--------------------------------------------------------------\n\r");

		printf("RC Messages\tCurrent\tLast\n\r");
		printf("Pitch(expect)\t%f\t%f\n\r", global_var[RC_EXP_PITCH].param, last_rc_exp_pitch);
		printf("Roll(expect)\t%f\t%f\n\r", global_var[RC_EXP_ROLL].param, last_rc_exp_roll);
		printf("Yaw(expect)\t%f\t%f\n\r", global_var[RC_EXP_YAW].param, last_rc_exp_yaw);	

		printf("Throttle\t%d\n\r", global_var[RC_EXP_THR].param);

 		if(global_var[PWM5_CCR].param > (MAX_PWM5_INPUT + MIN_PWM5_INPUT) / 2)
			printf("Engine\t\t%s\n\r", "Off");
		else
			printf("Engine\t\t%s\n\r", "On");

		printf("**************************************************************\n\r\n\r");

		if(par_is_changed) {
			print_unsaved_setting();
		}

		vTaskDelay(250);
		printf("[Please press <Space> to refresh the status]\n\r");
		printf("[Please press <Enter> to enable the command line]");
	
		char key_pressed = serial.getch();
		monitor_it_cmd = 0;		
	
		while(monitor_it_cmd != MONITOR_QUIT && monitor_it_cmd != MONITOR_RESUME) {
			if(key_pressed == ENTER) {
				/* Clean and move up two lines*/
				clean_line(1);
	
				while(monitor_it_cmd != MONITOR_QUIT && monitor_it_cmd != MONITOR_RESUME) {
					char *command_str;

					command_str = linenoise("(monitor) ");
					
					/* Ctrl^C */
					if(command_str == NULL) { 
						clean_line(1);
						continue;
					}				

					/* Check if it is internal command */
					/* The Internal command means that need not call
					   any functions but handle at here
					 */
					monitor_it_cmd = monitorInternalCmdIndentify(command_str);

					/* Check if it is not an Internal command */
					if(monitor_it_cmd == MONITOR_UNKNOWN) {
						/* FIXME:External Commands, need to call other functions */
						command_data monitor_cd = {.par_cnt = 0};			
						commandExec(command_str, &monitor_cd, monitorCmd_list, MONITOR_CMD_CNT);
					} else {
						printf("\x1b[0A");
					}
				}
			} else if(key_pressed == SPACE) {
				break;
			} else {
				key_pressed = serial.getch();
			}			
		}

		/* Exit the moniter if user type command "quit" */
		if(monitor_it_cmd == MONITOR_QUIT)
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
	printf("******************************************************************************************\n\r");	

	printf("\n\rDiscription:\n\r");
	printf("The monitor support reporting and setting the information of the QuadCopter in real time\n\r");
	printf("\n\r------------------------------------------------------------------------------------------\n\r");

	printf("*To refresh the status, please press [Space]\n\r");
	printf("*To modify the settings, please press [Enter] to enable the commandline\n\r");
	printf("------------------------------------------------------------------------------------------\n\r");

	printf("\n\rAll Commands:\n\r");

	printf("\n\rset [parameter] [value] / set update\n\r");
	printf("-Set the parameters of the QuadCopter(*The settings will change after typing \"set update\")\n\r");

	printf("\n\r------------------------------------------------------------------------------------------\n\r");
	printf("Modifiable parameter list:\n\r\n\r");
	printf("pitch.kp  pitch.ki  pitch.kd\n\r");
	printf("roll.kp   roll.ki   roll.kd\n\r");
	printf("yaw.kp    yaw.ki    yaw.kd\n\r");
	printf("------------------------------------------------------------------------------------------\n\r");

	printf("\n\rresume\n\r");
	printf("-Disable the commandline and resume to status report mode\n\r");

	printf("\n\rreset [parameter] /reset all\n\r");
	printf("-Drop the unsaved settings\n\r");	

	printf("\n\rquit\n\r");
	printf("-Quit the QuadCopter Status Monitor\n\r");

	printf("\n\r******************************************************************************************\n\r");	

	printf("\n\r[Please press q to quit the manual]");

	/* Exit */
	char ch = serial.getch();
	while(ch != 'q' && ch != 'Q')
		ch = serial.getch();
	
	monitor_it_cmd = MONITOR_RESUME;
}

int is_num(char *str) 
{
	/* If the char is not between 0-9, '.' or the first char is '.',
	   then this is not a number
	 */ 
	int i;
	for(i = 0;i < strlen(str); i++) {
		if(!isdigit((unsigned char)str[i]) && ((str[i] != '.') || (str[0] == '.')))
			return 0;
	}
	return 1;
}

int update_setting(char parameter[][MAX_CMD_LEN])
{
	if(strcmp(parameter[0], "update") == 0) {
		if(par_is_changed == 1) {
			printf("\x1b[0A\x1b[0G\x1b[0K");
			printf("[Warning:Are you sure you want to enable the new settings? (y/n)]\n\r");
				
			char *confirm_ch = NULL;			
	
			while(1) {
				confirm_ch = linenoise("> ");

				if(strcmp(confirm_ch, "y") == 0 || strcmp(confirm_ch, "Y") == 0) {
					/* Enable the new settings */
					int i;
					for(i = 0; i < PARAMETER_CNT; i++) {
							if(par_data[i].int_origin == 0) {
								/* Data is a float */
								if(par_data[i].par_is_changed == 1) {
									*(par_data[i].flt_origin) = par_data[i].flt_buf;
									par_data[i].par_is_changed = 0;
								}
							} else {
								/* Data is a int */
								if(par_data[i].par_is_changed == 1) {
									*(par_data[i].int_origin) = par_data[i].int_buf;
									par_data[i].par_is_changed = 0;
								}
							}
					}
					par_is_changed = 0;
					unsaved_print_cnt = 0;

					monitor_it_cmd = MONITOR_RESUME;
					break;
				} else if(strcmp(confirm_ch, "n") == 0 || strcmp(confirm_ch, "N") == 0 || confirm_ch == NULL) {
					break;
				} else if(confirm_ch == NULL) /* Ctrl^C */ {
					break;
				} else {	
					printf("[Error:Please type y(yes) or n(no)]\n\r");
					clean_line(2);
				}
			}
		} else {
			print_error_msg("[None of the settings have been changed]\n\r");
		}

		return CMD_EXECUTED;
	}
	
	return CMD_UNEXECUTED;
}

int set_parameter(char parameter[][MAX_CMD_LEN])
{
	int i, set_status = CMD_UNEXECUTED;
	for(i = 0; i < PARAMETER_CNT; i++) {
		if(strcmp(parameter[0], par_data[i].par_str) == 0) {
			if(is_num(parameter[1]) == 1) {
				printf("[Warning:Are you sure you want to change the setting? (y/n)]\n\r");

				char *confirm_ch = NULL;

				while(1) {
					confirm_ch = linenoise("> ");

					if(strcmp(confirm_ch, "y") == 0 || strcmp(confirm_ch, "Y") == 0) {
						/* If the pointer of int_orginial is set to 0,then this is a float */
						if(par_data[i].int_origin == 0) {
							/* Data is a float */
							par_data[i].flt_buf = atof(parameter[1]);
						} else {
							/* Data is a int */
							par_data[i].int_buf = atoi(parameter[1]);
						}
						par_is_changed = 1;
						par_data[i].par_is_changed = 1;

						break;
					} else if(strcmp(confirm_ch, "n") == 0 || strcmp(confirm_ch, "N") == 0) {
						break;
					} else if(confirm_ch == NULL) {
						clean_line(3 + unsaved_print_cnt);
						return CMD_EXECUTED;
					} else {
						printf("[Error:Please type y(yes) or n(no)]\n\r");
						printf("\x1b[0G\x1b[0K\x1b[0A\x1b[0G\x1b[0K\x1b[0A\x1b[0G\x1b[0K");
					}
				}
				set_status = CMD_EXECUTED;
					
				clean_line(3 + unsaved_print_cnt);
				unsaved_print_cnt = print_unsaved_setting();
				break;
			} else {
				/* Parameter 2 send a valid value */
				printf("[Error:%s is not a valid value]\n\r", parameter[1]);
				printf("[Please press any key to resume...]");

				serial.getch();
				printf("\x1b[0G\x1b[0K\x1b[0A\x1b[0G\x1b[0K\x1b[0A\x1b[0G\x1b[0K");

				set_status = CMD_EXECUTED;
				break;
			}
		}
	}

	return set_status;
}

void monitor_set(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	switch(par_cnt) {
	    case 0:
	    {
		/* User didn't pass any arguments */
		print_error_msg("[Error:Command \"set\" required at least 1 parameter]\n\r");
		break;
	    }
	    case 1:
	    {
		/* set update */
		if(update_setting(parameter) == CMD_EXECUTED);
		/* unknown parameter handling */
		else {
			/* Check if the user pass the parameter at here */
			int i;
			for(i = 0; i < PARAMETER_CNT; i++) {
				if(strcmp(parameter[0], par_data[i].par_str) == 0) {
					print_error_msg("[Error:Missing to pass a value while setting the parameter]\n\r");
					break;
				} else if(i == (PARAMETER_CNT - 1)) {
					print_error_msg("[Error:Unknown argument pass through with the \"set\" command]\n\r");
				}
			}			
		}

		break;	
	    }
	    case 2:
	    {	
		/* set [parameter] [value] */
		if(set_parameter(parameter) == CMD_EXECUTED); 
		else {
			/* Can't find the parameter of the QuadCopter */
			print_error_msg("[Error:Unknown argument pass through with the \"set\" command]\n\r");
		}
		break;
	    }
	    default:
		/* Too much arguments */
		print_error_msg("[Error:Too many arguments pass through with the \"set\" command]\n\r");
	}

}

int unsaved_check()
{
	int i;
	for(i = 0; i < PARAMETER_CNT; i++) {
		if(par_data[i].par_is_changed == 1)
			return 1;
	}
	
	return 0;
}

int reset_parameter(char parameter[][MAX_CMD_LEN])
{
	int i;
	for(i = 0; i < PARAMETER_CNT; i++) {
		if(strcmp(parameter[0], par_data[i].par_str) == 0) {
			if(par_data[i].par_is_changed == 1) {
				printf("\x1b[0A\x1b[0G\x1b[0K");
				printf("[Warning:Are you sure you want to reset the unsaved setting? (y/n)]\n\r");
				
				char *confirm_ch = NULL;			
		
				while(1) {
					confirm_ch = linenoise("> ");

					if(strcmp(confirm_ch, "y") == 0 || strcmp(confirm_ch, "Y") == 0) {
						/* Drop the setting */
						par_data[i].flt_buf = 0.0;
						par_data[i].int_buf = 0;
						/* Clean the is_changed flag */
						par_data[i].par_is_changed = 0;
		
						par_is_changed = unsaved_check();						

						clean_line(2 + unsaved_print_cnt);
						unsaved_print_cnt = print_unsaved_setting();
						break;
					} else if(strcmp(confirm_ch, "n") == 0 || strcmp(confirm_ch, "N") == 0 || confirm_ch == NULL) {
						clean_line(2);
						break;
					} else if(confirm_ch == NULL) /* Ctrl^C */ {
						clean_line(2);
						break;
					} else {	
						printf("[Error:Please type y(yes) or n(no)]\n\r");
						clean_line(2);
					}
				}
			} else {
				print_error_msg("[The setting have not been changed]\n\r");
			}
			return CMD_EXECUTED;
		}
	}
	return CMD_UNEXECUTED;
}

int reset_all(char parameter[][MAX_CMD_LEN])
{
	if(strcmp(parameter[0], "all") == 0)
	{      
		if(par_is_changed == 0) {
			print_error_msg("[None of the settings have been changed]\n\r");
			return CMD_EXECUTED; 
		}

		printf("[Warning:Are you sure you want to reset all settings? (y/n)]\n\r");
		char *confirm_ch = NULL;

		while(1) {
			confirm_ch = linenoise("> ");

			if(strcmp(confirm_ch, "y") == 0 || strcmp(confirm_ch, "Y") == 0) {
				int i;
				/* Clean the buffer and the flag of all copter parameters */
				for(i = 0; i < PARAMETER_CNT; i++) {
					/* If the pointer of int_orginial is set to 0,then this is a float */
					if(par_data[i].int_origin == 0) {
						/* Data is a float */
						par_data[i].flt_buf = 0.0;
					} else {
						/* Data is a int */
						par_data[i].int_buf = 0;
					}
				
					par_is_changed = 0;
					par_data[i].par_is_changed = 0;
					unsaved_print_cnt = 0;
				}
				break;
			} else if(strcmp(confirm_ch, "n") == 0 || strcmp(confirm_ch, "N") == 0) {
				break;
			} else if(confirm_ch == NULL) {
				clean_line(3);
				return CMD_EXECUTED;
			} else {
				printf("[Error:Please type y(yes) or n(no)]\n\r");
				printf("\x1b[0G\x1b[0K\x1b[0A\x1b[0G\x1b[0K\x1b[0A\x1b[0G\x1b[0K");
			}
			
		}
		
		monitor_it_cmd = MONITOR_RESUME;
		return CMD_EXECUTED;
	}

	return CMD_UNEXECUTED;
}

void monitor_reset(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	switch(par_cnt) {
	    case 0:
	    {
		/* User didn't pass any arguments */
		print_error_msg("[Error:Command \"reset\" required at least 1 parameter]\n\r");
		break;
	    }
	    case 1:
	    {
		/* reset [parameter] */
		if(reset_parameter(parameter) == CMD_EXECUTED);
		/* reset all */
		else if(reset_all(parameter) == CMD_EXECUTED);
		/* unknown parameter handling */
		else print_error_msg("[Error:Unknown argument pass through with the \"reset\" command]\n\r");

		break;	
	    }
	    default:
		/* Too much arguments */
		print_error_msg("[Error:Too many arguments pass through with the \"reset\" command]\n\r");
	}
}
