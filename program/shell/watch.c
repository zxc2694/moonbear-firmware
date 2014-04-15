#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "QuadCopterConfig.h"

extern xTaskHandle watch_task_handle;

char (*watch_arguments)[MAX_CMD_LEN];
int watch_arg_cnt;

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
	while(1) {
		if(strcmp(watch_arguments[1], "-gui")) {
			watch_gui();
		}
	}
}
