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
    ps,
    help,
    host,
    cat,
    ls,
    mmtest,
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
      [ps]      = {.name="ps"      , .size=2 , .info="Report current processes"},
      [help]    = {.name="help"    , .size=4 , .info="Show command list."},
      [host]    = {.name="host"    , .size=4 , .info="Transmit command to host."},
      [cat]     = {.name="cat"     , .size=3 , .info="Show on the stdout"},
      [ls]      = {.name="ls"      , .size=2 , .info="Show directory under"},
      [mmtest]  = {.name="mmtest"  , .size=6 , .info="Report Memory Management test"},
};	

/*

void mmtest_command(char *str)
{
	char set=1;
	char mm_str[MAX_SERIAL_STR];	
	Print("You will run & free allocated memory... [Y / N]?");
	Print_nextLine();
	Print("(ps) Input [Ctrl+c] to leave cycle when you select [Y].");	
	while(set){
		set=0;
		Read_Input(mm_str,MAX_SERIAL_STR);
		if(!strncmp(mm_str,"Y", 1) || !strncmp(mm_str,"y", 1)){
			mmtest_fio_function(str);
		}
		else if(!strncmp(mm_str,"N", 1) || !strncmp(mm_str,"n", 1)){
			Print("Leave mmtest!");
		}
		else{
			set=1;
			Print("Please input 'Y' & 'N'....");
		}
	}
}

void cat_command(char *str)
{
    char i;
    char c[20];
    char path[20]="/romfs/";
    char buff[100],tmp[7];
    buff[0]='\0';
    int count;
    if(strlen(str)==CMD[cat].size){
		Print("Please input: cat <file> (EX:cat test.txt)");
	}
	else if(str[CMD[cat].size]==' '){
		for(i=CMD[cat].size+1 ; i<strlen(str) ; i++){
				tmp[i- (CMD[cat].size+1) ]=str[i];
			}			
			strcat(path,tmp);
    		        int fd = fs_open(path, 0, O_RDONLY);
			if(fd<0){
      			        Print("No such this file.");
       			 }
       			 else{
          		        count = fio_read(fd, buff, sizeof(buff));
				char buff2[count-1];
				for(i=0;i<count-1;i++)
				buff2[i]=buff[i];
				Print(buff2);
 			} 
	}
	else{
		Print("Error!");
	}

}

void ls_command(char *str)
{
    char ls_buff[128];
    int fileNum;
    ls_buff[0]='\0';
    fileNum=getAllFileName("/romfs/",ls_buff);
    Print(ls_buff);
}

void host_command(char *str)
{
	char host_tmp[strlen(str)];
	char i=0;
	if(strlen(str)==CMD[host].size){
		Print("Please input: host <command>");
	}
	else if(str[CMD[host].size]==' '){
		for(i=CMD[host].size+1;i<strlen(str);i++){
				host_tmp[i- (CMD[host].size+1) ]=str[i];
			}
			host_system(host_tmp, strlen(host_tmp));
			Print("OK! You have transmitted the command to semihost.");
	}
	else{
		Print("Error!");
	}
}*/

void ShellTask_Command(char *str)
{		
	char tmp[20];
	char i;
	char c[40];
	if(!strncmp(str,"hello", 5)) {           
		printf("Hello! how are you?\n\r");
	}
	else if(!strncmp(str,"echo", 4)){
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
/*	else if(!strncmp(str,"ps",2)){
		char title[]="Name\t\t\b\bState\t\b\b\bPriority\t\bStack\t\bNum";
		Puts(title);
		char catch[50];
		vTaskList(catch);
		Print(catch);
	}*/
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
	else if(!strncmp(str,"host",4)){
//		host_command(str);
	}
	else if(!strncmp(str,"cat",3)){
//		cat_command(str);
	}
	else if(!strncmp(str,"ls",2)){
//		ls_command(str);
	}
	else if(!strncmp(str,"mmtest",6)){
//		mmtest_command(str);
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
