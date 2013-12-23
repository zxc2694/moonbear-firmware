#include <unistd.h>
<<<<<<< HEAD
#include <stdlib.h>
//#include <stdio.h>
//#include <errno.h>
#include <string.h>
//#include <stdlib.h>
//#include <unistd.h>
#include "linenoise.h"
/*FreeRTOS relative */
#include "FreeRTOS.h"
#include "module_rs232.h"

#define LINENOISE_DEFAULT_HISTORY_MAX_LEN 100
#define LINENOISE_MAX_LINE 1024 //4096 is too much to this environment, it will crash!

extern serial_ops serial;
enum key_action{
    TAB = 9,            /*tab*/
    ENTER = 13,         /* enter */
    CTRL_C = 3,         /* ctrl-c*/
    BACKSPACE =  127,   /* backspace */ 
    CTRL_H = 8,         /* ctrl-h */
    CTRL_D = 4,         /* ctrl-d, remove char at right of cursor, or of the*/
    CTRL_T = 20,        /* ctrl-t*/
    CTRL_B = 2,         /* ctrl-b*/
    CTRL_F = 6,         /* ctrl-f*/
    CTRL_P = 16,        /* ctrl-p */
    CTRL_N = 14,        /* ctrl-n */     
    ESC = 27,           /* escape sequence */
    ARROW_PREFIX = 91,  /*any ARROW = ESC + [ + (65~68) */
    LEFT_ARROW = 68,    /*last character of left arrow*/
    RIGHT_ARROW = 67,   /*last character of right arrow*/
    UP_ARROW = 65,      /*last character of up arrow*/
    DOWN_ARROW = 66,    /*last character of down arrow*/
    NULL_CH = 0,    /*NULL character*/
    CTRL_U = 21,    /*Ctrl+U*/
    CTRL_K = 11,    /*Ctrl+k*/
    CTRL_A = 1,     /*Ctrl+a*/
    CTRL_E = 5,     /*Ctrl+e*/
    CTRL_L = 12,    /*Ctrl+l*/
    CTRL_W = 23     /*Ctrl+w*/

};
static linenoiseCompletionCallback *completionCallback = NULL;
static int mlmode = 0;  /* Multi line mode. Default is single line. */
static int history_max_len = LINENOISE_DEFAULT_HISTORY_MAX_LEN;
static int history_len = 0;
char **history = NULL;
=======
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "module_rs232.h"
#include "memory.h"
#include "linenoise.h"  


#define LINENOISE_DEFAULT_HISTORY_MAX_LEN 100
#define LINENOISE_MAX_LINE 256 //4096 is too much to this environment, it will crash!
>>>>>>> 75a4793e0c8584e9416085ca6eb247a83ef9e8f4

struct linenoiseState {
    char *buf;          /* Edited line buffer. */
    size_t buflen;      /* Edited line buffer size. */
    const char *prompt; /* Prompt to display. */
    size_t plen;        /* Prompt length. */
    size_t pos;         /* Current cursor position. */
    size_t oldpos;      /* Previous refresh cursor position. */
    size_t len;         /* Current edited line length. */
    size_t cols;        /* Number of columns in terminal. */
    size_t maxrows;     /* Maximum num of rows used so far (multiline mode) */
    int history_index;  /* The history index we are currently editing. */
};

<<<<<<< HEAD
static void refreshLine(struct linenoiseState *l);



