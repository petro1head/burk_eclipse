/*------------------------------------------------------*/
/*------------------Sys unit library -----------------------*/
/*------------------------------------------------------*/

#ifndef _SYSUNIT_C_
#define _SYSUNIT_C_

#include "sys_unit.h"

#define LOCK_DELAY 20	/* PLL lock delay */

void sys_dis_clk( int device, int dis)
{
	volatile SYS_UNIT *SYS = (SYS_UNIT *)SYSUNIT_BASE;	
	SYS->DIS_CLK |= 1 << device;				// disable clock
	if (dis)
 	{
 	  SYS->DIS_CLK |= 1 << device;	// disable clock
 	}
	else
  	{
  	 SYS->DIS_CLK &= ~(1 << device);	// set going clock
  	}	
} // sys_unit_func

/*------------------------------------------------------------------*/
/* on=1 - ON, on=0 - OFF */
static int sys_clk_24(int on)
{
	volatile SYS_UNIT *SYS = (SYS_UNIT *)SYSUNIT_BASE;	
	if (on)
 	{
 	  SYS->DIS_CLK |= 1 << CLK_OCP_24;	// turn clock to 24 MHz
 	}
	else
	{
  	 SYS->DIS_CLK &= ~(1 << CLK_OCP_24);	// turn clock to normal
	}

	return 0;
} // sys_clk_24


/*------------------------------------------------------------------*/
// RD, FD, OD - dividers as used in formula
int PLL_program(int div_ref, int div, int vcod, int postd)
{
	volatile SYS_UNIT *SYS = (SYS_UNIT *)SYSUNIT_BASE;	

	sys_clk_24(1);

	SYS->PLL_CTRL = (SYS->PLL_CTRL & ~0xffff0f00) | //
                	(div_ref     << 24) |           //m
					(div         << 16) | 			//n
					(log2(vcod)  << 10) | 			//vcod
					(log2(postd) << 8);				//pd
	del(LOCK_DELAY);

	sys_clk_24(0);

	return 0;
} // PLL_program

/*------------------------------------------------------------------*/
int pll_get_freq()
{
	volatile int div_ref, div, vcod, post_d, pll_freq;

	volatile SYS_UNIT *SYS = (SYS_UNIT *)SYSUNIT_BASE;	

	/* pll_freq =  24 * div(N) / div_ref(M) / post_d */

	div_ref = (SYS->PLL_CTRL >> 24) & 0xff;
	div     = (SYS->PLL_CTRL >> 16) & 0xff;
	vcod    = 1 << ((SYS->PLL_CTRL >> 10) & 3);
	post_d  = 1 << ((SYS->PLL_CTRL >> 8)  & 3);
	pll_freq = 24 * div / div_ref / post_d;
	
	return pll_freq;
} // pll_get_freq

/*------------------------------------------------------------------*/
void del(unsigned d)
{	
	volatile unsigned i;
	for (i=1; i<=d; i++)
	{}

}// del

/*------------------------------------------------------------------*/
int log2(int power_of_2)
{
	return (power_of_2==1) ? 0 : (power_of_2==2) ? 1 : 
			(power_of_2==4) ? 2 : (power_of_2==8) ? 3 : 0;
} // log2

/*------------------------------------------------------------------*/

#endif /* _SYSUNIT_C_ */
