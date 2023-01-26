
#include <string.h>
#include <stdlib.h>
#include "uart.h"
#include "chip_vm014.h"
#include "devdescr.h"
#include "chip.h"
#include "io.h"
#include "burk_function.h"


int _getpid_r(struct _reent * a) {
	return 0;
}

int _kill_r(struct _reent * a, int b, int c) {
	return 0;
}

_PTR _calloc_r(struct _reent * a, size_t b, size_t c)
{
	return 0;
}


typedef struct Buffer {
	char str[100];
	int len;
} Buffer;


int parse(unsigned long * t, double *speed, char data[100])
{
	char * sep = NULL;
	char t_str[100];
	char speed_str[100];

	sep = strchr(data, ';');

	if (sep != NULL)
	{
		strncpy(t_str, data, (sep - data) / sizeof(char));
		*t = atoi(t_str);
		strcpy(speed_str, (sep + 1));
		*speed = atof(speed_str);
		return 1;
	}

	return 0;
}


void outPrintln(MySystem *ps) {
 // Serial.println(SystemOut());
	uart_send_number(1, SystemOut(ps));
}

int main(void) {

	volatile UART_UNIT * uart1 = (UART_UNIT*) 0xBFF00800;
	uart1 -> IIRF = 0xff; //fifo
	uart1 -> IIRF = 0xff;

	uart_baud_set(1, 26); // baud 57600 bit/s
	uart1 -> LCR = 3; //  8 bit data, no parity, 1 stop bit // uart1->LCR = (1<<0)|(1<<1)

	unsigned long t = 0;
	double speed = 0;

	Buffer buf;
	char qt_data[100];
	// flag that we readed data
	int readed_data = 0;
	// char for read data
	char c = 0;

	MySystem s;
	set_default(&s);

	uart_send_string(1, (unsigned char *)"OK");

	while (1) {
		if (uart_check_in(1)) {
			c = uart_read_byte(1);
			if (c == '\n') {
				// Cut the data from QT
				buf.str[buf.len] = '\0';
				// Copy data from buffer
				strcpy(qt_data, buf.str);
				// Clear buffer
				buf.str[0] = '\0';
				buf.len = 0;
				// up flag that we readed data
				readed_data = 1;

			} else {
				// data don't ended
				// add symbol to the string
				buf.str[buf.len] = c;
				buf.len++;
			}
		}

		if (readed_data) {
			// down flag for new read data
			readed_data = 0;
			// parse data
			if (parse(&t, &speed, qt_data))
			{
				// system process run
				SystemRun(&s, t, speed);
				outPrintln(&s);

			}else {

				//send old data
			     }
		}

	}
	return 0;
}

