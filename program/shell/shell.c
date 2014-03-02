#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "QuadCopterConfig.h"

/* Shell Command handlers */
void shell_unknown_cmd(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_clear(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_help(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_monitor(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_ps(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_test(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_sdinfo(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_sdsave(char parameter[][MAX_CMD_LEN], int par_cnt);

/* The identifier of the command */
enum SHELL_CMD_ID {
	unknown_cmd_ID,
	clear_ID,
	help_ID,
	monitor_ID,
	/*ps_ID,*/
	test_ID,
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
	CMD_DEF(test, shell),
	CMD_DEF(sdinfo, shell),
	CMD_DEF(sdsave, shell)
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
	serial.printf("QuadCopter Software Developing Shell\n\r");
	serial.printf("Copyleft (MIT License) 2014 - Moon Bear Copter Team\n\r");

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
	serial.printf("Capacity : ");
	serial.printf("%d MB\r\n", (int)(SDCardInfo.CardCapacity >> 20));
	serial.printf("CardBlockSize : ");
	serial.printf("%d\r\n", SDCardInfo.CardBlockSize);
	serial.printf("CardType : ");
	serial.printf("%d\r\n", SDCardInfo.CardType);
	serial.printf("RCA : ");
	serial.printf("%d\r\n", SDCardInfo.RCA);
	serial.printf("----------------------\r\n\r\n");
}


#define ReadBuf_Size 500
#define WriteData_Size 500

uint8_t ReadBuf[ReadBuf_Size] = {'\0'};
char WriteData[WriteData_Size] = {EOF};

/* TODO:Porting this command into the status monitor */
void shell_sdsave(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	FATFS FatFs;
	FRESULT res;
	FILINFO finfo;
	DIR dirs;
	FIL file;		

	char *confirm_ch = NULL;
	confirm_ch = linenoise("Store all the PID settings? (y/n):");

	while(strcmp(confirm_ch, "y") == 0 || strcmp(confirm_ch, "Y") == 0) {
		uint32_t i = 0;
		res = f_mount(&FatFs, "", 1);
		res = f_opendir(&dirs, "0:/");
		res = f_readdir(&dirs, &finfo);
		res = f_open(&file, "SDCard_K.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);

		sprintf(WriteData,"Pitch\n\r%f,%f,%f\n\rRoll\n\r%f,%f,%f\n\rYaw\n\r%f,%f,%f\n\r",
			PID_Pitch.Kp, PID_Pitch.Ki, PID_Pitch.Kd,
			PID_Roll.Kp, PID_Roll.Ki, PID_Roll.Kd,
			PID_Yaw.Kp, PID_Yaw.Ki, PID_Yaw.Kd
		);

		res = f_write(&file, WriteData, strlen(WriteData), (UINT *)&i);

		file.fptr = 0;
		res = f_read(&file, ReadBuf, ReadBuf_Size, (UINT *)&i);
		
		/* Debug */
		serial.printf("[Debug output]\n\r%s\n\r", ReadBuf);		

		f_close(&file);
		break;
	}
}