void linenoiseClearScreen(void) {

    puts("\x1b[H\x1b[2J");

=======
enum KEY_ACTION{
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
    CTRL_W = 23         /* ctrl+w */
};

static linenoiseCompletionCallback *completionCallback = NULL;
static void refreshLine(struct linenoiseState *l);

static int mlmode = 0;  /* Multi line mode. Default is single line. */
static int history_max_len = LINENOISE_DEFAULT_HISTORY_MAX_LEN;
static int history_len = 0;
char **history = NULL;


void linenoiseClearScreen(void) {
    putstr("\x1b[H\x1b[2J");
>>>>>>> 75a4793e0c8584e9416085ca6eb247a83ef9e8f4
}

static void freeCompletions(linenoiseCompletions *lc) {
    size_t i;
    for (i = 0; i < lc->len; i++)  
        vPortFree(lc->cvec[i]);    
    if (lc->cvec != NULL)
<<<<<<< HEAD
       vPortFree(lc->cvec);
}


static void linenoiseBeep(void) {
    puts("\x7");
=======
        vPortFree(lc->cvec);
}

static void linenoiseBeep(void) {
    putstr("\x7");
>>>>>>> 75a4793e0c8584e9416085ca6eb247a83ef9e8f4
}

static int completeLine(struct linenoiseState *ls) {
    linenoiseCompletions lc = { 0, NULL };
    int nwritten = 0;
    char c = 0;

    completionCallback(ls->buf,&lc);
    if (lc.len == 0) {
        linenoiseBeep();
    } else {
	    size_t stop = 0, i = 0;

    	while(!stop) {
    	     /* Show completion or original buffer */
            if (i < lc.len) {
                struct linenoiseState saved = *ls;

                ls->len = ls->pos = strlen(lc.cvec[i]);
                ls->buf = lc.cvec[i];
                refreshLine(ls);
                ls->len = saved.len;
                ls->pos = saved.pos;
                ls->buf = saved.buf;
            } else {
                refreshLine(ls);
            }

	       c = serial.getch();

            switch(c) {
                case TAB: /* tab */
                    i = (i+1) % (lc.len+1);
                    if (i == lc.len) linenoiseBeep();
                    break;
                case ESC: /* escape */
                    /* Re-show original buffer */
                    if (i < lc.len) refreshLine(ls);
                    stop = 1;
                    break;
                default:
                    /* Update buffer and return */
                    if (i < lc.len) {
            			for(nwritten = 0; nwritten < ls->buflen; nwritten++) {
            				ls->buf[nwritten] = lc.cvec[i][nwritten];
            				if(lc.cvec[i][nwritten] == '\0')
            					break;	
            			}

                            ls->len = ls->pos = nwritten;
                    }
                    stop = 1;
                    break;
            }
        }
    }

    freeCompletions(&lc);
    return c; /* Return last read character */
}

void linenoiseSetCompletionCallback(linenoiseCompletionCallback *fn) {
    completionCallback = fn;
}

void linenoiseAddCompletion(linenoiseCompletions *lc, char *str) {     
    size_t len = strlen(str);
    char *copy = (char *)pvPortMalloc(len+1);
    memcpy(copy,str,len+1);
<<<<<<< HEAD
    lc->cvec = (char**)pvPortRealloc(lc->cvec,sizeof(char*)*(lc->len+1));
    lc->cvec[lc->len++] = copy;                                        
=======
    lc->cvec = (char**)realloc(lc->cvec,sizeof(char*)*(lc->len+1));
    lc->cvec[lc->len++] = copy;                                    
>>>>>>> 75a4793e0c8584e9416085ca6eb247a83ef9e8f4
}

static void refreshSingleLine(struct linenoiseState *l) {
    size_t plen = strlen(l->prompt);
    char *buf = l->buf;
    size_t len = l->len;
    size_t pos = l->pos;

    while((plen+pos) >= l->cols) {
        buf++;
        len--;
        pos--;
    }
    while (plen+len > l->cols) {
        len--;
    }

<<<<<<< HEAD
    /* Cursor to left edge ->ESC [ 0 G*/ 
    puts("\x1b[0G");
    /* Write the prompt and the current buffer content */
    puts(l->prompt);
    puts(buf);
    /* Erase to right -> ESC [ 0 K*/ 
    puts("\x1b[0K");
    /* \x1b[0G->Move cursor to original position(col=0). */
    /* \x1b[00c->Set the count of moving cursor(col=pos+plen) */
    char sq1[] = "\x1b[0G";
    char sq2[] = "\x1b[00C";
    sq2[2] = (pos+plen) / 10 + 0x30; 
    sq2[3] = (pos+plen) % 10 + 0x30;
    puts(sq1);
    puts(sq2);
=======
    /* Cursor to left edge */ 
    putstr("\x1b[0G");
    /* Write the prompt and the current buffer content */
    putstr(l->prompt);
    putstr(buf);
    /* Erase to right */
    putstr("\x1b[0K");
    /* Move cursor to original position. */
    char sq[] = "\x1b[0G\x1b[00C"; //the max columes of Terminal environment is 80
    /* Set the count of moving cursor */
    sq[6] = (pos+plen) / 10 + 0x30;  
    sq[7] = (pos+plen) % 10 + 0x30;
    putstr(sq);
>>>>>>> 75a4793e0c8584e9416085ca6eb247a83ef9e8f4
}

static void refreshLine(struct linenoiseState *l) {
    if (mlmode) {
        //refreshMultiLine(l);
    } else {
        refreshSingleLine(l);
    }
}

int linenoiseEditInsert(struct linenoiseState *l, int c) {
    if (l->len < l->buflen) {
        if (l->len == l->pos) {
            l->buf[l->pos] = c;
            l->pos++;
            l->len++;
            l->buf[l->len] = '\0';
            if ((!mlmode && l->plen+l->len < l->cols) /* || mlmode */) {
<<<<<<< HEAD
                    /* Avoid a full update of the line in the
                     * trivial case. */
        		serial.putch(c);

        	} else {

        		refreshLine(l);

    	    }
    	}
    } else {
        memmove(l->buf+l->pos+1,l->buf+l->pos,l->len-l->pos);
        l->buf[l->pos] = c;
        l->len++;
        l->pos++;
        l->buf[l->len] = '\0';
	    refreshLine(l);
    }
    return 0;
=======
                /* Avoid a full update of the line in the
                 * trivial case. */
                serial.putch(c);
            } else {
                refreshLine(l);
            }
	} else {
            memmove(l->buf+l->pos+1,l->buf+l->pos,l->len-l->pos);
            l->buf[l->pos] = c;
            l->len++;
            l->pos++;
            l->buf[l->len] = '\0';
            refreshLine(l);
        }
    }
