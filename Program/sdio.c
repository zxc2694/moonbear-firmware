/*=====================================================================================================*/
/*=====================================================================================================*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "stm32f4_system.h"
#include "stm32f4_sdio.h"
#include "module_rs232.h"
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

void SDIO_Config(void)
{
	while (SD_Init() != SD_OK)
		Delay_10ms(20);

}

void sdio_task()
{
	while (sys_status == SYSTEM_UNINITIALIZED);

	uint32_t i = 0;
	uint8_t ReadBuf[128] = {0};
	char WriteData[50];
	float Kp = 12.123;
	float Kd = 50.966;

	sprintf(WriteData,"Kp : %f , Kd : %f  ",Kp,Kd);
	res = f_mount(&FatFs, "", 1);
	res = f_opendir(&dirs, "0:/");
	res = f_readdir(&dirs, &finfo);
	res = f_open(&file, "SDCard_K.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
	res = f_write(&file, WriteData, strlen(WriteData), (UINT *)&i);
	file.fptr = 0;
	res = f_read(&file, ReadBuf, strlen(WriteData), (UINT *)&i);

	f_close(&file);
	printf("%s\n", ReadBuf);

	while (1) {
		vTaskDelay(200);
	}
}
