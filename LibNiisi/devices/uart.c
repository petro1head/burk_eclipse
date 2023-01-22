/*------------------------------------------------------*/
/*------------------UART library -----------------------*/
/*------------------------------------------------------*/

#ifndef _UART_C_
#define _UART_C_

#include "uart.h"

static UART_UNIT * const U[] = {
 (UART_UNIT *)UART0_BASE,
 (UART_UNIT *)UART1_BASE,
 };

/*--------------------------------------------------------------
	uart_read_byte
--------------------------------------------------------------*/
char uart_read_byte(int uart_n)
{
	while ( ! uart_check_in(uart_n) ) ;
		
	return U[uart_n]->RXTX;
	
} // uart_read_byte

/*--------------------------------------------------------------
   uart_send_byte
--------------------------------------------------------------*/
void uart_send_byte( int uart_n, char byte)
{
	while ( ! uart_check_out(uart_n) ) ;
		U[uart_n]->RXTX = byte;

} // uart_send_byte

/*--------------------------------------------------------------
   uart_send_string (tos)
--------------------------------------------------------------*/
void uart_send_string(int uart_n, unsigned char *st)
{
	while( *st )
	{
		uart_send_byte( uart_n, *st); st++;
	}
} // uart_send_string

/*--------------------------------------------------------------
   uart_send_number (ton)
--------------------------------------------------------------*/
void uart_send_number(int uart_n, unsigned int number)
{
	int i;
	unsigned char a0;
	char a1;
	
	for(i=7; i>=0; i--)
	{		
		a0 = ( number >> i*4 ) & 0xf ;
		a1 = a0 - 10;
		if( a1 < 0) a0 = a0 + '0'; 
		else a0 = a0 + 0x37; 		// 'A' - 10;
		uart_send_byte( uart_n, a0 );
		
		a0 = a0 >> 4;	
	}	  
} // uart_send_number

/*--------------------------------------------------------------*/
/*   Decimal output, only significant digits                    */
/*--------------------------------------------------------------*/
void uart_send_number10(int uart_n, int val)
{
	int	i, n;
	unsigned int q, r;
	char	buf[32];
	
	n = 0;
	q = (unsigned )val;

	do {
	   r = q % 10;
	   q /= 10;
	   buf[n] = r + '0';
	   n++;
	}
	while ( q > 0 ) ;
	
	for ( i=n-1; i>=0; i-- ) uart_send_byte( uart_n, buf[i] );
} // uart_send_number10

/*--------------------------------------------------------------
	uart_baud_set
--------------------------------------------------------------*/
void uart_baud_set(int uart_n, int divisor)
{
	
	U[uart_n]->LCR |= DLAB;			 
	U[uart_n]->RXTX = divisor;			// DLL 
	U[uart_n]->IER = ( divisor >> 8 );	// DLM
	
	U[uart_n]->LCR &= ~DLAB;

//Set message format (data_format | parity | stop_bit)
	U[uart_n]->LCR = (LENGTH_7 | PARITY_OFF | STOP_BIT_2) ;
	
} // uart_baud_set	

/*--------------------------------------------------------------
	uart_check_out (Tx ready)
--------------------------------------------------------------*/
unsigned int uart_check_out(int uart_n)
{
	return ( (U[uart_n]->LSR >> 6) & 1 ); // Tx ready
} // uart_check_out

/*--------------------------------------------------------------
	uart_check_in (Rx ready)
--------------------------------------------------------------*/
unsigned int uart_check_in(int uart_n)
{	
	return ( U[uart_n]->LSR & 1 );	//Rx ready
} // uart_check_in

#endif /* _UART_C_ */
