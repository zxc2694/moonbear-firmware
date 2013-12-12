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
	float c = 5.04;
	int d =-23;
	int e =56;
	printf("printf\n\r");
	printf("%f\n\r", c);
	printf("%d\n\r", d);
	printf("%d\n\r", e);


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
