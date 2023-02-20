#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uart.h"
#include "chip_vm014.h"
#include "devdescr.h"
#include "chip.h"
#include "io.h"
#include "burk_function.h"

int _getpid_r(struct _reent *a) {
	return 0;
}

int _kill_r(struct _reent *a, int b, int c) {
	return 0;
}

_PTR _calloc_r(struct _reent *a, size_t b, size_t c) {
	return 0;
}

typedef struct Buffer {
	char str[100];
	int len;
} Buffer;

//int parse(unsigned long *t, double *speed, char data[100]) {
//	char *sep = NULL;
//	char t_str[100];
//	char speed_str[100];
//
//	sep = strchr(data, ';');
//
//	if (sep != NULL) {
//		strncpy(t_str, data, (sep - data) / sizeof(char));
//		*t = atoi(t_str);
//		strcpy(speed_str, (sep + 1));
//		*speed = atof(speed_str);
//		return 1;
//	}
//
//	return 0;
//}

void outPrintln(MySystem *ps) {
	// Serial.println(SystemOut());
	//uart_send_number(0, SystemOut(ps));
	char string [10];
	sprintf (string, "%lf.3\n",SystemOut(ps));
	uart_send_string(0, (unsigned char*) string);
}

int main(void) {

	volatile UART_UNIT *uart0 = (UART_UNIT*) 0xBFF00100;
	uart0->IIRF = 0xff; //fifo
	uart0->IIRF = 0xff;

	uart_baud_set(0, 26); // baud 57600 bit/s
	uart0->LCR = 3; //  8 bit data, no parity, 1 stop bit // uart1->LCR = (1<<0)|(1<<1)

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

	uart_send_string(0, (unsigned char*) "OK");

	while (1) {
		if (uart_check_in(0)) {
			c = uart_read_byte(0);
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

		if (readed_data)
		{
			// down flag for new read data
			readed_data = 0;
			// parse data
	//			if (parse(&t, &speed, qt_data))
	//			{
	//				// system process run
	//				SystemRun(&s, t, speed);
	//				outPrintln(&s);
	//
	//			}
			if (sscanf(qt_data, "%lu;%lf", &t, &speed))
			{
				// system process run
				SystemRun(&s, t, speed);
				outPrintln(&s);
			}

			else
			{
				//char bad_string[100] = "bad";
				char bad_string[100];

				// bad string
				//sprintf (bad_string, "%s\n", qt_data);
     			//strcat(bad_string, qt_data);
				//strcat(bad_string, "\n");

				sprintf (bad_string, "bad%s\n", qt_data);
				uart_send_string(0, (unsigned char*)bad_string);
			}
		}

	}
	return 0;
}

