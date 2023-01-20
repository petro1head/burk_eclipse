/*
 * main.c
 *
 *  Created on: 20.06.2018
 *      Author: Dmitry
 */
//#include "vmeInit.h"

#include <string.h>
#include <stdlib.h>
/*
int	_raise_r(struct _reent * a, int b)
{
	return 0;
}
*/

int _getpid_r(struct _reent * a)
{
	return 0;
}

int _kill_r(struct _reent * a, int b, int c)
{
	return 0;
}

_PTR _calloc_r(struct _reent * a, size_t b, size_t c)
{
	return 0;
}

//int testFunc(int testVar)
//{
//	testVar +=1;
//	return testVar;
//}
//
//
//int main (void)
//{
//	int counter = 0;
//	int testVar2 = 1;
//	unsigned int * testCs1Word = 0xa00f8000;
//	*testCs1Word = 1;
//	mcpInitMemoryController();
//	while(1){
//		testVar2 = testFunc(testVar2);
//		*testCs1Word = testVar2 ;
//		if(counter > 0x1000){
//			counter = 0;
//			tos("\nAddress: ");
//			ton(testCs1Word);
//			tos(" - write: ");
//			ton(testVar2);
//			tos(" - read: ");
//			ton(*testCs1Word);
//			ton(testVar2);
//			tos("\n");
//		}
//		testCs1Word++;
//		counter++;
//
//	}
//	return 0;
//}








//int main (void)
//{
//	int cycleCount;
//	char *str = "12";
//	int n;
//
//	n = atoi(str);
//
//	int gpioLed =3;
//	vmeInit();
//
//
//	while (1)
//	{
//		gpio_invert(gpioLed);
//		delay(100000);
//		cycleCount++;
//		tos("LED ON/OFF count: "); ton10(cycleCount); tos("\n");
//	}
//
//	return 0;
//}

/* Passive Serial configuration FPGA
 * Author: Anastasiya
 */

/*---------------define----------------*/

#include "devdescr_vm014.h"
#include "chip_vm014.h"
#include "gpio.h"
#include "uart.h"
#include "io.h"



// GPIO base address
//#define	GPIO_BASE   0xBFF00700

// GPIO pin numbers that control FPGA pins
#define nCONFIG      14
#define nSTATUS      16
#define CONF_DONE    17
#define DCLK         0
#define DATA0        1


// start address of configuration array
#define START_OF_ARRAY    0xA0300000

// array end address
#define END_OF_ARRAY      0xA0317EE7

/*---------------Function Prototypes------------------*/

//void gpio_initial();
//void delay(int iter);


/*--------- GPIO controller ----------------*/

//GPIO structure with offsets
//typedef struct {
//volatile unsigned int IDREG;	// 0x00
//volatile unsigned int IN;		// 0x04
//volatile unsigned int OUT;		// 0x08
//volatile unsigned int SET;		// 0x0c
//volatile unsigned int CLR;		// 0x10
//volatile unsigned int INVERT;	// 0x14
//volatile unsigned int DIR;		// 0x18
//volatile unsigned int INTFLAG;	// 0x1c
//
//volatile unsigned int AFIO;		// 0x20
//volatile unsigned int AFMODE;	// 0x24
//volatile unsigned int dummy;	// 0x28
//volatile unsigned int MODE[32];	// 0x2c-0xa8
//} GPIO_UNIT;


/*------------- delay ---------------*/

void delay(int iter)
{
	for (volatile int i = 0; i < iter; ++i);
}

/*-------------Initialization GPIO-------------*/

//void gpio_initial()
//{
//pointer to GPIO_UNIT structure with base address 0xBFF00700
//volatile GPIO_UNIT *GPIO = (GPIO_UNIT *)GPIO_BASE;
//
//				/*        Register RGPIO_DIR
//                      Transfer direction for pins GPIO[31:0]
//                      0 - input, 1 - output
//                */
//volatile unsigned int i;
//
//GPIO->INTFLAG= 0xffffffff;	// Clear ints
//GPIO->AFIO = 0;				// Set PIO mode
//GPIO->DIR = 0;				// all ports as input
//
//for (i=0; i<32; i++)
//    GPIO->MODE[i] = 0;

//  nCONFIG, DCLK, DATA0 - output (1)
// 	nSTATUS, CONF_DONE - input, because they need to be read (0)

//GPIO->DIR = (1<<nCONFIG)|(1<<DCLK)|(1<<DATA0)|(0<<nSTATUS)|(0<<CONF_DONE);
//GPIO->OUT = (0<<nCONFIG)|(0<<DCLK)|(0<<DATA0);
//
//} // gpio_initial


