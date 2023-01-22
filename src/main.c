#include <string.h>
#include <stdlib.h>
#include "uart.h"
#include "chip_vm014.h"
#include "devdescr.h"
#include "chip.h"
#include "io.h"

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

typedef struct Bufffer {
	char str[100];
	int len;
} Bufffer;

int parse(unsigned long * t, double *speed, char data[100]) {
	char * sep = NULL;
	char t_str[100];
	char speed_str[100];

	sep = strchr(data, ';');

	if (sep != NULL) {
		// получаем строку времени - всё что до ;
		strncpy(t_str, data, (sep - data) / sizeof(char));
		// преобразуем в целое числа
		*t = atoi(t_str);

		// получаем скорость в виде строки
		// используем указатель на символ после пробела
		strcpy(speed_str, (sep + 1));
		// преобразуем в вещественное число
		*speed = atof(speed_str);

		return 1;
	}

	return 0;
}

int main(void) {

	volatile UART_UNIT * uart1 = (UART_UNIT*) 0xBFF00800;

	uart1->IIRF = 0xff;
	uart1->IIRF = 0xff;

//char symbol = 0xAA;
//uart1->RXTX = symbol;

	uart1->RXTX = 0xAA; // transmit

	unsigned long t = 0;
	double speed = 0;

	Bufffer buf;
	char qt_data[100];
	// flag that we readed data
	int readed_data = 0;
	// char for read data
	char c;

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
			if (parse(&t, &speed, qt_data)) {
				// system process run
			} else {
				//send old data
			}
		}

	}
	return 0;
}

