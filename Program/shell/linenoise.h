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

enum KEY_ACTION {
	TAB = 9,            /* tab */
	ENTER = 13,         /* enter */
	CTRL_C = 3,         /* ctrl-c */
	BACKSPACE =  127,   /* backspace */
	CTRL_H = 8,         /* ctrl-h */
	CTRL_D = 4,         /* ctrl-d */
	CTRL_T = 20,        /* ctrl-t */
	CTRL_B = 2,         /* ctrl-b */
	CTRL_F = 6,         /* ctrl-f */
	CTRL_P = 16,        /* ctrl-p */
	CTRL_N = 14,        /* ctrl-n */
	ESC = 27,           /* escape */
	ARROW_PREFIX = 91,  /* any ARROW = ESC + '[' + 'A' ~ 'D' */
	LEFT_ARROW = 68,    /* last character of left arrow */
	RIGHT_ARROW = 67,   /* last character of right arrow */
	UP_ARROW = 65,      /* last character of up arrow */
	DOWN_ARROW = 66,    /* last character of down arrow */
	NULL_CH = 0,        /* NULL character */
	CTRL_U = 21,        /* ctrl+u */
	CTRL_K = 11,        /* ctrl+k */
	CTRL_A = 1,         /* ctrl+a */
	CTRL_E = 5,         /* ctrl+e */
	CTRL_L = 12,        /* ctrl+l */
	CTRL_W = 23,         /* ctrl+w */
	SPACE = 32
};

#endif

