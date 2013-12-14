#include "stm32f4_system.h"
#include "stm32f4_usart.h"
#include "module_rs232.h"
#include "algorithm_string.h"
#include <unistd.h>
#include <stdarg.h>
/*=====================================================================================================*
**函數 : test_printf
**功能 : 
**輸入 : 
**輸出 : 
**使用 : 
**=====================================================================================================*/

void test_printf(void)
{
	/*test printf can support USART*/
	float f1 = 5.04, f2 = 0xFFFF, f3=0xFFFFFFFF-1;
	int i1 = -23, i2 =56, i3 =0xFFFF;
	printf("test printf!\n\r");
	printf("float: 5.04=%f, 0xFFFF = %f, 0xFFFFFFF = %f\n\r", f1,f2,f3);
	printf("int: -23 = %d, 56 = %d, 0xFFFF = %d\n\r", i1, i2, i3);

	

}

/*=====================================================================================================*
**函數 : test_txrx
**功能 : 
**輸入 : 
**輸出 : 
**使用 : 
**=====================================================================================================*/

void test_TXRX(void)
{

	RS232_SendStr(USART3, "abcde");
}
