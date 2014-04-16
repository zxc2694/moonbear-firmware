#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "QuadCopterConfig.h"

extern xTaskHandle watch_task_handle;

char (*watch_arguments)[MAX_CMD_LEN];
int watch_arg_cnt;

global_t *find_variable(char *name) {
	int i;
	for(i = 0; i < system.var_count; i++) {
		if(strcmp(name, system.variable[i].name) == 0)
				return system.variable + i;
	}

	/* Can't find the variable */
	return 0;
}

void watch_gui()
{
	while(1) {
		serial.printf("%f %f %f %f %f %f\n\r",
			AngE.Pitch, AngE.Roll,
			system.variable[MOTOR1].value, system.variable[MOTOR2].value,
			system.variable[MOTOR3].value, system.variable[MOTOR4].value
		);

		vTaskDelay(20);
	}
}

void shell_watch(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	watch_arguments = parameter;
	watch_arg_cnt = par_cnt;	

	/* Check for the passing arguements is exist */
	int i;
	for(i = 0; i < par_cnt; i++) {
		if(i == 0 && strcmp(watch_arguments[i], "-gui") == 0)
			continue;

		if(i > 0 && strcmp(watch_arguments[i], "-gui") == 0) {
			serial.printf("Error: -gui mode can only working independently!\n\r");
			return;
		} else if(find_variable(watch_arguments[i]) == 0) {
			serial.printf("Error: Unknown variable!\n\r");
			return;	
		}
	}

	/* Prompt */
	serial.printf("Refresh time: 1.0s\n\r");

	/* Enable the watch task */
	vTaskResume(watch_task_handle);

	/* Waiting for interruption signal */
	while(1) {
		char *signal = linenoise("");

		//Get the interrupt signal
		if(signal == NULL) {
			//Disable the task
			vTaskSuspend(watch_task_handle);	

			//Exit
			break;
		}
	}
}

void watch_task()
{
	global_t *find_var;

	while(1) {
		if(strcmp(watch_arguments[0], "-gui") == 0) {
			watch_gui();
			continue;
		}

		int i;
		for(i = 0; i < watch_arg_cnt; i++) {
			find_var = find_variable(watch_arguments[i]);
			serial.printf("%s : %f\n\r", watch_arguments[i], find_var->value);
		}

		serial.printf("<Delay 1 sec ...>\n\r");

		vTaskDelay(1000);
	}
}
