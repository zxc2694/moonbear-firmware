#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "QuadCopterConfig.h"

/* Shell Command handlers */
void shell_unknown_cmd(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_clear(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_help(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_monitor(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_test(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_sdinfo(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_sdsave(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_license(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_watch(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_gui_test(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_tuning(char parameter[][MAX_CMD_LEN], int par_cnt);
void shell_showData(char parameter[][MAX_CMD_LEN], int par_cnt);

/* The identifier of the command */
enum SHELL_CMD_ID {
	unknown_cmd_ID,
	clear_ID,
	help_ID,
	monitor_ID,
	test_ID,
	sdinfo_ID,
	sdsave_ID,
	license_ID,
	watch_ID,
	gui_test_ID,
	tuning_ID,
	showData_ID,
	SHELL_CMD_CNT
};

//First string don't need to store anything for unknown commands
command_list shellCmd_list[SHELL_CMD_CNT] = {
	CMD_DEF(unknown_cmd, shell),
	CMD_DEF(clear, shell),
	CMD_DEF(help, shell),
	CMD_DEF(monitor, shell),
	CMD_DEF(test, shell),
	CMD_DEF(sdinfo, shell),
	CMD_DEF(sdsave, shell),
	CMD_DEF(license, shell),
	CMD_DEF(gui_test, shell),
	CMD_DEF(watch, shell),
	CMD_DEF(tuning, shell),
	CMD_DEF(showData, shell)
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
	while (system.status != SYSTEM_INITIALIZED);

#if configSTATUS_SHELL
	/* Clear the screen */
	serial.printf("\x1b[H\x1b[2J");
	/* Show the prompt messages */
	serial.printf("[System status]Initialized successfully!\n\r");
	serial.printf("QuadCopter Software Developing Shell\n\r");
	serial.printf("Copyleft (MIT License) 2014 - Moon Bear Copter Team\n\r");

	serial.printf("Please type \"help\" to get more informations\n\r");

#endif

	while (1) {
		linenoiseSetCompletionCallback(shell_linenoise_completion);

		command_data shell_cd = {.par_cnt = 0};

#if configSTATUS_SHELL
		char *shell_str = linenoise("Quadcopter Shell > ");


		if (shell_str == NULL)
			continue;

		commandExec(shell_str, &shell_cd, shellCmd_list, SHELL_CMD_CNT);

		linenoiseHistoryAdd(shell_str);
#endif

		//Show two values to gui2.py 
#if configSTATUS_GET_ROLL_PITCH
			while(1){
				serial.printf("%f %f\n\r", AngE.Roll, AngE.Pitch);
				vTaskDelay(100);
			}
#endif
		//Show two values to gui3.py 
#if configSTATUS_GET_ROLL_PITCH_YAW	
			while(1){
				serial.printf("%f %f %f\n\r", AngE.Roll, AngE.Pitch, AngE.Yaw);
				vTaskDelay(100);
			}
#endif

		//Show four values to gui4.py
#if configSTATUS_GET_MOTORS
			while(1){
				serial.printf("\n\r%f %f %f %f ",
					      system.variable[MOTOR1].value, system.variable[MOTOR2].value,
					      system.variable[MOTOR3].value, system.variable[MOTOR4].value
					     );
				vTaskDelay(100);
			}
/*			while(1){
				serial.printf("\n\r%f %f %f %f ",
					      system.variable[TEST1].value, system.variable[TEST2].value,
					      system.variable[TEST3].value, system.variable[TEST4].value
					     );
				vTaskDelay(100);
			}*/
#endif

		//Show six values to gui6.py
#if configSTATUS_GET_ROLL_PITCH_MOTORS
			while(1){
				serial.printf("\n\r%f %f %f %f %f %f",
					      AngE.Roll, AngE.Pitch,
					      system.variable[MOTOR1].value, system.variable[MOTOR2].value,
					      system.variable[MOTOR3].value, system.variable[MOTOR4].value
					     );
				vTaskDelay(100);
			}
#endif

#if configSTATUS_GET_MAG
			while(1){
				serial.printf("%f %f %f \n\r",
					      system.variable[MAGX].value, system.variable[MAGY].value,
					      system.variable[MAGZ].value);
				vTaskDelay(100);
			}
/*			while(1){
				serial.printf("%f %f %f \n\r",
					      system.variable[TEST1].value, system.variable[TEST2].value,
					      system.variable[TEST3].value);
				vTaskDelay(100);
			}*/
#endif	

#if configSTATUS_GET_ACC
			while(1){
				serial.printf("%f %f %f \n\r",
					      system.variable[ACCX].value, system.variable[ACCY].value,
					      system.variable[ACCZ].value);
				vTaskDelay(100);
			}
#endif

#if configSTATUS_GET_GYRO
			while(1){
				serial.printf("%f %f %f \n\r",
					      system.variable[GYROX].value, system.variable[GYROY].value,
					      system.variable[GYROZ].value);
				vTaskDelay(100);
			}
#endif

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
	serial.printf("sdinfo\tShow SD card informations.\n\r");
	serial.printf("sdsave\tSave PID informations in the SD card.\n\r");
	serial.printf("watch\tDisplay attitudes and motors PWM\n\r");
	serial.printf("tuning\ttuning the PD gains\n\r");
	serial.printf("showData Display all data\n\r");
	serial.printf("\n\r");
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
	//FRESULT res;
	FILINFO finfo;
	DIR dirs;
	FIL file;

	char *confirm_ch = NULL;
	confirm_ch = linenoise("Store all the PID settings? (y/n):");

	while (strcmp(confirm_ch, "y") == 0 || strcmp(confirm_ch, "Y") == 0) {
		uint32_t i = 0;
		f_mount(&FatFs, "", 1);
		f_opendir(&dirs, "0:/");
		f_readdir(&dirs, &finfo);
		f_open(&file, "SDCARD_K.TXT", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);

		sprintf(WriteData, "<Pitch> Kp: %f, Ki: %f, Kd: %f\n\r<Roll> Kp: %f, Ki: %f, Kd: %f\n\r<Yaw> Kp: %f, Ki: %f, Kd: %f\n\r",
			PID_Pitch.Kp, PID_Pitch.Ki, PID_Pitch.Kd,
			PID_Roll.Kp, PID_Roll.Ki, PID_Roll.Kd,
			PID_Yaw.Kp, PID_Yaw.Ki, PID_Yaw.Kd
		       );
		

		f_write(&file, WriteData, strlen(WriteData), (UINT *)&i);

		file.fptr = 0;
		f_read(&file, ReadBuf, ReadBuf_Size, (UINT *)&i);

		/* Debug */
		serial.printf("Has been saved !\n\r\n\r");
		serial.printf("Read SD card ......\n\r");
		serial.printf("%s\n\r", ReadBuf);

		f_close(&file);
		break;
	}
}

void shell_license(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	serial.printf("Moon bear QuadCopter Project\n\r");

	serial.printf("Copyright (c) 2014 - MIT License\n\r\n\r");

	serial.printf("QCopterFC\n\r");
	serial.printf("Wen-Hung Wang <Hom19910422@gmail.com>\n\r\n\r");

	serial.printf("Linenoise\n\r");
	serial.printf("Antirez <antirez@gmail.com>\n\r\n\r");

	serial.printf("Contributors of Moon Bear team\n\r");
	serial.printf("Da-Feng Huang <fantasyboris@gmail.com>\n\r");
	serial.printf("Cheng-De Liu <zxc2694zxc2694@gmail.com>\n\r");
	serial.printf("Cheng-Han Yang <poemofking@gmail.com>\n\r");
	serial.printf("Shengwen Cheng <l1996812@gmail.com>\n\r");
}

void shell_gui_test(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	char buf[17] = {'\0'};
	IMU_package package;

	while (1) {
		package.roll = (int16_t)system.variable[TRUE_ROLL].value * 100;
		package.pitch  = (int16_t)system.variable[TRUE_PITCH].value * 100;
		package.yaw = (int16_t)system.variable[TRUE_YAW].value * 100;
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

void shell_tuning(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	serial.printf("\nRoll_Kp:%f  Pitch_Kp:%f  ,Yaw_Kp:%f  \n\rRoll_Kd:%f  ,Pitch_Kd:%f  Yaw_Kd:%f \n\n\r", 
					PID_Roll.Kp,PID_Pitch.Kp,PID_Yaw.Kp,PID_Roll.Kd,PID_Pitch.Kd,PID_Yaw.Kd);
	vTaskDelay(50);
	serial.printf("You can change 'Roll angle' control parameter ...\n\r");
	serial.printf("Change Kp gain: input 'p' -> input '+' or '-'  \n\r");
	serial.printf("Change Kd gain: input 'd' -> input '+' or '-'  \n\n\r");
	while(1){
		if(serial.getc() == 'p'){ 
			serial.printf("[tuning Kp] (input '+' or '-') \n\r");
			while(1){
				if(serial.getc() == '+'){ 
					PID_Roll.Kp = PID_Roll.Kp + 0.3f;
					serial.printf("> Roll.Kp = %f\n\r", PID_Roll.Kp);
				}
				else if(serial.getc() == '-'){ 
					PID_Roll.Kp = PID_Roll.Kp - 0.3f;
					serial.printf("> Roll.Kp = %f\n\r", PID_Roll.Kp);
				}
				else if(serial.getc() == 'q'){ 
					break;
				}
			}
		}
		else if(serial.getc() == 'd'){ 
			serial.printf("[tuning Kd] (input '+' or '-') \n\r");
			while(1){
				if(serial.getc() == '+'){ 
					PID_Roll.Kd = PID_Roll.Kd + 0.3f;
					serial.printf("> Roll.Kd = %f\n\r", PID_Roll.Kd);
				}
				else if(serial.getc() == '-'){ 
					PID_Roll.Kd = PID_Roll.Kd - 0.3f;
					serial.printf("> Roll.Kd = %f\n\r", PID_Roll.Kd);
				}
				else if(serial.getc() == 'q'){ 
					break;
				}
			}
		}
		else if(serial.getc() == 'q') 
			break;
	}
}

void shell_showData(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	serial.printf("-----------showData command------------\n\r");
	serial.printf("'z'=Show attitude  -> Pitch Roll Yaw\n\r");
	serial.printf("'x'=Show motor PWM -> Motor1 ~ Motor4\n\r");
	serial.printf("'c'=Show CCR PWM  -> CCR1 ~ CCR5 \n\r");
	serial.printf("'v'=Show PD gain -> Pitch:Kp Kd, Roll:Kp Kd, Yaw:Kp Kd\n\r");
	serial.printf("'m'=Show magnetometer data ...\n\r");
	serial.printf("'b'=Show barometer press ...\n\r");
	serial.printf("'1'=Just test -1...\n\r");
	serial.printf("'2'=Just test -2 ...\n\r");
	serial.printf("'3'=Just test -3...\n\r");
	serial.printf("'4'=Just test -4 ...\n\r");
	serial.printf("'q'=quit showData command.\n\r");
	serial.printf("'h'=Print command function.\n\r");
	while(1){
		if(serial.getc() == 'z'){ 
			serial.printf("Pitch : %f\tRoll : %f\tYaw : %f\n\r", AngE.Pitch, AngE.Roll, AngE.Yaw);
			vTaskDelay(50);
		}
		else if(serial.getc() == 'x'){
			serial.printf("Motor1: %f\tMotor2: %f\tMotor3: %f\tMotor4: %f\n\r",
			        system.variable[MOTOR1].value, system.variable[MOTOR2].value,
			        system.variable[MOTOR3].value, system.variable[MOTOR4].value
					     );
			vTaskDelay(50);
		}
		else if(serial.getc() == 'c'){
			serial.printf("PWM1_CCR: %f\t,PWM2_CCR: %f\t,PWM3_CCR: %f\t,PWM4_CCR: %f\t,PWM5_CCR: %f\t\n\r",
					system.variable[PWM1_CCR].value,system.variable[PWM2_CCR].value,
					system.variable[PWM3_CCR].value,system.variable[PWM4_CCR].value,system.variable[PWM5_CCR].value);
			vTaskDelay(50);
		}
		else if(serial.getc() == 'v'){
			serial.printf("Pitch_Kp:%f  Pitch_Kd:%f  ,Roll_Kp:%f  Roll_Kd:%f  ,Yaw_Kp:%f  Yaw_Kd:%f \n\r", 
					PID_Pitch.Kp,PID_Pitch.Kd,PID_Roll.Kp,PID_Roll.Kd,PID_Yaw.Kp,PID_Yaw.Kd);
			vTaskDelay(50);
		}

		else if(serial.getc() == 'b'){
			serial.printf("Barometer: %f\n\r",system.variable[BAROMETER].value);
			vTaskDelay(50);
		}
		else if(serial.getc() == '1'){	  

			serial.printf("test1: %f\n\r",system.variable[TEST1].value);
			vTaskDelay(50);
		}
		else if(serial.getc() == '2'){	  
			
			serial.printf("test1: %f\n\r",system.variable[TEST2].value);
			vTaskDelay(50);
		}
		else if(serial.getc() == '3'){	  

			serial.printf("test3: %f\n\r",system.variable[TEST3].value);
			vTaskDelay(50);
		}
		else if(serial.getc() == '4'){	  
			
			serial.printf("test4: %f\n\r",system.variable[TEST4].value);
			vTaskDelay(50);
		}
		else if(serial.getc() == 'm'){
			serial.printf("MAG X: %f\tMAG Y: %f\tMAG Z: %f\n\r",
			        system.variable[MAGX].value, system.variable[MAGY].value,
			        system.variable[MAGZ].value);
			vTaskDelay(50);
		}
		if(serial.getc() == 'h'){ 
			serial.printf("-----------showData command------------\n\r");
			serial.printf("'z'=Show attitude  -> Pitch Roll Yaw\n\r");
			serial.printf("'x'=Show motor PWM -> Motor1 ~ Motor4\n\r");
			serial.printf("'c'=Show CCR PWM  -> CCR1 ~ CCR5 \n\r");
			serial.printf("'v'=Show PD gain -> Pitch:Kp Kd, Roll:Kp Kd, Yaw:Kp Kd\n\r");
			serial.printf("'m'=Show magnetometer data ...\n\r");
			serial.printf("'b'=Show barometer press ...\n\r");
			serial.printf("'1'=Just test -1...\n\r");
			serial.printf("'2'=Just test -2 ...\n\r");
			serial.printf("'3'=Just test -3...\n\r");
			serial.printf("'4'=Just test -4 ...\n\r");
			serial.printf("'q'=quit showData command.\n\r");
			serial.printf("'h'=Print command function.\n\r");
		}
		
		else if(serial.getc() == 'q') 
			break;
	}
}