>>>>>>> 75a4793e0c8584e9416085ca6eb247a83ef9e8f4
}

void linenoiseEditMoveLeft(struct linenoiseState *l) {
    if (l->pos > 0) {
        l->pos--;
        refreshLine(l);
    }
}

void linenoiseEditMoveRight(struct linenoiseState *l) {
    if (l->pos != l->len) {
        l->pos++;
        refreshLine(l);
    }
}

#define LINENOISE_HISTORY_NEXT 0
#define LINENOISE_HISTORY_PREV 1
void linenoiseEditHistoryNext(struct linenoiseState *l, int dir) {
    if (history_len > 1) {
        /* Update the current history entry before to
         * overwrite it with the next one. */
        vPortFree(history[history_len - 1 - l->history_index]);
        history[history_len - 1 - l->history_index] = strdup(l->buf);
        /* Show the new entry */       
        l->history_index += (dir == LINENOISE_HISTORY_PREV) ? 1 : -1;
        if (l->history_index < 0) {    
            l->history_index = 0;          
            return;
        } else if (l->history_index >= history_len) {
            l->history_index = history_len-1;
            return;
        }
        strncpy(l->buf,history[history_len - 1 - l->history_index],l->buflen);
        l->buf[l->buflen-1] = '\0';    
        l->len = l->pos = strlen(l->buf);
        refreshLine(l);       
    }  
}

void linenoiseEditDelete(struct linenoiseState *l) {
    if (l->len > 0 && l->pos < l->len) {
        memmove(l->buf+l->pos,l->buf+l->pos+1,l->len-l->pos-1);
        l->len--;
        l->buf[l->len] = '\0';
        refreshLine(l);
    }
}

void linenoiseEditBackspace(struct linenoiseState *l) {
    if (l->pos > 0 && l->len > 0) {
        memmove(l->buf+l->pos-1,l->buf+l->pos,l->len-l->pos);
        l->pos--;
        l->len--;
        l->buf[l->len] = '\0';
        refreshLine(l);
    }
}

void linenoiseEditDeletePrevWord(struct linenoiseState *l) {
    size_t old_pos = l->pos;
    size_t diff;

    while (l->pos > 0 && l->buf[l->pos-1] == ' ')
        l->pos--;
    while (l->pos > 0 && l->buf[l->pos-1] != ' ')
        l->pos--;
    diff = old_pos - l->pos;
    memmove(l->buf+l->pos,l->buf+old_pos,l->len-old_pos+1);
    l->len -= diff;
    refreshLine(l);
}

