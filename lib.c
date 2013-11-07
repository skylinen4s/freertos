#include "lib.h"
#include "string-util.h"
#include <stdarg.h> /*for va_list*/

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

/**/
static void print_func(const char *format, va_list args)
{
	int i = 0;
	int int_;
	char ch[2] = {0};
	char *str;
	for(i = 0; format[i] != 0; i++){
		if(format[i] == '%'){
			switch(format[i+1]){
				case 'c':
					{
						ch[0] = (char) va_arg(args, int);
						str = ch;
					}break;
				case 's':
					{
						str = va_arg(args, char *);
					}break;
				default:
					{
						ch[0] = format[i];
						str = ch;
					}
			}
			print_msg(str);
			i++;
		}
		else{
			ch[0] = format[i];
			str = ch;
		}
	}
}

/**/
void printf(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	print_func(format, args);
	va_end(args);
}

