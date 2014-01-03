/* #include "module_rs232.h" */

#ifndef __MODULE_RS232_H
#define __MODULE_RS232_H

#include "stm32f4xx.h"
/*=====================================================================================================*/
/*=====================================================================================================*/
typedef struct {
	char(*getch)();		   //If declare as getc will cause naming conflict
	void (*putch)(char str); 	  //If declare as putc will cause naming conflict
} serial_ops;

extern serial_ops serial;

void RS232_Config(void);
void putch_base(char str);
char getch_base(void);
int printf(const char *format, ...);
int putstr(const char *msg);  //If declare as puts will cause naming conflict
int getstr(void);             //If declare as puts will cause naming conflict

double atof(const char *s);
/*=====================================================================================================*/
/*=====================================================================================================*/
#endif
