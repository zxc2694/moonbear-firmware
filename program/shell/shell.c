/* Standard C lib includes */
#include <stddef.h>
#include <string.h>

#include "QuadCopterConfig.h"

#define ReadBuf_Size 500
extern sdio_task_handle;
extern SD_STATUS SDstatus;
extern SD_STATUS SDcondition;
extern ReadBuf[ReadBuf_Size];
extern xSemaphoreHandle sdio_semaphore;
extern SYSTEM_STATUS set_PWM_Motors;
/* Shell Command handlers */
void shell_unknown_cmd(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_clear(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_help(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_monitor(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_ps(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_sdinfo(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_sdsave(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_watch(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_gui(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_guiBinary(char parameter[][MAX_CMD_LEN], int par_cnt);
/* The identifier of the command */
enum SHELL_CMD_ID {
	unknown_cmd_ID,
	clear_ID,
	help_ID,
	monitor_ID,
	/*ps_ID,*/
	sdinfo_ID,
	sdsave_ID,
	watch_ID,
	gui_ID,
	guiBinary_ID,
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
	CMD_DEF(watch, shell),
	CMD_DEF(gui, shell),
	CMD_DEF(guiBinary, shell),
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
	#if configSTATUS_SHELL
		printf("[System status]Initialized successfully!\n\r");
		printf("Please type \"help\" to get more informations\n\r");
	#endif

	while (1) {
		linenoiseSetCompletionCallback(shell_linenoise_completion);

		command_data shell_cd = {.par_cnt = 0};

		#if configSTATUS_SHELL
			char *shell_str = linenoise("Quadcopter Shell > ");
			if(shell_str == NULL)
				continue;
			commandExec(shell_str, &shell_cd, shellCmd_list, SHELL_CMD_CNT);
			linenoiseHistoryAdd(shell_str);
		#endif

		#if configSTATUS_GET_ROLL_PITCH
			while(1){
				printf("%f %f\n\r", AngE.Roll, AngE.Pitch);
				vTaskDelay(50);
			}
		#endif

		#if configSTATUS_GET_MOTORS
			while(1){
				printf("%f %f %f %f\n\r",global_var[MOTOR1].param, global_var[MOTOR2].param, global_var[MOTOR3].param, global_var[MOTOR4].param);
	//TEST 			printf("%f %f %f %f\n\r",global_var[test1].param, global_var[test2].param, global_var[test3].param, global_var[test4].param);
				vTaskDelay(50);
			}
		#endif
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
	printf("watch\tObserve attitude & debug !\n\r");
	printf("gui\tSupport real time display by python.\n\r");
	printf("guiBinary\tBinary transmit function\n\r");

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

void shell_watch(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	printf("-----------Watch command------------\n\r");
	printf("'z'=Show attitude  -> Pitch Roll Yaw\n\r");
	printf("'x'=Show motor PWM -> Motor1 ~ Motor4\n\r");
	printf("'c'=Show WFLY PWM  -> CCR1 ~ CCR4 \n\r");
	printf("'v'=Show PD gain -> Pitch:Kp Kd, Roll:Kp Kd, Yaw:Kp Kd\n\r");
	printf("'b'=Just for debug-1 ...\n\r");
	printf("'n'=Just for debug-2...\n\r");
	printf("'m'=Just for debug-3 ...\n\r");
	printf("'q'=quit watch command.\n\r");
	printf("'h'=Printf watch command function.\n\r");
	while(1){
		if(serial.getch() == 'z'){ 
			printf("Pitch : %f\tRoll : %f\tYaw : %f\n\r", AngE.Pitch, AngE.Roll, AngE.Yaw);
			vTaskDelay(50);
		}
		else if(serial.getch() == 'x'){
			printf("MOTOR: %f  %f  %f  %f\n\r",global_var[MOTOR1].param, global_var[MOTOR2].param, global_var[MOTOR3].param, global_var[MOTOR4].param);
			vTaskDelay(50);
		}
		else if(serial.getch() == 'c'){
			printf("PWM1_CCR: %f\t,PWM2_CCR: %f\t,PWM3_CCR: %f\t,PWM4_CCR: %f\n\r",global_var[PWM1_CCR].param,global_var[PWM2_CCR].param,global_var[PWM3_CCR].param,global_var[PWM4_CCR].param);
			vTaskDelay(50);
		}
		else if(serial.getch() == 'v'){
			printf("Pitch_Kp:%f  Pitch_Kd:%f  ,Roll_Kp:%f  Roll_Kd:%f  ,Yaw_Kp:%f  Yaw_Kd:%f \n\r", PID_Pitch.Kp, PID_Pitch.Kd, PID_Roll.Kp, PID_Roll.Kd, PID_Yaw.Kp, PID_Yaw.Kd);
		}

		else if(serial.getch() == 'b'){
			printf("Barometer: %f\n\r",global_var[BAROMETER].param);
			vTaskDelay(50);
		}
		else if(serial.getch() == 'n'){	  
			global_var[Write_PWM_Motor1].param = 830;  
			global_var[Write_PWM_Motor2].param = 830; 
			global_var[Write_PWM_Motor3].param = 830;
			global_var[Write_PWM_Motor4].param = 830;
			set_PWM_Motors = SYSTEM_INITIALIZED;
			printf("ok, set 830 !\n");
			vTaskDelay(50);
			set_PWM_Motors = SYSTEM_INITIALIZED;
		}
		else if(serial.getch() == 'm'){	  
			
			printf("Change WFLY controller !\n");
			vTaskDelay(50);
			set_PWM_Motors = SYSTEM_UNINITIALIZED;
		}
		if(serial.getch() == 'h'){ 
			printf("-----------Watch command------------\n\r");
			printf("'z'=Show attitude  -> Pitch Roll Yaw\n\r");
			printf("'x'=Show motor PWM -> Motor1 ~ Motor4\n\r");
			printf("'c'=Show WFLY PWM  -> CCR1 ~ CCR4 \n\r");
			printf("'v'=Show PD gain -> Pitch:Kp Kd, Roll:Kp Kd, Yaw:Kp Kd\n\r");
			printf("'b'=Show barometer value.\n\r");
			printf("'n'=Just for debug-2...\n\r");
			printf("'m'=Just for debug-3 ...\n\r");
			printf("'q'=quit watch command.\n\r");
			printf("'h'=Printf watch command function.\n\r");
		}
		
		else if(serial.getch() == 'q') 
			break;
	}
}
//Support real time to display GUI by python.
void shell_gui(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	while(1){
	
		printf("%f %f %f %f %f %f\n\r", AngE.Pitch, AngE.Roll, global_var[MOTOR1].param, global_var[MOTOR2].param, global_var[MOTOR3].param, global_var[MOTOR4].param);
		
		vTaskDelay(10);
	}
}

void shell_guiBinary(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	char buf[17] = {'\0'};
	IMU_package package;

	while (1) {
		package.roll = (int16_t)AngE.Roll;
		package.pitch  = (int16_t)AngE.Pitch;
		package.yaw = (int16_t)AngE.Yaw;
		package.acc_x = Acc.X;
		package.acc_y = Acc.Y;
		package.acc_z = Acc.Z;
		package.gyro_x = Gyr.X;
		package.gyro_y = Gyr.Y;
		package.gyro_z = Gyr.Z;

		generate_package(&package, (uint8_t *)buf);
		send_package((uint8_t *)buf);
	}
}


