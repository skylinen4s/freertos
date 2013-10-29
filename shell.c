#include "shell.h"
#include "lib.h"
#include "string-util.h"
#include "fio.h"

void check_input(char *str)
{
	if (!strncmp(str, "help", 4)){
		print_msg("help");
	}
	else if (!strncmp(str, "hello", 5)){
		print_msg("hello");
	}
	else if (!strncmp(str, "ps", 2)){
		print_msg("ps");
	}
	else if ((!strncmp(str, "echo ", 5) && (str[5] != ' '))){
		print_msg(&str[5]);
		print_next_line();
	}
	else{
		if (str[0] == '\0');
		else{
			print_msg("'");
			print_msg(&str[0]);
			print_msg("': command not found\n\r");
		}
	}
}

void readwrite_task(void *pvParameters)
{
        char str[100];
        char ch_buf[2] = {'0','\0'};
        char ch;
        int count_char;
        int done;

        while(1) {
                count_char = 0;
                done = 0;
                do{
                        ch = receive_byte();
                        if ((ch == '\n') || (ch == '\r')){
                                str[count_char++] = '\0';
                                print_next_line();
                                done = -1;
                        }
                        else if ((ch == BACKSPACE || ch == '\b') && (count_char != 0)){
                                str[count_char--] = '\0';
								/*1.back to last word
								 *2.replace it with space
								 *3.cursor back to the last word*/        
                                print_msg("\b \b");
                        }
                        else if(ch != BACKSPACE){
                                str[count_char++] = ch;
                                ch_buf[0] = ch;
                                print_msg(ch_buf);
                        }
                } while (!done);
                        check_input(str);
        }
}	
