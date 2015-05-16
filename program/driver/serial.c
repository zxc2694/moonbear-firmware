#include <unistd.h>
#include <stdarg.h>
#include <ctype.h>

#include "string.h"
#include "stdlib.h"

#include "QuadCopterConfig.h"

/* Serial Initializaton ------------------------------------------------------*/

/**
  * @brief  USART3 Serial port initialization
  * @param  None
  * @retval None
  */
void Serial_Config(int buadrate) /* Tx:Pb10, Rx:Pb11 */
{
	/* RCC Initialization */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,  ENABLE);
	//GPIO使用之宣告，來自stm32f4xx_rcc.c，使用腳位PB系列
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	//USART使用之宣告，來自stm32f4xx_rcc.c，使用USART第三組

	/* GPIO Initialization */
	GPIO_InitTypeDef GPIO_InitStruct = {
		.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11, 		//腳位設定
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_OType = GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_UP,
		.GPIO_Speed = GPIO_Speed_50MHz
	};

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
	// GPIO使用之宣告，來自stm32f4xx_gpio.c
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* USART3 Initialization */
	USART_InitTypeDef USART_InitStruct = {
		.USART_BaudRate = buadrate, 				//設定鮑率
		.USART_WordLength = USART_WordLength_8b,
		.USART_StopBits = USART_StopBits_1,
		.USART_Parity = USART_Parity_No,
		.USART_HardwareFlowControl = USART_HardwareFlowControl_None,
		.USART_Mode = USART_Mode_Rx | USART_Mode_Tx 	//設定模式
	};

	USART_Init(USART3, &USART_InitStruct);
	USART_Cmd(USART3, ENABLE);
	USART_ClearFlag(USART3, USART_FLAG_TC);
	// USART初始函數，來自stm32f4xx_usart.c

	/* NVIC Initialization */
	USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

	NVIC_InitTypeDef NVIC_InitStruct = {
		.NVIC_IRQChannel = USART3_IRQn,
		.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1,
		.NVIC_IRQChannelSubPriority = 0,
		.NVIC_IRQChannelCmd = ENABLE
	};
	NVIC_Init(&NVIC_InitStruct);
}

void USART3_IRQHandler()
{
	long lHigherPriorityTaskWoken = pdFALSE;

	serial_msg rx_msg;

	if (USART_GetITStatus(USART3, USART_IT_TXE) != RESET) {
		xSemaphoreGiveFromISR(serial_tx_wait_sem, &lHigherPriorityTaskWoken);

		USART_ITConfig(USART3, USART_IT_TXE, DISABLE);

	} else if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) {
		rx_msg.ch = USART_ReceiveData(USART3);

		if (!xQueueSendToBackFromISR(serial_rx_queue, &rx_msg, &lHigherPriorityTaskWoken))
			portEND_SWITCHING_ISR(lHigherPriorityTaskWoken);

	} else {
		while (1);
	}

	portEND_SWITCHING_ISR(lHigherPriorityTaskWoken);
}

/* Serial Structure ----------------------------------------------------------*/

/* Serial structure */
SERIAL serial = {
	.getc = getc_base,
	.putc = putc_base,
	.gets = gets_base,
	.puts = puts_base,
	.printf = printf_base
};

/**
  * Function name : serial.getc (function base)
  * @brief  get a char from the serial char queue
  * @param  None
  * @retval None
  */
char getc_base(void)
{
	serial_msg msg;

	while (!xQueueReceive(serial_rx_queue, &msg, portMAX_DELAY));

	return msg.ch;
}

/**
  * Function name : serial.putc (function base)
  * @brief  send a char through the serial
  * @param  None
  * @retval None
  */
void putc_base(char str)
{
	while (!xSemaphoreTake(serial_tx_wait_sem, portMAX_DELAY));

	USART_SendData(USART3, (uint16_t)str);
	USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
}

/**
  * Function name : serial.gets (function base)
  * @brief  get a string from the serial
  * @param  None
  * @retval None
  */
int gets_base(void)
{
	char str;
	str = serial.getc();
	serial.printf("%c", str);
	return 1;
}

/**
  * Function name : serial.puts (function base)
  * @brief  send a string through the serial
  * @param  None
  * @retval None
  */
int puts_base(const char *msg)
{
	for (; *msg; msg++)
		serial.putc(*msg);

	return 1;
}


int printf_base(const char *format, ...)
{
	char str[128];
	va_list para;
	va_start(para, format);
	int curr_pos = 0;
	char ch[] = {'0', '\0'};
	char integer[11];
	str[0] = '\0';

	while (format[curr_pos] != '\0') {
		if (format[curr_pos] != '%') {
			ch[0] = format[curr_pos];
			strcat(str, ch);

		} else {
			switch (format[++curr_pos]) {
			case 's':
				strcat(str, va_arg(para, char *));
				break;

			case 'c':
				ch[0] = (char)va_arg(para, int);
				strcat(str, ch);
				break;

			case 'i':
			case 'f':
				strcat(str, ftoa(va_arg(para, double)));
				break;

			case 'd':
				strcat(str, itoa(va_arg(para, int), integer));
				break;

			case 'u':
				strcat(str, itoa(va_arg(para, unsigned), integer));
				break;

			default:
				break;
			}
		}

		curr_pos++;
	}

	va_end(para);
	serial.puts(str);
	return 1;
}
