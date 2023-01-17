/*------------------------------------------------------*/
/*------------------Timer library -----------------------*/
/*------------------------------------------------------*/

#ifndef _TIMER_DEC_C_
#define _TIMER_DEC_C_

/* Timers use 32-bit values */
#include "timer_dec.h"

static TIMER_UNIT * const T[] = {
 (TIMER_UNIT *)TIMER0_BASE,
 (TIMER_UNIT *)TIMER1_BASE,
 (TIMER_UNIT *)TIMER2_BASE
 };
 
/*--------------------------------------------------------------
--------------------------------------------------------------*/
static void del()
{
	volatile int i;
	for (i=0; i<20; i++);
}

/*--------------------------------------------------------------
   Load timer by given value and start in single mode
--------------------------------------------------------------*/
void timer_mode(int timer_n, 
				int mode, 
				int int_en, 
				unsigned int freq, 
				unsigned long long value)
{
	T[timer_n]->STATUS = 0;	// stop
	T[timer_n]->LOAD_LO = (unsigned int) (value & 0x00000000ffffffffLL);
	T[timer_n]->LOAD_HI = (unsigned int) (value >> 32);
	del();
	T[timer_n]->STATUS = mode | int_en | freq;
} // timer_mode

/*--------------------------------------------------------------
   Start timer
--------------------------------------------------------------*/
void timer_start(int timer_n)
{
	T[timer_n]->STATUS |= TIMER_RUN ;
	del();
}

/*--------------------------------------------------------------
   Stop timer
--------------------------------------------------------------*/
void timer_stop(int timer_n)
{
	T[timer_n]->STATUS = 0;
	del();
}

/*--------------------------------------------------------------
   Read current timer counter value
--------------------------------------------------------------*/
unsigned int timer_read( int timer_n )
{
	return (T[timer_n]->READ_LO);
}

/*--------------------------------------------------------------
  Returns timer fixed frequency (Hz)
--------------------------------------------------------------*/
unsigned int timer_fix_frequency(int timer_n)
{
	return(TIMER_FIX_FREQUENCY);
}

/*--------------------------------------------------------------
   Reload timer
--------------------------------------------------------------*/
void timer_reload(int timer_n)
{
	T[timer_n]->STATUS |= TIMER_RELOAD;
}

#endif /* _TIMER_DEC_C_ */
