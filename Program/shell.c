#include "shell.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "module_rs232.h"

extern serial_ops serial;

#define MAX_SERIAL_STR 100
#define Command_Number 8

enum ALLcommand 
{
/*Add your command in here. */
    hello,
    echo,
    help,
};

struct STcommand
{
	char *name;
	int  size; 	
	char *info;
};
typedef struct STcommand CmdList;
CmdList CMD[]={
	   /*Add your command list in here. */

      [hello]   = {.name="hello"   , .size=5 , .info="Show 'Hello! how are you?'"},
      [echo]    = {.name="echo"    , .size=4 , .info="Show your input"},
      [help]    = {.name="help"    , .size=4 , .info="Show command list."},
      
};	

void ShellTask_Command(char *str)
{		
	char tmp[20];
	char i;
	char c[40];
	if(!strncmp(str,"hello", CMD[hello].size)) {           
		printf("Hello! how are you?\n\r");
	}
	else if(!strncmp(str,"echo",CMD[echo].size)){
		if(strlen(str)==4){
			printf("\n\r");
		}
		if(str[4]==' '){
			for(i=5;i<strlen(str);i++){
				tmp[i-5]=str[i];
			}
			printf("%s\n\r",tmp);
		}
	}

	else if(!strncmp(str,"help", 4)) {           
		sprintf(c,"You can use %d command in the freeRTOS",Command_Number);
		printf("%s\n\r",c);
		printf("\n\r");
		for(i=0;i<Command_Number;i++){
			puts(CMD[i].name);
			puts("\t-- ");
			puts(CMD[i].info);
			printf("\n\r");
		}
	}

	else{
		printf("Command not found, please input 'help'\n\r");
	}
}

void Read_Input(char *str,int STR)
{
	char ch;
	int curr_char, done;
	curr_char = 0;
	done = 0;
	str[curr_char] = '\0';
	do{
		/* Receive a byte from the RS232 port (this call will block). */
         	ch=serial.getch();

		if (curr_char >= STR-1 || (ch == '\r') || (ch == '\n')){
			str[curr_char] = '\0';
			done = -1;
			/* Otherwise, add the character to the response string. */
		}
		else if(ch == 127){ //press the backspace key
                	if(curr_char!=0){
                    		curr_char--;
                    		printf("\b \b");
              		}
            	}
            	else if(ch == 27){  //press up, down, left, right, home, page up, delete, end, page down
                	ch=serial.getch();
                	if(ch != '['){
                    		str[curr_char++] = ch;
                    		printf("%c",ch);
                	}
                	else{
                    		ch=serial.getch();
                   		 if(ch >= '1' && ch <= '6'){
                        		ch=serial.getch();
                    		}
                	}
            	}
		else{
			str[curr_char++] = ch;
			printf("%c",ch);
		}
	} while (!done);

        printf("\n\r");
	return str;
}

void Shell()
{
	printf("\n\n\r");
	char str[MAX_SERIAL_STR];
	char pos[] = "Quadrotor's RTOS~$ ";
	while (1){
        puts(pos);
		Read_Input(str,MAX_SERIAL_STR);
		
		/*This is my shell command*/
		ShellTask_Command(str);
    }
}
