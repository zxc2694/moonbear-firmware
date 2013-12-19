/* #include "module_rs232.h" */

#ifndef __MODULE_RS232_H
#define __MODULE_RS232_H

#include "stm32f4xx.h"
/*=====================================================================================================*/
/*=====================================================================================================*/
typedef struct {
    uint16_t (*getch) ();		 //If declare as getc will cause naming conflict
    void (*putch) (char str); 	 //If declare as putc will cause naming conflict
} serial_ops;

typedef struct{
	char str;
} serial_msg;

void RS232_Config(void);
void RS232_DMA_Config(void);
void RS232_SendStr(USART_TypeDef *USARTx, char *pWord);
void RS232_SendNum(USART_TypeDef *USARTx, u8 Type, u8 NumLen, s32 SendData);
int RS232_SendData(USART_TypeDef *USARTx, char *SendData, u16 DataLen);
void RS232_RecvStr(USART_TypeDef *USARTx, char *pWord);
void RS232_RecvData(USART_TypeDef *USARTx, u8 *RecvData, u16 DataLen);
void RS232_VisualScope(USART_TypeDef *, u8 *, u16);
uint16_t getch_base(void);
void putch_base(char str);
int printf(const char *format, ...);
int puts(const char* msg);
int gets(void);


/*=====================================================================================================*/
/*=====================================================================================================*/
#endif
