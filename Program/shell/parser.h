#ifndef __PARSER_H
#define __PARSER_H

#define MAX_CMD_LEN 64 //256 is too big!
#define MAX_PAR_CNT 10

#define CMD_DEF(name, group) [name ## _ID] = {.str = #name, .func = group ## _ ## name}

typedef struct {
	char str[MAX_CMD_LEN];
	void (*func)(char parameter[][MAX_CMD_LEN], int par_cnt);
} command_list;

typedef struct {
	char instr[MAX_CMD_LEN];             /* The instruction part of the string */
	char par[MAX_PAR_CNT][MAX_CMD_LEN];  /* The parameter part of the string */
	int par_cnt;                         /* The count of the parameter */
	command_list *cmd;                     /* The structure which contain the instructions and its handler */
} command_data;

void commandExec(char *cmd_str, command_data *cd, command_list *list, int list_cnt);

#endif
