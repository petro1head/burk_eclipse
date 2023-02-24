/*------------------------------------------------------*/
/*------------------Timer library -----------------------*/
/*------------------------------------------------------*/

/* Timers use 32-bit values */
#include "timer.h"

static TIMER_UNIT * const T[] = {
 (TIMER_UNIT *)TIMER0_BASE,
 (TIMER_UNIT *)TIMER1_BASE,
 (TIMER_UNIT *)TIMER2_BASE,
 };

/*--------------------------------------------------------------
   Set timer mode
--------------------------------------------------------------*/
void timer_mode(int timer_n, int mode, int int_en, 
					unsigned int freq)
{
	T[timer_n]->CTRL = TIMER_STOP;	// stop
	T[timer_n]->CNT  = 0; 			// start counter value
	T[timer_n]->CTRL = mode | int_en | freq;
	T[timer_n]->CTRL &= ~(1<<6);	// disable frequency devider

} // timer_mode


/*--------------------------------------------------------------
   Set mode for compare(CMP) registers
--------------------------------------------------------------*/
void timer_cmp_mode(int timer_n, unsigned int ch_n, unsigned int mode, 
					unsigned int value)
{
	T[timer_n]->CMP[ch_n] = value;
	T[timer_n]->CCMP = (mode << (4 * ch_n) );
} // timer_cmp_mode	

/*--------------------------------------------------------------
   Set mode for capture(CAP) registers
--------------------------------------------------------------*/
void timer_cap_mode(int timer_n, unsigned int ch_n, unsigned int mode)
{
	T[timer_n]->CCAP = (mode <<  4*ch_n );
} // timer_cap_mode

/*--------------------------------------------------------------
   Start timer
--------------------------------------------------------------*/
void timer_start(int timer_n)
{
	T[timer_n]->CTRL = TIMER_RUN ;
} // timer_start

/*--------------------------------------------------------------
   Stop timer
--------------------------------------------------------------*/
void timer_stop(int timer_n)
{
	T[timer_n]->CTRL &= ~1; // stop timer
} // timer_stop

/*--------------------------------------------------------------
   Read current timer counter value
--------------------------------------------------------------*/
unsigned int timer_read(int timer_n)
{
	return (T[timer_n]->CNT);
} // timer_read

/*--------------------------------------------------------------
   Reset timer
--------------------------------------------------------------*/
void timer_reset(int timer_n)
{
	T[timer_n]->CTRL = TIMER_RESET;
} // timer_reset

/*--------------------------------------------------------------
   Get interrupt flag
--------------------------------------------------------------*/
unsigned int timer_int_get(int timer_n)
{
	return ( T[timer_n]->IRQ );
}// timer_int_get

/*--------------------------------------------------------------
    Clear interrupt
--------------------------------------------------------------*/
void timer_int_clear(int timer_n)
{
	T[timer_n]->CTRL |= TIMER_INT_REQ;	// Clear interrupt request in CTRL
	T[timer_n]->IRQ  |= 0xff;			// Clear IRQ bits									
} // timer_int_clear

/*--------------------------------------------------------------
  Returns timer fixed frequency (Hz)
--------------------------------------------------------------*/
unsigned int timer_fix_frequency()
{
	return(TIMER_FIX_FREQUENCY);
} // timer_fix_frequency

/*--------------------------------------------------------------
   Read counter value for capture(CAP) registers
--------------------------------------------------------------*/
unsigned int timer_cap_read(int timer_n, unsigned int ch_n)
{
	return ( T[timer_n]->CAP[ch_n] );
} // timer_cap_read

/*--------------------------------------------------------------
   Set frequency devider 
--------------------------------------------------------------*/
void timer_freqdiv_set(int timer_n, unsigned int div)
{
	T[timer_n]->DIV = div ;
	T[timer_n]->CTRL |= (1<<6);	// enable frequency devider
} // timer_freqdiv_set	

/*--------------------------------------------------------------
   Get frequency devider
--------------------------------------------------------------*/
unsigned int timer_freqdiv_get (int timer_n)
{
	return ( T[timer_n]->DIV );
} // timer_freqdiv_get	


