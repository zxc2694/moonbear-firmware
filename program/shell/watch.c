#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "QuadCopterConfig.h"

void shell_watch(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	serial.printf("Test");
}

void watch_task()
{
}
