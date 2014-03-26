#include "string.h"
#include "parser.h"

static void cmdSpiltToken(char *cmd_str, command_data *cd)
{
	int i = 0, str_cnt = 0;

	while (cmd_str[i] != '\0') {
		if (cmd_str[i] == ' ') {
			//Put the '\0' at the end of the string
			if (cd->par_cnt == 0)
				cd->instr[str_cnt] = '\0';
			else
				cd->par[cd->par_cnt - 1][str_cnt] = '\0';

			cd->par_cnt++;

			i++;
			str_cnt = 0; //Reset for next string
			continue;
		}

		/* Handling the instruction while par_cnt is 0 */
		if (cd->par_cnt == 0)
			cd->instr[str_cnt] = cmd_str[str_cnt];

		/* Handling The parameter while par_cnt is bigger than 0 */
		else
			cd->par[cd->par_cnt - 1][str_cnt] = cmd_str[i];

		i++;
		str_cnt++;
	}
}

void commandExec(char *cmd_str, command_data *cd, command_list *list, int list_cnt)
{
	cmdSpiltToken(cmd_str, cd);

	int i;

	for (i = 0; i < list_cnt; i++) {
		if (strcmp(cd->instr, list[i].str) == 0) {
			list[i].func(cd->par, cd->par_cnt);
			return;
		}
	}

	list[0].func(cd->par, cd->par_cnt); //Unknown command's function
}

