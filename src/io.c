#if (TEST_LIB==0)
/*------------------------------------------------------*/
/*------------------IO function -----------------------*/
/*------------------------------------------------------*/

#include "uart.h"
#include "io.h"

/*--------------------------------------------------------------
   tos (uart_send_string)
--------------------------------------------------------------*/
void tos(unsigned char *st)
{
//	int i;
//	i=100000;
//	while(i)i--;
	uart_send_string(0, st);
}
/*--------------------------------------------------------------
   ton (uart_send_number)
--------------------------------------------------------------*/
void ton(unsigned int number)
{
	uart_send_number(0, number);
}
/*--------------------------------------------------------------
   ton10 (uart_send_number10)
--------------------------------------------------------------*/
void ton10(int val)
{
	uart_send_number10(0, val);
}

void tob(char c)
{
	uart_send_byte(0, c);
}

#endif /* TEST_LIB==0 */
