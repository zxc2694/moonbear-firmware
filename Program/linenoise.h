#ifndef __LINENOISE_H
#define __LINENOISE_H

<<<<<<< HEAD
typedef struct linenoiseCompletions {
  int len;
  char **cvec;
} linenoiseCompletions;


=======
#include <stddef.h>

typedef struct linenoiseCompletions {
  size_t len;
  char **cvec;
} linenoiseCompletions;

>>>>>>> 75a4793e0c8584e9416085ca6eb247a83ef9e8f4
typedef void(linenoiseCompletionCallback)(const char *, linenoiseCompletions *);
void linenoiseSetCompletionCallback(linenoiseCompletionCallback *);
void linenoiseAddCompletion(linenoiseCompletions *, char *);

char *linenoise(const char *prompt);
<<<<<<< HEAD
//int linenoiseHistoryAdd(const char *line);
void linenoiseClearScreen(void);
//int linenoiseGetHistory();

#endif
=======
int linenoiseHistoryAdd(const char *line);
void linenoiseClearScreen(void);

#endif

>>>>>>> 75a4793e0c8584e9416085ca6eb247a83ef9e8f4