static int linenoiseEdit(char *buf, size_t buflen, const char *prompt)
{
    struct linenoiseState l = {
        .buf = buf, .buflen = buflen,
        .prompt = prompt, .plen = strlen(prompt),
        .oldpos = 0, .pos = 0,
        .len = 0,
        .cols = 80, .maxrows = 0,
        .history_index = 0,
    };

    /* Buffer starts empty. */
    buf[0] = '\0';
    buflen--; /* Make sure there is always space for the nulterm */

    /* The latest history entry is always our current buffer, that
     * initially is just an empty string. */
    linenoiseHistoryAdd("");
<<<<<<< HEAD
    puts(prompt);
    while(1) {
    	char c;
    	char seq[2] = {0};	
=======
    history_len++;

    putstr(prompt);
    while(1) {
    	char c;
    	char seq[2] = {0};	

>>>>>>> 75a4793e0c8584e9416085ca6eb247a83ef9e8f4
    	c = serial.getch();	

    	/* Only autocomplete when the callback is set. */
        if (c == 9 && completionCallback != NULL) {
            c = completeLine(&l);        
            /* Return on errors */
            if (c < 0) return l.len; 
            /* Read next character when 0 */
            if (c == 0) continue;      
        }

<<<<<<< HEAD
    	switch(c) {

        	case ENTER:    /* enter */
        	    history_len--;
				vPortFree(history[history_len]);
        	    return (int)l.len;	    
        	case BACKSPACE:   /* backspace */	
        	case CTRL_H:     /* ctrl-h */
        	    linenoiseEditBackspace(&l);
        	    break;
            case CTRL_D:     /* ctrl-d, remove char at right of cursor, or of the
                               line is empty, act as end-of-file. */
                if (l.len > 0) {
                    linenoiseEditDelete(&l);
                } else {
                    history_len--;
                    vPortFree(history[history_len]);
                    return -1;
                }
                break;
            case CTRL_T:    /* ctrl-t, swaps current character with previous. */
        	    //...
        	    break;
            case CTRL_B:     /* ctrl-b */
                linenoiseEditMoveLeft(&l);
                break;
            case CTRL_F:     /* ctrl-f */
                linenoiseEditMoveRight(&l);
                break;
            case CTRL_P:    /* ctrl-p */
                linenoiseEditHistoryNext(&l, LINENOISE_HISTORY_PREV);
                break;
            case CTRL_N:    /* ctrl-n */
                linenoiseEditHistoryNext(&l, LINENOISE_HISTORY_NEXT);
                break;
            case ESC:    /* escape sequence */
        	   seq[0] = serial.getch(); 
        	   seq[1] = serial.getch();
        	   /* Need to implement reading 2 bytes from USART at here */
                if (seq[0] == ARROW_PREFIX && seq[1] == LEFT_ARROW) {
                    /* Left arrow */
                    linenoiseEditMoveLeft(&l);
                } else if (seq[0] == ARROW_PREFIX && seq[1] == RIGHT_ARROW) {
                    /* Right arrow */
                    linenoiseEditMoveRight(&l);
    	        } else if (seq[0] == ARROW_PREFIX && (seq[1] == UP_ARROW || seq[1] == DOWN_ARROW)) {
                    /* Up and Down arrows */
                    linenoiseEditHistoryNext(&l,
                    (seq[1] == UP_ARROW) ? LINENOISE_HISTORY_PREV :
                                     LINENOISE_HISTORY_NEXT);
                } 
                break;
            default:
            	if(c == '\0') //avoid the NULL byte which received from the USART
            		break;
                if (linenoiseEditInsert(&l,c)) return -1;
            	    break;
            case CTRL_U: /* Ctrl+u, delete the whole line. */
                buf[0] = '\0';
                l.pos = l.len = 0;
                refreshLine(&l);
            	break;
            case CTRL_K: /* Ctrl+k, delete from current to end of line. */
                buf[l.pos] = '\0';
                l.len = l.pos;
                refreshLine(&l);
            	break;
            case CTRL_A: /* Ctrl+a, go to the start of the line */
                l.pos = 0;
                refreshLine(&l);
            	break;
            case CTRL_E: /* ctrl+e, go to the end of the line */
                l.pos = l.len;
                refreshLine(&l);
            	break;
            case CTRL_L: /* ctrl+l, clear screen */	
                linenoiseClearScreen();        
                refreshLine(&l);
                break;
            case CTRL_W: /* ctrl+w, delete previous word */
            	linenoiseEditDeletePrevWord(&l);
            	break;
    	}
=======
	switch(c) {
	case ENTER:    /* enter */
	    history_len--;
	    vPortFree(history[history_len]);
	    return (int)l.len;	    
	case BACKSPACE:   /* backspace */	
	case CTRL_H:     /* ctrl-h */
	    linenoiseEditBackspace(&l);
	    break;
        case CTRL_D:     /* ctrl-d, remove char at right of cursor, or of the
                            line is empty, act as end-of-file. */
            if (l.len > 0) {
                linenoiseEditDelete(&l);
            } else {
                history_len--;
                vPortFree(history[history_len]);
                return -1;
            }
            break;
        case CTRL_T:    /* ctrl-t, swaps current character with previous. */
            if (l.pos > 0 && l.pos < l.len) {
                int aux = buf[l.pos-1];
                buf[l.pos-1] = buf[l.pos];
                buf[l.pos] = aux;
                if (l.pos != l.len-1) l.pos++;
                refreshLine(&l);
            }
            break;
        case CTRL_B:     /* ctrl-b */
            linenoiseEditMoveLeft(&l);
            break;
        case CTRL_F:     /* ctrl-f */
            linenoiseEditMoveRight(&l);
            break;
        case CTRL_P:    /* ctrl-p */
	    linenoiseEditHistoryNext(&l, LINENOISE_HISTORY_PREV);
            break;
        case CTRL_N:    /* ctrl-n */
            linenoiseEditHistoryNext(&l, LINENOISE_HISTORY_NEXT);
            break;
	   /* escape sequence */
	case ESC:
	    seq[0] = serial.getch();
	    seq[1] = serial.getch();

            if (seq[0] == ARROW_PREFIX && seq[1] == LEFT_ARROW) {
                /* Left arrow */
                linenoiseEditMoveLeft(&l);
            } else if (seq[0] == ARROW_PREFIX && seq[1] == RIGHT_ARROW) {
                /* Right arrow */
                linenoiseEditMoveRight(&l);
            } else if (seq[0] == ARROW_PREFIX && (seq[1] == UP_ARROW || seq[1] == DOWN_ARROW)) {
                /* Up and Down arrows */
                linenoiseEditHistoryNext(&l,
                    (seq[1] == UP_ARROW) ? LINENOISE_HISTORY_PREV :
                                     LINENOISE_HISTORY_NEXT);
            }
	    //Here is also a extanded escape!
	    break;
	default:
            linenoiseEditInsert(&l,c);
	    break;
        case CTRL_U: /* Ctrl+u, delete the whole line. */
            buf[0] = '\0';
            l.pos = l.len = 0;
            refreshLine(&l);
	    break;
        case CTRL_K: /* Ctrl+k, delete from current to end of line. */
            buf[l.pos] = '\0';
            l.len = l.pos;
            refreshLine(&l);
	    break;
        case CTRL_A: /* Ctrl+a, go to the start of the line */
            l.pos = 0;
            refreshLine(&l);
	    break;
        case CTRL_E: /* ctrl+e, go to the end of the line */
            l.pos = l.len;
            refreshLine(&l);
	    break;
	case CTRL_L: /* ctrl+l, clear screen */	
            linenoiseClearScreen();        
            refreshLine(&l);
            break;
	case CTRL_W: /* ctrl+w, delete previous word */
	    linenoiseEditDeletePrevWord(&l);
	    break;
	}
>>>>>>> 75a4793e0c8584e9416085ca6eb247a83ef9e8f4
    }
    return l.len;
}

static int linenoiseRaw(char *buf, size_t buflen, const char *prompt) {
    int count;

    count = linenoiseEdit(buf, buflen, prompt);
<<<<<<< HEAD
    puts("\n\r");
=======
    putstr("\n\r");
>>>>>>> 75a4793e0c8584e9416085ca6eb247a83ef9e8f4
    
    return count;
}

char *linenoise(const char *prompt) {
    char buf[LINENOISE_MAX_LINE];
    int count;

    count = linenoiseRaw(buf,LINENOISE_MAX_LINE,prompt);
    if (count == -1) return NULL;
    return strdup(buf);
}

static void freeHistory(void) {
    if (history) {
        int j;

        for (j = 0; j < history_len; j++)
	    vPortFree(history[j]); 
        vPortFree(history);
    }  
}

int linenoiseHistoryAdd(const char *line) {
    char *linecopy;

    if (history_max_len == 0) return 0;
    if (history == NULL) {
        history = (char **)pvPortMalloc(sizeof(char*)*history_max_len);
        if (history == NULL) return 0; 
        memset(history,0,(sizeof(char*)*history_max_len));
    }
    linecopy = strdup(line);
    if (!linecopy) return 0;
    if (history_len == history_max_len) {
        vPortFree(history[0]);
        memmove(history,history+1,sizeof(char*)*(history_max_len-1));
        history_len--;
    }
    history[history_len] = linecopy;
    history_len++;
    return 1;
}
<<<<<<< HEAD
int linenoiseGetHistory(){

    if ( (history == NULL) || (history_len <=0) ) return 0;

    int i;
    puts("list previous 10 commands");
    for (i = 0 ; i < 10 ; i++){
        printf("%d : %s\r\n",i,history[history_len - i]);
    }

    
    return 1;


}
=======
>>>>>>> 75a4793e0c8584e9416085ca6eb247a83ef9e8f4
