/*--------------------------------------------------------*/
/*------------------Sys unit library------------------------*/
/*--------------------------------------------------------*/

#ifndef _SYSUNIT_H_
#define _SYSUNIT_H_

#include "chip.h"
#include "devdescr.h"

/*----------------- Prototypes for Sys_unit ----------------*/
void sys_dis_clk(int device, int dis);
int pll_get_freq();
int PLL_program(int div_ref, int div, int vcod, int postd);
int log2(int power_of_2);
void del(unsigned d);

#endif /* _SYSUNIT_H_ */
