
#include <string.h>
#include <stdlib.h>
#include "uart.h"
#include "chip_vm014.h"
#include "devdescr.h"
#include "chip.h"
#include "io.h"

int _getpid_r (struct _reent * a)
{
	return 0;
}

int _kill_r (struct _reent * a, int b, int c)
{
	return 0;
}

_PTR _calloc_r(struct _reent * a, size_t b, size_t c)
{
	return 0;
}


int main (void)
{

	volatile UART_UNIT * uart1 = (UART_UNIT*) 0xBFF00800;

	uart1->IIRF = 0xff;
	uart1->IIRF = 0xff;

//char symbol = 0xAA;
//uart1->RXTX = symbol;

	uart1->RXTX = 0xAA; // transmit

	return 0;
}


