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
		if (!strncmp(str,"\0",1)){
		}
		else{
			print_msg("'");
			print_msg(&str[0]);
			print_msg("': command not found\n\r");
		}
	}
}	
