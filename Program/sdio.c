/*=====================================================================================================*/
/*=====================================================================================================*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "stm32f4_system.h"
#include "stm32f4_sdio.h"
#include "module_rs232.h"
#include "ff.h"
#include "diskio.h"
/*=====================================================================================================*/
/*=====================================================================================================*/
#define KEY   PBI(2)
#define LED_R PCO(15)
#define LED_G PCO(14)
#define LED_B PCO(13)

FATFS FatFs;
FRESULT res;
FILINFO finfo;
DIR dirs;
FIL file;

void SDIO_Config(void)
{
	while (SD_Init() != SD_OK)
		Delay_10ms(20);

}
int SD_demo(void)
{
	uint32_t i = 0;
	uint8_t ReadBuf[128] = {0};
	uint8_t TrData[8][32] = {0};
	uint8_t WriteData[50] = "ABCDEFG";

	res = f_mount(&FatFs, "", 1);
	res = f_opendir(&dirs, "0:/");
	res = f_readdir(&dirs, &finfo);
	res = f_open(&file, "SDCard_K.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
	res = f_write(&file, WriteData, 20, (UINT *)&i);
	file.fptr = 0;
	res = f_read(&file, ReadBuf, 100, (UINT *)&i);

	f_close(&file);
	printf("%s", ReadBuf);

	while (1) {
		vTaskDelay(200);
	}
}
