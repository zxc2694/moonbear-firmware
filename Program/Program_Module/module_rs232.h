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
void putch_base(char str);
int printf(const char *format, ...);
int puts(const char* msg);
int gets(void);


/*=====================================================================================================*/
/*=====================================================================================================*/
#endif
