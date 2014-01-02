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
	while (SD_Init() != SD_OK) {
		while (1) {
			LED_R = ~LED_R;
			Delay_10ms(20);
		}
	}
}
int SD_demo(void)
{
	uint32_t i = 0;
	uint8_t ReadBuf[128] = {0};
	uint8_t TrData[8][32] = {0};
	uint8_t WriteData[50] = "ABCDEFG";

	LED_R = 1;
	LED_G = 1;

	// SD Card Init Info
	printf(" \r\n");
	printf("----------------------\r\n");
	printf("----------------------\r\n");
	printf(" SDIO SD FatFs demo\r\n");
	printf("----------------------\r\n");
	printf("----------------------\r\n\r\n");
	printf(" SD Init ... ");

	printf("OK!!\r\n\r\n");

	printf("-----SD Init Info-----\r\n");

	printf(" Capacity : ");
	printf("%d MB\r\n", SDCardInfo.CardCapacity >> 20);

	printf(" CardBlockSize : ");
	printf("%d\r\n", SDCardInfo.CardBlockSize);

	printf(" CardType : ");
	printf("%d\r\n", SDCardInfo.CardType);

	printf(" RCA : ");
	printf("%d\r\n", SDCardInfo.RCA);
	printf("----------------------\r\n\r\n");

	/*  // Wait
	  while(KEY != 1){
	    LED_G = ~LED_G;
	    vTaskDelay(200);
	  }*/

	// Read Directory File
	printf("----------------------\r\n");
	printf(" SD_Card Read Directory File\r\n");
	printf("----------------------\r\n\r\n");

	res = f_mount(&FatFs, "", 1);
	res = f_opendir(&dirs, "0:/");
	res = f_readdir(&dirs, &finfo);

	while (res != FR_OK) {
		printf(" FatFs failed!!\r\n");

		while (1) {
			LED_R = ~LED_R;
			vTaskDelay(200);
		}
	}

	printf(" File name : \r\n");

	while (finfo.fname[0]) {
		f_readdir(&dirs, &finfo);

		if (!finfo.fname[0])
			break;

		printf(" ... ");
		printf("%s\r\n", finfo.fname);
	}

	/*  // Wait
	  while(KEY != 1){
	    LED_G = ~LED_G;
	    vTaskDelay(200);
	  }*/

	// Write File
	printf(" f_open ... ");
	res = f_open(&file, "SDCard_K.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);

	if (res == FR_OK)
		printf("OK!!\r\n");
	else
		printf("failed!!\r\n");

	printf(" f_write ... ");
	res = f_write(&file, WriteData, 20, (UINT *)&i);

	if (res == FR_OK)
		printf("OK!!\r\n");
	else
		printf("failed!!\r\n");

	file.fptr = 0;

	printf(" f_read ... ");
	res = f_read(&file, ReadBuf, 100, (UINT *)&i);

	if (res == FR_OK)
		printf("OK!!\r\n");
	else
		printf("failed!!\r\n");

	f_close(&file);

	printf("%s", ReadBuf);

	while (1) {
		LED_G = ~LED_G;
		vTaskDelay(200);
	}
}
