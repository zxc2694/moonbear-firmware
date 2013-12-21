#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#define ALIGN (sizeof(size_t))
#define ONES ((size_t)-1/UCHAR_MAX)                                                                      
#define HIGHS (ONES * (UCHAR_MAX/2+1))
#define HASZERO(x) ((x)-ONES & ~(x) & HIGHS)

#define SS (sizeof(size_t))

char *strchr(const char *s, int c)
{
        for (; *s && *s != c; s++);
        return (*s == c) ? (char *)s : NULL;
}

char *strcpy(char *dest, const char *src)
{
        const unsigned char *s = src;
        unsigned char *d = dest;
        while ((*d++ = *s++));
        return dest;
}

char *strncpy(char *dest, const char *src, size_t n)
{
        const unsigned char *s = src;
        unsigned char *d = dest;
        while (n-- && (*d++ = *s++));
        return dest;
}

size_t strlen(const char *s)
{
        const char *a = s;
        const size_t *w;
        for (; (uintptr_t) s % ALIGN; s++)
                if (!*s) return (s - a);
        for (w = (const void *) s; !HASZERO(*w); w++);
        for (s = (const void *) w; *s; s++);
        return (s - a);
}

int strcmp(const char* s1, const char* s2)
{
        while(*s1 && (*s1==*s2))
                s1++,s2++;
        return *(const unsigned char*)s1-*(const unsigned char*)s2;
}

char *strcat(char* dst, char* src)
{
        if(src == NULL)
                return NULL;

        char* retAddr = dst;

        /* --- Find last position --- */
        while (*dst++ != '\0');

        dst--;
        while (*dst++ = *src++);
        return retAddr;
}
