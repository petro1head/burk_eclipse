/*------------------------------------------------------*/
/*------------------GPIO library -----------------------*/
/*------------------------------------------------------*/

#ifndef _GPIO_C_
#define _GPIO_C_

#include "gpio.h"

/*--------------------------------------------------------------
--------------------------------------------------------------*/
// Init GPIO: Set GPIO pointer, Set PIO mode, 
// Clear ints, Clear all GPIO->MODE,

void gpio_init()
{
volatile GPIO_UNIT *GPIO = (GPIO_UNIT *)GPIO_BASE;

volatile unsigned int i;

GPIO->INTFLAG= 0xffffffff;	// Clear ints
GPIO->AFIO = 0;				// Set PIO mode
GPIO->DIR = 0;				// all ports as input

for (i=0; i<32; i++)
    GPIO->MODE[i] = 0;    
} // gpio_init

/*--------------------------------------------------------------
--------------------------------------------------------------*/
void gpio_set(int set_mask)
{
	volatile GPIO_UNIT *GPIO = (GPIO_UNIT *)GPIO_BASE;
	GPIO->SET = set_mask;
} // gpio_set

/*--------------------------------------------------------------
--------------------------------------------------------------*/
void gpio_clear(int clr_mask)
{
	volatile GPIO_UNIT *GPIO = (GPIO_UNIT *)GPIO_BASE;
	GPIO->CLR = clr_mask;
} // gpio_clear

/*--------------------------------------------------------------
--------------------------------------------------------------*/
void gpio_invert(int invert_mask)
{
	volatile GPIO_UNIT *GPIO = (GPIO_UNIT *)GPIO_BASE;
	GPIO->INVERT = invert_mask;
} // gpio_invert

/*--------------------------------------------------------------
--------------------------------------------------------------*/
void gpio_write_dir(int out_mask)	// 1 - output, 0 - input
{
	volatile GPIO_UNIT *GPIO = (GPIO_UNIT *)GPIO_BASE;
	GPIO->DIR = out_mask;
} // gpio_write_dir

/*--------------------------------------------------------------
--------------------------------------------------------------*/
int gpio_read_dir()
{
	volatile GPIO_UNIT *GPIO = (GPIO_UNIT *)GPIO_BASE;
	return( GPIO->DIR );
} // gpio_read_dir

/*--------------------------------------------------------------
--------------------------------------------------------------*/
void gpio_write_altfunc(int altfunc_mask)
{
	volatile GPIO_UNIT *GPIO = (GPIO_UNIT *)GPIO_BASE;
	GPIO->AFIO = altfunc_mask;
} // gpio_write_altfunc

/*--------------------------------------------------------------
--------------------------------------------------------------*/
int gpio_read_altfunc(void)
{
	volatile GPIO_UNIT *GPIO = (GPIO_UNIT *)GPIO_BASE;
	return( GPIO->AFIO );
} // gpio_read_altfunc

/*--------------------------------------------------------------
--------------------------------------------------------------*/
void gpio_write_altmode(int altmode_mask)
{
	volatile GPIO_UNIT *GPIO = (GPIO_UNIT *)GPIO_BASE;
	GPIO->AFMODE = altmode_mask;
} // gpio_write_altmode

/*--------------------------------------------------------------
--------------------------------------------------------------*/
int gpio_read_altmode()
{
	volatile GPIO_UNIT *GPIO = (GPIO_UNIT *)GPIO_BASE;
	return( GPIO->AFMODE );
} // gpio_read_altmode

/*--------------------------------------------------------------
--------------------------------------------------------------*/
int gpio_read_input()
{
	volatile GPIO_UNIT *GPIO = (GPIO_UNIT *)GPIO_BASE;
	return( GPIO->IN );
} // gpio_read_input

/*--------------------------------------------------------------
--------------------------------------------------------------*/
void gpio_write_output(int out_val)
{
	volatile GPIO_UNIT *GPIO = (GPIO_UNIT *)GPIO_BASE;
	GPIO->OUT = out_val;
} // gpio_write_output

/*--------------------------------------------------------------
--------------------------------------------------------------*/
int gpio_read_output()
{
	volatile GPIO_UNIT *GPIO = (GPIO_UNIT *)GPIO_BASE;
	return( GPIO->OUT );
} // gpio_read_output

/*--------------------------------------------------------------
--------------------------------------------------------------*/
void gpio_write_mode(int line, int alt_b, int int_b, int out_b, 
						int alt_func, int int_mode)
{
	volatile GPIO_UNIT *GPIO = (GPIO_UNIT *)GPIO_BASE;
	int	val;
	
	val = 
		((alt_b & 1) << 2) |	// Flag "alt_func ON"
		((int_b & 1) << 1) |	// Flag "Interrupt Pin (if alt_b==0)"
		((out_b & 1) << 0) |	// Flag "Direction == Output"
		((alt_func & 1) << 4) |	// alt_func number
		((int_mode & 3) << 8);	// int_mode (0-Lo, 1-Hi, 2-Fall, 3-Rise)
	
	GPIO->MODE[ line & 0x1F ] = val;
} // gpio_write_mode

/*--------------------------------------------------------------
--------------------------------------------------------------*/
int gpio_read_intflag()
{
	volatile GPIO_UNIT *GPIO = (GPIO_UNIT *)GPIO_BASE;
	return( GPIO->INTFLAG );
} // gpio_read_intflag

/*--------------------------------------------------------------
--------------------------------------------------------------*/
void gpio_write_intflag(int int_flag)
{
	volatile GPIO_UNIT *GPIO = (GPIO_UNIT *)GPIO_BASE;
	GPIO->INTFLAG = int_flag;
} // gpio_write_intflag

/*--------------------------------------------------------------
--------------------------------------------------------------*/
int gpio_read_id()
{
	volatile GPIO_UNIT *GPIO = (GPIO_UNIT *)GPIO_BASE;
	return( GPIO->IDREG );
} // gpio_read_id

#endif /* _GPIO_C_ */
