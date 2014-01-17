#ifndef __MODULE_RS232_H
#define __MODULE_RS232_H

typedef struct {
	char(*getc)(void);
	void (*putc)(char str);
	int (*puts)(const char *msg);
	int (*gets)(void);
} SERIAL;

extern SERIAL serial;
void putc_base(char str);
char getc_base(void);
int puts_base(const char *msg);
int gets_base(void);

void RS232_Config(void);

int printf(const char *format, ...);
double atof(const char *s);

#endif
