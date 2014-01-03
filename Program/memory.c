#include <stddef.h>
#include "FreeRTOS.h"

#include "string.h"
#include "memory.h"

void *realloc(void *ptr, size_t len)
{
        void *new_ptr;
        new_ptr = (void*)pvPortMalloc(len);

        if(new_ptr) {
                memcpy(new_ptr, ptr, len);
                vPortFree(ptr);
                return new_ptr;
        }

	return NULL;
}

