#ifndef __MEMORY_H
#define __MEMORY_H

#include <stddef.h>

void *malloc(size_t size);
void free(void *p);
void *realloc(void *ptr, size_t len);

#endif
