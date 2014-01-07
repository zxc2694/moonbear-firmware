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
#include "stm32f4_sdio.h"

#include "FreeRTOS.h"
#include "task.h"

#include "status_monitor.h"

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

/* The identifier of the command */
enum SHELL_CMD_ID {
	unknown_cmd_ID,
	clear_ID,
	help_ID,
	monitor_ID,
	/*ps_ID,*/
	sdinfo_ID,
	sdsave_ID,
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
	/* Clear the screen */
	printf("\x1b[H\x1b[2J");
	/* Show the prompt messages */
	printf("[System status]Initialized successfully!\n\r");
	printf("Please type \"help\" to get more informations\n\r");

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
	printf("Command not found\n\r");
}

void shell_clear(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	linenoiseClearScreen();
}

void shell_help(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	printf("\n\rLinenoise shell environment <QuadCopter Shell>\n\r");
	printf("The lineoise is auorized under BSD License and released by antirez\n\r");
	printf("The QCopterFlightControl is based on Hom19910422's version\n\r");

	printf("\n\rSupport commands:\n\r");
	printf("clear  \tClear the screan\n\r");
	printf("help \tShow the list of all commands\n\r");
	printf("monitor The QuadCopter Status monitor\n\r");
	printf("ps \tShow the list of all tasks\n\r");
	printf("sdinfo\tShow SD card informations.\n\r");
	printf("sdsave\tSave PID informations in the SD card.\n\r");

}

void shell_ps(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	signed char buf[256] = {'\0'};

	vTaskList(buf);

	//TODO:replace the hardcode by using sprintf()
	printf("\n\rName          State   Priority  Stack Num\n\r");
	printf("*****************************************\n\r");
	printf("%s\n\r", buf);
}

void shell_sdinfo(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	printf("-----SD Init Info-----\r\n");
	printf(" Capacity : ");
	printf("%d MB\r\n", (int)(SDCardInfo.CardCapacity >> 20));
	printf(" CardBlockSize : ");
	printf("%d\r\n", SDCardInfo.CardBlockSize);
	printf(" CardType : ");
	printf("%d\r\n", SDCardInfo.CardType);
	printf(" RCA : ");
	printf("%d\r\n", SDCardInfo.RCA);
	printf("----------------------\r\n\r\n");
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
		printf("Has been saved ! \n\r\n\r");
		printf("Read SD card ...... \n\r");
		printf("SD card content : \n\r");		
		printf("%s", ReadBuf);	
	}
	else if(SDcondition == SD_UNSAVE){
		printf("OK! not store!\n\r");
	}
	else if(SDcondition == SD_ERSAVE){
		printf("error!\n\r");
	}
}