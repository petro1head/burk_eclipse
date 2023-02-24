/*------------------------------------------------------*/
/*------------------UART library------------------------*/
/*------------------------------------------------------*/

#ifndef _UART_H_
#define _UART_H_

#include "chip.h"
#include "devdescr.h"

/*----------------- Prototypes -------------------------*/
char uart_read_byte(int uart);
void uart_send_byte( int uart, char ch);
void uart_send_string(int uart, unsigned char *st);
void uart_send_number(int uart, unsigned int number);
void uart_send_number10(int uart_n, int val);

void uart_init(int uart, 
		  int fifo_on_off, 
		  int data_format,
  		  int stop_bit,  
		  int parity);

void uart_baud_set(int uart, int divisor);
unsigned int uart_check_out( int uart );
unsigned int uart_check_in( int uart );

#endif /* _UART_H_ */
