#include "lib.h"
#include "string-util.h"

/*print messages*/
void print_msg(char *msg)
{
	if (!msg) return;
	fio_write(STDOUT, msg, strlen(msg));
}

/*change next line*/
void print_next_line()
{
	print_msg("\n\r");
}
