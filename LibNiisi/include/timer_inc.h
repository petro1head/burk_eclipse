/*--------------------------------------------------------*/
/*------------------Timer library------------------------*/
/*--------------------------------------------------------*/

#ifndef _TIMER_INC_H_
#define _TIMER_INC_H_

#include "chip.h"
#include "devdescr.h"

//   Set timer mode
void timer_mode(int timer_n, 
				int cycl_single, 
				int en_dis_intrpt, 
				unsigned int fix_sys_freq);

//   Set mode for compare (CMP) register
void timer_cmp_mode(int timer_n, 
					unsigned int ch_n, 
					unsigned int mode, 
					unsigned int value );

//   Set mode for capture(CAP) register
void timer_cap_mode(int timer_n, 
					unsigned int ch_n, 
					unsigned int mode);
				   
//   Start timer
void timer_start(int timer_n);

//   Stop timer
void timer_stop(int timer_n);

//   Read current timer counter value
unsigned int timer_read(int timer_n);

//   Get interrupt flag
unsigned int timer_intf_get(int timer_n);

//    Clear interrupt
void timer_int_clear(int timer_n);

//  Returns timer fixed frequency (Hz)
unsigned int timer_fix_frequency();

//    Read counter value from capture(CCAP) register
unsigned int timer_cap_read(int timer_n, 
				   unsigned int ch );
				   
//   Set frequency devider 
void timer_freqdiv_set(int timer_n, unsigned int div );	

//   Get frequency devider
unsigned int timer_freqdiv_get (int timer_n);


#endif /* _TIMER_INC_H_ */
