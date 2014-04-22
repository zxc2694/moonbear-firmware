#include "QuadCopterConfig.h"

#include "string.h"
#include "stdlib.h"

#define MAX_FUNC_LEN 32

#define FUNC_REGISTER(name) #name

int func_example();

enum TEST_FUNCTION {
	func_example_ID,
	FUNCTION_CNT
};

char function_list[FUNCTION_CNT][MAX_FUNC_LEN] = {
	FUNC_REGISTER(func_example)
};

void Func_Call_Handler(char *func_name)
{
	int i;

	for (i = 0; i < FUNCTION_CNT; i++) {
		if (strcmp(func_name, "func_example") == 0) {
			int retval = func_example();
			serial.printf("Function name:%s\n\r", func_name);
			serial.printf("Argument:None\n\r");
			serial.printf("Return value:%d <type-int>\n\r", retval);

			return;
		}
	}

	serial.printf("\n\r[test]Unknown function\n\r");
}

void shell_test(char parameter[][MAX_CMD_LEN], int par_cnt)
{
	switch (par_cnt) {
	case 0:
		serial.printf("type command \"test [function name]\" to call the function.\n\r");
		serial.printf("Software testing function:\n\r");

		int i;

		for (i = 0; i < FUNCTION_CNT; i++) {
			serial.printf("[%d]%s\n\r", i + 1, function_list[i]);
		}

		break;

	case 1:
		Func_Call_Handler(parameter[0]);

		break;

	default:
		break;
	}
}


int func_example()
{
	serial.printf("This words is calling by the func_example!\n\r");

	return 100;
}
