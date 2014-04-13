#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "QuadCopterConfig.h"

void shell_watch(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	/* Enable the watch task */

	/* Waiting for interruption signal */
	while(1) {
		char *signal = linenoise("");

		//Get the interrupt signal
		if(signal == NULL) {
			//Disable the task
			

			//Exit
			break;
		}
	}
}

void watch_task()
{
}
