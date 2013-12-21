#ifndef __STRING_H
#define __STRING_H

#include <stddef.h>

char *strchr(const char *s, int c);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
size_t strlen(const char *s);
int strcmp(const char* s1, const char* s2);
char *strcat(char* dst, char* src);

#endif