int main()
{

volatile GPIO_UNIT *GPIO = (GPIO_UNIT *)GPIO_BASE;

unsigned char data;// bitstream
int m = 0;// 1 bit

	char *str = "123.12345678";
	double d;
	d = atof(str);


	char *str1 = "55555346";
	unsigned long int l;
	l = atol(str1);

	char *str2 = "123";
	int i;
	i = atoi(str2);

char dst1[10] = "11111111";
char dst2[10] = "";

//strncpy (dst2, dst1, 5); // work

strcpy (dst2, dst1);

gpio_init();
//  nCONFIG, DCLK, DATA0 - output (1)
// 	nSTATUS, CONF_DONE - input, because they need to be read (0)

GPIO->DIR = (1<<nCONFIG)|(1<<DCLK)|(1<<DATA0)|(0<<nSTATUS)|(0<<CONF_DONE);
GPIO->OUT = (0<<nCONFIG)|(0<<DCLK)|(0<<DATA0);

//delay(10);

// set nCONFIG in high state ( 0 -> 1)
GPIO->OUT |= (1<<nCONFIG);


unsigned char * start_add = (unsigned char *)START_OF_ARRAY;
data = *start_add;

unsigned char * end_add = (unsigned char *)END_OF_ARRAY;


if ((GPIO->IN >> nSTATUS) & 1) // true
   {
		//ton10(1);
		//tos("\n");
   }
else
	{
		//ton10(0);
		//tos("\n");
	}

if (((GPIO->IN >> CONF_DONE)&1) == 0)
   {
		//ton10(0);
		//tos("\n");
   }
else
	{
		//ton10(1);
		//tos("\n");
	}


while ((GPIO->OUT |= (1<<nCONFIG)) && (((GPIO->IN >> nSTATUS) & 1) == 1) && (start_add < end_add) && (((GPIO->IN >> CONF_DONE)&1) == 0))
	{
             // FALLING
			/*for (m = 0; m < 8; m++) // bits с 0 по 7  falling DCLK
			{
				GPIO->OUT &= ~(1<<DCLK);
				if (((data >> m) & 1) == 1)
				{
					GPIO->OUT |= (1<<DATA0);
					delay(1);
					GPIO->OUT |= (1<<DCLK);
					delay(1);
				}
				 else
					{
						GPIO->OUT &= ~(1<<DATA0);
						delay(1);
						GPIO->OUT |= (1<<DCLK);
						delay(1);
					}
			}
			*/
		if (((GPIO->IN >> nSTATUS)&1) == 0)
		{
			GPIO->OUT &= ~(1<<DCLK);
		//	ton10 (error nSTATUS 0);
		//    tos("\n");
		}
		else
		 {
			 //LSB
			for (m = 0; m < 8; m++) // bits с 0 по 7
			{
				// RISING DCLK
				GPIO->OUT |= (1<<DCLK); // set 1 in DCLK
				if (((data >> m) & 1) == 1)
				{
					GPIO->OUT |= (1<<DATA0);
					delay(1);
					GPIO->OUT &= ~(1<<DCLK);
					delay(1);
				}
				 else
					{
						GPIO->OUT &= ~(1<<DATA0);
						delay(1);
						GPIO->OUT &= ~(1<<DCLK);
						delay(1);
					}
			}
			 data = *(start_add++);
		 }


		 	/*//MSB
	 for (m = 7; m >= 0; m--) // bits с 7 по 0
					{
						// RISING DCLK
						GPIO->OUT |= (1<<DCLK); // set 1 in DCLK
						if (((data >> m) & 1) == 1)
						{
							GPIO->OUT |= (1<<DATA0);
							delay(1);
							GPIO->OUT &= ~(1<<DCLK);
							delay(1);
						}
						 else
							{
								GPIO->OUT &= ~(1<<DATA0);
								delay(1);
								GPIO->OUT &= ~(1<<DCLK);
								delay(1);
							}
					}
					 data = *(++start_add);
					 */
	}

//GPIO->OUT &= ~(1<<DATA0);


		for (volatile int i = 0; i <64; i++)
		{
			GPIO->OUT |= (1<<DCLK);
			delay(1);
			GPIO->OUT &= ~(1<<DCLK);
			delay(1);
		}



	if (((GPIO->IN >> CONF_DONE)&1) == 1)
   {
	//	ton10(Ok CONF_DONE 1);
	//	tos("\n");
   }
else
	{
	//	ton10(error CONF_DONE 0);
	//	tos("\n");
	}
GPIO->OUT &= ~(1<<DCLK);

return 0;
}







