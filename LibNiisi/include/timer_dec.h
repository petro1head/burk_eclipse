/*--------------------------------------------------------*/
/*------------------Timer library------------------------*/
/*--------------------------------------------------------*/

#ifndef _TIMER_DEC_H_
#define _TIMER_DEC_H_

#include "chip_vm044.h"
#include "devdescr_vm044.h"

//   Load timer by given value and start in single/cycle mode, 
//	interrupts disable/enable
void timer_mode(int timer_n, 
				int mode, 
				int int_en, 
				unsigned int freq, 
				unsigned long long value);

//   Start timer
void timer_start(int timer_n);

//   Stop timer
void timer_stop(int timer_n);

//   Read current timer counter value
unsigned int timer_read( int timer_n );

//  Returns timer fixed frequency (Hz)
unsigned int timer_fix_frequency(int timer_n);

//   Reload timer
void timer_reload(int timer_n);

//   Reset timer
void timer_reset(int timer_n);

#endif /* _TIMER_DEC_H_ */
