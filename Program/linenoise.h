#ifndef __LINENOISE_H
#define __LINENOISE_H

typedef struct linenoiseCompletions {
  int len;
  char **cvec;
} linenoiseCompletions;


typedef void(linenoiseCompletionCallback)(const char *, linenoiseCompletions *);
void linenoiseSetCompletionCallback(linenoiseCompletionCallback *);
void linenoiseAddCompletion(linenoiseCompletions *, char *);

char *linenoise(const char *prompt);
//int linenoiseHistoryAdd(const char *line);
void linenoiseClearScreen(void);
//int linenoiseGetHistory();

#endif
