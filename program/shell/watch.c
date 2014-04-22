#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "QuadCopterConfig.h"

enum {
	TASK_RUNNING,
	TASK_STOP,
	TASK_INTERRUPT
};

/* Watch task related variable */
status_t task_status;
extern xTaskHandle watch_task_handle;

/* The arguments of the shell */
char(*watch_arguments)[MAX_CMD_LEN];
int watch_arg_cnt;

/**
  * @brief  Search the variable by name in the system variables
  * @param  name (pointer of char)
  * @retval pointer of finded variable
  */
global_t *find_variable(char *name)
{
	int i;

	for (i = 0; i < system.var_count; i++) {
		if (strcmp(name, system.variable[i].name) == 0)
			return system.variable + i;
	}

	/* Can't find the variable */
	return 0;
}

/**
  * @brief  GUI Plotting mode of watch command
  * @param  None
  * @retval None
  */
void watch_gui()
{
	serial.printf("\n\r%f %f %f %f %f %f",
		      AngE.Pitch, AngE.Roll,
		      system.variable[MOTOR1].value, system.variable[MOTOR2].value,
		      system.variable[MOTOR3].value, system.variable[MOTOR4].value
		     );
}

/**
  * @brief  Data printing mode of watch command
  * @param  None
  * @retval None
  */
void watch_data()
{
	global_t *find_var;

	/* Clear the screen */
	serial.puts("\x1b[H\x1b[2J");

	serial.printf("Refresh time: 1.0s\n\r");

	int i;

	for (i = 0; i < watch_arg_cnt; i++) {
		find_var = find_variable(watch_arguments[i]);
		serial.printf("\n\r%s : %f", watch_arguments[i], find_var->value);
	}
}

/**
  * @brief  watch command handler
  * @param  None
  * @retval None
  */
void shell_watch(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	watch_arguments = parameter;
	watch_arg_cnt = par_cnt;

	/* Check for the passing arguements is exist */
	int i;

	for (i = 0; i < par_cnt; i++) {
		if (i == 0 && strcmp(watch_arguments[i], "-gui") == 0)
			continue;

		if (i > 0 && strcmp(watch_arguments[i], "-gui") == 0) {
			serial.printf("Error: -gui mode can only working independently!\n\r");
			return;

		} else if (find_variable(watch_arguments[i]) == 0) {
			serial.printf("Error: Unknown variable!\n\r");
			return;
		}
	}

	/* Enable the watch task */
	task_status = TASK_RUNNING;
	vTaskResume(watch_task_handle);

	/* Waiting for interruption signal */
	while (1) {
		char *signal = linenoise("");

		//Get the interrupt signal
		if (signal == NULL) {
			//Disable the task
			task_status = TASK_INTERRUPT;

			/* Synchronization, Waiting for the task stop signal */
			while (task_status != TASK_STOP);

			/* Clear the screen */
			serial.puts("\x1b[H\x1b[2J");

			break; //Exit
		}
	}
}

/**
  * @brief  watch command (Shell) handling task
  * @param  None
  * @retval None
  */
void watch_task()
{
	int delay_time;

	while (1) {
		/* GUI Plotting mode */
		if (strcmp(watch_arguments[0], "-gui") == 0) {
			watch_gui();
			delay_time = 50;

		} else {
			/* Data printing mode */
			watch_data();
			delay_time = 1000;
		}

		serial.printf("\n\r");

		/* Capture the interrupt signal form the Shell */
		if (task_status == TASK_INTERRUPT) {
			task_status = TASK_STOP; //Stop signal
			vTaskSuspend(NULL);
		}

		/* Delay, depending on the mode and the setting */
		vTaskDelay(delay_time);
	}
}
