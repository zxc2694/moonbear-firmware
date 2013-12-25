#ifndef __LINENOISE_H
#define __LINENOISE_H

#include <stddef.h>

typedef struct linenoiseCompletions {
  size_t len;
  char **cvec;
} linenoiseCompletions;

typedef void(linenoiseCompletionCallback)(const char *, linenoiseCompletions *);
void linenoiseSetCompletionCallback(linenoiseCompletionCallback *);
void linenoiseAddCompletion(linenoiseCompletions *, char *);

char *linenoise(const char *prompt);
int linenoiseHistoryAdd(const char *line);
void linenoiseClearScreen(void);

#endif

