/*------------------------------------------------------*/
/*------------------GPIO library------------------------*/
/*------------------------------------------------------*/

#ifndef _GPIO_H_
#define _GPIO_H_

#include "chip.h"
#include "devdescr.h"

/*----------------- Prototypes -------------------------*/
void gpio_init();
void gpio_set(int set_mask);
void gpio_clear(int clr_mask);
void gpio_invert(int invert_mask);
void gpio_write_dir(int out_mask);
int gpio_read_dir();
void gpio_write_altfunc(int altfunc_mask);
int gpio_read_altfunc();
void gpio_write_altmode(int altmode_mask);
int gpio_read_altmode();
int gpio_read_input(); 
void gpio_write_output(int out_val);
int gpio_read_output();
void gpio_write_mode(int line, 
					int alt_b, 
					int int_b, 
					int out_b, 
					int alt_func, 
					int int_mode);
int gpio_read_intflag();
void gpio_write_intflag(int int_flag);
int gpio_read_id();

#endif /* _GPIO_H_ */
