/*=====================================================================================================*/
/*=====================================================================================================*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "linenoise.h"  
#include "stm32f4_system.h"
#include "stm32f4_sdio.h"
#include "module_rs232.h"
#include "string.h"
#include "sys_manager.h"
#include "algorithm_pid.h"
#include "ff.h"
#include "diskio.h"
#include "sys_manager.h"
/*=====================================================================================================*/
/*=====================================================================================================*/
FATFS FatFs;
FRESULT res;
FILINFO finfo;
DIR dirs;
FIL file;
extern SYSTEM_STATUS sys_status;
extern xSemaphoreHandle sdio_semaphore;
SD_STATUS SDstatus;
SD_STATUS SDcondition;

#define ReadBuf_Size 500
#define WriteData_Size 500

uint8_t ReadBuf[ReadBuf_Size] = {'\0'};
char WriteData[WriteData_Size] = {EOF};

void SDIO_Config(void)
{
	while (SD_Init() != SD_OK)
		Delay_10ms(20);

}

void sdio_task()
{
	while (sys_status == SYSTEM_UNINITIALIZED);
	while(1){
		if( xSemaphoreTake(sdio_semaphore, 99999) ){	
			char *confirm_ch = NULL;
			confirm_ch = linenoise("\n\rDo you want to store PID control parameter ? (y/n) :");			
			while(1){
				if(strcmp(confirm_ch, "y") == 0 || strcmp(confirm_ch, "Y") == 0) {
				    uint32_t i = 0;
                    res = f_mount(&FatFs, "", 1);
                    res = f_opendir(&dirs, "0:/");
                    res = f_readdir(&dirs, &finfo);
                    res = f_open(&file, "SDCard_K.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
    
                    sprintf(WriteData,"<PID_Pitch> Kp : %f , Ki : %f ,kd : %f \n\r",PID_Pitch.Kp,PID_Pitch.Ki,PID_Pitch.Kd);
                    res = f_write(&file, WriteData, strlen(WriteData), (UINT *)&i);
                    vTaskDelay(50);
                    sprintf(WriteData,"<PID_Roll> Kp : %f , Ki : %f ,kd : %f \n\r",PID_Roll.Kp,PID_Roll.Ki,PID_Roll.Kd);
                    res = f_write(&file, WriteData, strlen(WriteData), (UINT *)&i);
                    vTaskDelay(50);
                    file.fptr = 0;
                    res = f_read(&file, ReadBuf, ReadBuf_Size, (UINT *)&i);
                    SDcondition = SD_SAVE;
                    SDstatus = SD_READY ;
                    f_close(&file);
					break;
				}
				else if(strcmp(confirm_ch, "n") == 0 || strcmp(confirm_ch, "N") == 0 || confirm_ch == NULL){
					SDcondition = SD_UNSAVE;
					SDstatus = SD_READY ;
					break;
				}
				else {
					SDcondition = SD_ERSAVE;
					SDstatus = SD_READY ;
					break;
				}
			}
		}
	}
}

