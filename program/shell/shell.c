#include <stddef.h>
#include <string.h>

#include "QuadCopterConfig.h"

#define ReadBuf_Size 500
extern sdio_task_handle;
extern SD_STATUS SDstatus;
extern SD_STATUS SDcondition;
extern ReadBuf[ReadBuf_Size];
extern xSemaphoreHandle sdio_semaphore;
/* Shell Command handlers */
void shell_unknown_cmd(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_clear(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_help(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_monitor(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_ps(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_sdinfo(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_sdsave(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_attitude(char parameter[][MAX_CMD_LEN], int par_cnt);

/* The identifier of the command */
enum SHELL_CMD_ID {
	unknown_cmd_ID,
	clear_ID,
	help_ID,
	monitor_ID,
	/*ps_ID,*/
	sdinfo_ID,
	sdsave_ID,
	attitude_ID,
	SHELL_CMD_CNT
};

//First string don't need to store anything for unknown commands
command_list shellCmd_list[SHELL_CMD_CNT] = {
	CMD_DEF(unknown_cmd, shell),
	CMD_DEF(clear, shell),
	CMD_DEF(help, shell),
	CMD_DEF(monitor, shell),
	/*CMD_DEF(ps, shell),*/
	CMD_DEF(sdinfo, shell),
	CMD_DEF(sdsave, shell),
	CMD_DEF(attitude, shell),
};

/**** Shell task **********************************************************************/
void shell_linenoise_completion(const char *buf, linenoiseCompletions *lc)
{
	int i; //i = 1 to ignore the "UNKNOWN_COMMAND" string

	for (i = 1; i < SHELL_CMD_CNT; i++) {
		if (buf[0] == shellCmd_list[i].str[0])
			linenoiseAddCompletion(lc, shellCmd_list[i].str);
	}
}

void shell_task()
{
	//Waiting for system finish initialize
        while (sys_status != SYSTEM_INITIALIZED);

	/* Clear the screen */
	serial.printf("\x1b[H\x1b[2J");
	/* Show the prompt messages */
	serial.printf("[System status]Initialized successfully!\n\r");

	serial.printf("Please type \"help\" to get more informations\n\r");

	while (1) {
		linenoiseSetCompletionCallback(shell_linenoise_completion);

		command_data shell_cd = {.par_cnt = 0};

		char *shell_str = linenoise("Quadcopter Shell > ");
		
		if(shell_str == NULL)
			continue;

		commandExec(shell_str, &shell_cd, shellCmd_list, SHELL_CMD_CNT);

		linenoiseHistoryAdd(shell_str);
	}
}
/**** Customize command function ******************************************************/
void shell_unknown_cmd(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	serial.printf("Command not found\n\r");
}

void shell_clear(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	linenoiseClearScreen();
}

void shell_help(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	serial.printf("\n\rLinenoise shell environment <QuadCopter Shell>\n\r");
	serial.printf("The lineoise is auorized under BSD License and released by antirez\n\r");
	serial.printf("The QCopterFlightControl is based on Hom19910422's version\n\r");

	serial.printf("\n\rSupport commands:\n\r");
	serial.printf("clear  \tClear the screan\n\r");
	serial.printf("help \tShow the list of all commands\n\r");
	serial.printf("monitor The QuadCopter Status monitor\n\r");
	serial.printf("ps \tShow the list of all tasks\n\r");
	serial.printf("sdinfo\tShow SD card informations.\n\r");
	serial.printf("sdsave\tSave PID informations in the SD card.\n\r");
	serial.printf("attitude\t['z'=show angle;'x'=show PID parameter;'c'=show channel of PWM;'q'=quit]\n\r");
}

void shell_ps(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	signed char buf[256] = {'\0'};

	vTaskList(buf);

	//TODO:replace the hardcode by using sserial.printf()
	serial.printf("\n\rName          State   Priority  Stack Num\n\r");
	serial.printf("*****************************************\n\r");
	serial.printf("%s\n\r", buf);
}

void shell_sdinfo(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	serial.printf("-----SD Init Info-----\r\n");
	serial.printf(" Capacity : ");
	serial.printf("%d MB\r\n", (int)(SDCardInfo.CardCapacity >> 20));
	serial.printf(" CardBlockSize : ");
	serial.printf("%d\r\n", SDCardInfo.CardBlockSize);
	serial.printf(" CardType : ");
	serial.printf("%d\r\n", SDCardInfo.CardType);
	serial.printf(" RCA : ");
	serial.printf("%d\r\n", SDCardInfo.RCA);
	serial.printf("----------------------\r\n\r\n");
	vTaskDelay(100);	
}

void shell_sdsave(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	SDstatus = SD_UNREADY;
	SDcondition == SD_UNSAVE;
	xSemaphoreGive(sdio_semaphore);
	while(SDstatus == SD_UNREADY);
	vTaskDelay(200);
	if(SDcondition == SD_SAVE){
		serial.printf("Has been saved ! \n\r\n\r");
		serial.printf("Read SD card ...... \n\r");
		serial.printf("SD card content : \n\r");		
		serial.printf("%s", ReadBuf);	
	}
	else if(SDcondition == SD_UNSAVE){
		serial.printf("OK! not store!\n\r");
	}
	else if(SDcondition == SD_ERSAVE){
		serial.printf("error!\n\r");
	}
}

/* The attitude command can help user observe Roll & Pitch & Yaw angle, kp & kd & ki and PWM input.*/
/*===================================
'z' = show roll & pitch & yaw angle
'x' = show PID parameter
'c' = show channel of PWM
'q' = quit
=====================================*/
void shell_attitude(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	while(1){
		if(serial.getc() == 'z'){ 
			serial.printf("Pitch : %f\tRoll : %f\tYaw : %f\t\n\r", AngE.Pitch, AngE.Roll, AngE.Yaw);
			vTaskDelay(50);
		}
		else if(serial.getc() == 'x'){
			serial.printf("Pitch_Kp:%f  Pitch_Kd:%f  ,Roll_Kp:%f  Roll_Kd:%f  ,Yaw_Kp:%f  Yaw_Kd:%f \n\r", PID_Pitch.Kp, PID_Pitch.Kd, PID_Roll.Kp, PID_Roll.Kd, PID_Yaw.Kp, PID_Yaw.Kd);
		}
		else if(serial.getc() == 'c'){
			serial.printf("PWM1_CCR: %f\t,PWM2_CCR: %f\t,PWM3_CCR: %f\t,PWM4_CCR: %f\n\r",global_var[PWM1_CCR].param,global_var[PWM2_CCR].param,global_var[PWM3_CCR].param,global_var[PWM4_CCR].param);
			vTaskDelay(50);
		}
		else if(serial.getc() == 'q') 
			break;
	}
}