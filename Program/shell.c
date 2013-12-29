/* Standard C lib includes */
#include <stddef.h>
#include <string.h>

/* Linenoise and shell includes */
#include "shell.h"
#include "linenoise.h"

#include "module_rs232.h"

#include "FreeRTOS.h"

#define CMD_DEF(name) [name ## _ID] = {.str = #name, .func = name}

enum CMD_ID {
        unknown_command_ID,
        monitor_ID,
        CMD_CNT
};

typedef struct {
	char str[MAX_CMD_LEN];
	void (*func)(char parameter[][MAX_CMD_LEN], int par_cnt);
} instr_data;

struct command_data {
        char instr[MAX_CMD_LEN];             /* The instruction part of the string */
        char par[MAX_PAR_CNT][MAX_CMD_LEN];  /* The parameter part of the string */
        int par_cnt;                         /* The count of the parameter */
	instr_data *cmd;		     /* The structure which contain the instruction and its handler */
};


/* Command correspondence functions */
void unknown_command(char parameter[][MAX_CMD_LEN], int par_cnt);
void monitor(char parameter[][MAX_CMD_LEN], int par_cnt);

//First string don't need to store anything for unknown commands
instr_data id[CMD_CNT] = {
	CMD_DEF(unknown_command),
	CMD_DEF(monitor),
};


int completion_disable = 0;
int history_disable = 0;

/**** Commands  identification ********************************************************/
static void commandConfig(char *cmd_str, struct command_data *cd)
{
	int i = 0, str_cnt = 0;
	while(cmd_str[i] != '\0') {
		if(cmd_str[i] == ' ') {
			//Put the '\0' at the end of the string
			if(cd->par_cnt == 0)
				cd->instr[str_cnt] = '\0';
			else
				cd->par[cd->par_cnt - 1][str_cnt] = '\0';

			cd->par_cnt++;

			i++;
			str_cnt = 0; //Reset for next string
			continue;
		}

		/* Handling the instruction while par_cnt is 0 */
		if(cd->par_cnt == 0) 
			cd->instr[str_cnt] = cmd_str[str_cnt];
		/* Handling The parameter while par_cnt is bigger than 0 */
		else
			cd->par[cd->par_cnt -1][str_cnt] = cmd_str[i];

		i++;
		str_cnt++;	
	}
}

void commandExec(char *cmd_str, struct command_data *cd)
{
	commandConfig(cmd_str, cd);

	int i;
	for(i = 0; i < CMD_CNT; i++) {
		if(strcmp(cd->instr, id[i].str) == 0) {
			id[i].func(cd->par, cd->par_cnt);
			return;
		}
	}
	
	id[0].func(cd->par, cd->par_cnt); //Unknown command's function
}
 
/**** Shell task **********************************************************************/
void linenoise_completion(const char *buf, linenoiseCompletions *lc) {
	if(completion_disable)
		return;

	int i; //i = 1 to ignore the "UNKNOWN_COMMAND" string
	for(i = 1;i < CMD_CNT; i++) {
		if(buf[0] == id[i].str[0])
			linenoiseAddCompletion(lc, id[i].str);
	}
}

void shell_task()
{
        char *shell_str;

        struct command_data cd = {
                .cmd = id,
                .par_cnt = 0
        };

        linenoiseSetCompletionCallback(linenoise_completion);

        /* Clear the screen */
        printf("\x1b[H\x1b[2J");
	/* Show the prompt messages */
        printf("[System status]Initialized successfully!\n\r");
        printf("Please type \"help\" to get more informations\n\r");

        while(1) {
                shell_str = linenoise("User > ");
                commandExec(shell_str, &cd);
        }
}
/**** Customize command function ******************************************************/
void unknown_command(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	printf("Command not found\n\r");
}

void monitor(char parameter[][MAX_CMD_LEN], int par_cnt)
{
        printf("\x1b[H\x1b[2J");

	
}


