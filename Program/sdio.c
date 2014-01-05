/*=====================================================================================================*/
/*=====================================================================================================*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "stm32f4_system.h"
#include "stm32f4_sdio.h"
#include "module_rs232.h"
#include "string.h"
#include "sys_manager.h"
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
extern xTaskHandle sdio_task_handle;
SD_STATUS SDstatus;

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
		while(SDstatus == SD_UNREADY){
			uint32_t i = 0;
			float Kp = 12.123;
			float Kd = 50.966;
			char j;

			res = f_mount(&FatFs, "", 1);
			res = f_opendir(&dirs, "0:/");
			res = f_readdir(&dirs, &finfo);
			res = f_open(&file, "SDCard_K.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
			for(j=0;j<3;j++){
				sprintf(WriteData,"Kp : %f , Kd : %f \n\r",Kp,Kd);
				res = f_write(&file, WriteData, strlen(WriteData), (UINT *)&i);
			}
			file.fptr = 0;
			res = f_read(&file, ReadBuf, ReadBuf_Size, (UINT *)&i);
			f_close(&file);
			SDstatus = SD_READY ;
		}
		vTaskSuspend(sdio_task_handle);
		vTaskDelay(20);
	}
}
