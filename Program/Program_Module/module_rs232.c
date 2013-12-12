/*=====================================================================================================*/
/*=====================================================================================================*/
#include "stm32f4_system.h"
#include "stm32f4_usart.h"
#include "module_rs232.h"
#include "algorithm_string.h"
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : RS232_Config
**功能 :
**輸入 :
**輸出 :
**使用 :
**=====================================================================================================*/
/*=====================================================================================================*/
void RS232_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,  ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

	/* USART1 Tx PB10 */	/* USART1 Rx PB11 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	USART_InitStruct.USART_BaudRate = 9600;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStruct);
	USART_Cmd(USART3, ENABLE);

	USART_ClearFlag(USART1, USART_FLAG_TC);
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : RS232_SendStr
**功能 : 發送字串
**輸入 : USARTx, *pWord
**輸出 : None
**使用 : RS232_SendStr(USART1, (u8*)"Hellow World!");
**=====================================================================================================*/
/*=====================================================================================================*/
void RS232_SendStr(USART_TypeDef *USARTx, char *pWord)
{
	while (*pWord != '\0') {
		USART_SendByte(USARTx, (char *)pWord);
		pWord++;
	}
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : RS232_SendNum
**功能 : 將數值轉字串發送
**輸入 : USARTx, Type, NumLen, SendData
**輸出 : None
**使用 : RS232_SendNum(USART1, Type_O, 6, 1024);
**=====================================================================================================*/
/*=====================================================================================================*/
void RS232_SendNum(USART_TypeDef *USARTx, u8 Type, u8 NumLen, s32 SendData)
{
	u8 TrData[32] = {0};
	u8 *pWord = TrData;

	Str_NumToChar(Type, NumLen, TrData, SendData);

	while (*pWord != '\0') {
		USART_SendByte(USARTx, (char *)pWord);
		pWord++;
	}
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : RS232_SendData
**功能 : 發送資料
**輸入 : USARTx, *SendData, DataLen
**輸出 : None
**使用 : RS232_SendData(USART1, SendData, DataLen);
**=====================================================================================================*/
/*=====================================================================================================*/
int RS232_SendData(USART_TypeDef *USARTx, char *SendData, u16 DataLen)
{
	do {
		USART_SendByte(USARTx, SendData);
		SendData++;
		DataLen--;
	} while (DataLen);

	return 1;
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : RS232_RecvStr
**功能 : 接收字串
**輸入 : USARTx, *pWord
**輸出 : None
**使用 : RS232_RecvStr(USART1, Stirng);
**=====================================================================================================*/
/*=====================================================================================================*/
void RS232_RecvStr(USART_TypeDef *USARTx, char *pWord)
{
	do {
		*pWord = USART_RecvByte(USARTx);
		pWord++;
	} while (*(pWord - 1) != '\0');

	*pWord = '\0';
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : RS232_RecvData
**功能 : 接收資料
**輸入 : USARTx, *RecvData, DataLen
**輸出 : None
**使用 : RS232_RecvData(USART1, RecvData, DataLen);
**=====================================================================================================*/
/*=====================================================================================================*/
void RS232_RecvData(USART_TypeDef *USARTx, u8 *RecvData, u16 DataLen)
{
	do {
		*RecvData = USART_RecvByte(USARTx);
		RecvData++;
		DataLen--;
	} while (DataLen);
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : RS232_VisualScope_CRC16
**功能 :
**輸入 :
**輸出 :
**使用 :
**=====================================================================================================*/
/*=====================================================================================================*/
static u16 RS232_VisualScope_CRC16(u8 *Array, u16 Len)
{
	u16 USART_IX, USART_IY, USART_CRC;

	USART_CRC = 0xffff;

	for (USART_IX = 0; USART_IX < Len; USART_IX++) {
		USART_CRC = USART_CRC ^ (u16)(Array[USART_IX]);

		for (USART_IY = 0; USART_IY <= 7; USART_IY++) {
			if ((USART_CRC & 1) != 0)
				USART_CRC = (USART_CRC >> 1) ^ 0xA001;
			else
				USART_CRC = USART_CRC >> 1;
		}
	}

	return (USART_CRC);
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : RS232_VisualScope
**功能 :
**輸入 :
**輸出 :
**使用 :
**=====================================================================================================*/
/*=====================================================================================================*/
void RS232_VisualScope(USART_TypeDef *USARTx, u8 *pWord, u16 Len)
{
	u8 i = 0;
	u16 Temp = 0;

	Temp = RS232_VisualScope_CRC16(pWord, Len);
	pWord[8] = Temp & 0x00ff;
	pWord[9] = (Temp & 0xff00) >> 8;

	for (i = 0; i < 10; i++) {
		USART_SendData(USARTx, *pWord);

		while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);

		pWord++;
	}
}

/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : itoa
**功能 : Support the sprintf & printf function
**輸入 :
**輸出 :
**使用 :
**=====================================================================================================*/
/*=====================================================================================================*/
char *itoa(int value, char *str)
{
	int base = 10;
	int divideNum = base;
	int i = 0;

	if (value < 0) {
		str[0] = '-';
		i++;
		value*=-1;
	}

	while (value / divideNum > 0) {
		divideNum *= base;
	}

	

	while (divideNum / base > 0) {
		divideNum /= base;
		str[i++] = value / divideNum + 48;
		value %= divideNum;
	}

	str[i] = '\0';
	return str;

}

void ltoa(char *buf, unsigned long i, int base)
{
        char *s;
        #define LEN        25
        int rem;
        char rev[LEN+1];

        if (i == 0)
                s = "0";
        else
                {
                rev[LEN] = 0;
                s = &rev[LEN];
                while (i)
                        {
                        rem = i % base;
                        if (rem < 10)
                                *--s = rem + '0';
                        else if (base == 16)
                                *--s = "abcdef"[rem - 10];
                        i /= base;
                        }
                }
        strcpy(buf, s);
}

typedef union {
long        L;
float        F;
}         LF_t;

char *ftoa(float f) //, int *status)
{
        long mantissa, int_part, frac_part;
        short exp2;
        LF_t x;
        char *p;
        static char outbuf[15];

        //*status = 0;
        if (f == 0.0)
        {
                outbuf[0] = '0';
                outbuf[1] = '.';
                outbuf[2] = '0';
                outbuf[3] = 0;
                return outbuf;
        }
        x.F = f;

        exp2 = (unsigned char)(x.L >> 23) - 127;
        mantissa = (x.L & 0xFFFFFF) | 0x800000;
        frac_part = 0;
        int_part = 0;

        if (exp2 >= 31)
        {
                //*status = _FTOA_TOO_LARGE;
                return 0;
        }
        else if (exp2 < -23)
        {
                //*status = _FTOA_TOO_SMALL;
                return 0;
        }
        else if (exp2 >= 23)
        int_part = mantissa << (exp2 - 23);
        else if (exp2 >= 0)
        {
                int_part = mantissa >> (23 - exp2);
                frac_part = (mantissa << (exp2 + 1)) & 0xFFFFFF;
        }
        else /* if (exp2 < 0) */
        frac_part = (mantissa & 0xFFFFFF) >> -(exp2 + 1);

        p = outbuf;

        if (x.L < 0)
                *p++ = '-';

        if (int_part == 0)
                *p++ = '0';
        else
        {
                ltoa(p, int_part, 10);
                while (*p)
                p++;
        }
        *p++ = '.';

        if (frac_part == 0)
                *p++ = '0';
        else
        {
                char m, max;

                max = sizeof (outbuf) - (p - outbuf) - 1;
                if (max > 7)
                        max = 7;
                /* print BCD */
                for (m = 0; m < max; m++)
                {
                        /* frac_part *= 10;        */
                        frac_part = (frac_part << 3) + (frac_part << 1);

                        *p++ = (frac_part >> 24) + '0';
                        frac_part &= 0xFFFFFF;
                }
                /* delete ending zeroes */
                for (--p; p[0] == '0' && p[-1] != '.'; --p)
                        ;
                        ++p;
        }
        *p = 0;

        return outbuf;
}


/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : sprintf
**功能 :
**輸入 : ( char * str, const char * format, ... )
**輸出 : strlen(str)
**使用 :
**=====================================================================================================*/
/*=====================================================================================================*/

int sprintf(char *str, const char *format, ...)
{
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
	return strlen(str);
}

/*=====================================================================================================*
**函數 : printf
**功能 :
**輸入 :
**輸出 : RS232_SendData(USART3, str, strlen(str));
**使用 :
**=====================================================================================================*/
/*=====================================================================================================*/

int printf(const char *format, ...)
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
	return RS232_SendData(USART3, str, strlen(str));
}