/*
 * vmeInit.c
 *
 *  Created on: Feb 17, 2020
 *      Author: aL
 */

#include "vmeInit.h"
#include "io.h"
#include "gpio.h"
#include "sys_unit.h"
#include "stdint.h"
#include "timer.h"
#include "chip_vm014.h"
#include "mcpMemoryController.h"
#include "devdescr_vm014.h"
const int gpioLed = 3;
const int clkOutPin = 6;

int _getpid_r( struct _reent * dd) {return 0;}
int _kill_r (struct _reent *dd, int dd1, int dd2) {return 0;}

#define LENGTH 32
#define MBIST_BASE 0xBFF0F000
#define MBIST_CACHE 0xBFF0F000
#define MBIST_MKIO 0xBFF0F004
#define MBIST_SW 0xBFF0F008
#define MBIST_UART 0xBFF0F00c
#define MBIST_SPI 0xBFF0F010
#define MBIST_ID 0xBFF0F07c

//#define MBIST_BASE Array
//#define MBIST_CACHE Array
//#define MBIST_MKIO Array
//#define MBIST_SW Array
//#define MBIST_UART Array
//#define MBIST_SPI Array
//#define MBIST_ID Array

unsigned int Array[LENGTH];

typedef struct {

	unsigned int Bist_En :1;
	unsigned int RunBist :1;
	unsigned int mode_test :1;
	unsigned int Pattern :2;
	unsigned int Rd_Wr :1;
	unsigned int Alg :3;
	unsigned int reserve :22;
	unsigned int Done :1;

} MbistGeneric;

typedef struct {

	unsigned int Bist_En :1;
	unsigned int RunBist :1;
	unsigned int mode_test :1;
	unsigned int Pattern :2;
	unsigned int Rd_Wr :1;
	unsigned int Alg :3;
	unsigned int Mask_Pbuf :1;
	unsigned int Mask_Dtag :1;
	unsigned int Mask_Itag :1;
	unsigned int Mask_Dcache :1;
	unsigned int Mask_Icache :1;
	unsigned int Nway :1;
	unsigned int reserve :1;
	unsigned int Address :10;
	unsigned int Error :5;
	unsigned int Done :1;

} MbistCache;

typedef struct {

	unsigned int Bist_En :1; //1 - enable BIST
	unsigned int RunBist :1;
	unsigned int mode_test :1; //0 - stop on error, 1 - dont stop on error
	unsigned int Pattern :2;
	unsigned int Rd_Wr :1;
	unsigned int Alg :3;
	unsigned int reserve2 :1;
	unsigned int Mask_mkio2 :2;
	unsigned int Mask_mkio1 :2;
	unsigned int N_port :1;
	unsigned int reserve :3;
	unsigned int Address :11;
	unsigned int Error :2;
	unsigned int Done :1;

} MbistMkio;

typedef struct {

	unsigned int Bist_En :1;
	unsigned int RunBist :1;
	unsigned int mode_test :1;
	unsigned int Pattern :2;
	unsigned int Rd_Wr :1;
	unsigned int Alg :3;
	unsigned int RxTx :1;
	unsigned int reserve :2;
	unsigned int Address :8;
	unsigned int Error :11;
	unsigned int Done :1;

} MbistSw;

typedef struct {

	unsigned int Bist_En :1;
	unsigned int RunBist :1;
	unsigned int mode_test :1;
	unsigned int Pattern :2;
	unsigned int Rd_Wr :1;
	unsigned int Alg :3;
	unsigned int reserve2 :3;
	unsigned int Address :4;
	unsigned int reserve :11;
	unsigned int Error :4;
	unsigned int Done :1;

} MbistUart;

typedef struct {

	unsigned int Bist_En :1;
	unsigned int RunBist :1;
	unsigned int mode_test :1;
	unsigned int Pattern :2;
	unsigned int Rd_Wr :1;
	unsigned int Alg :3;
	unsigned int reserve2 :2;
	unsigned int Address :5;
	unsigned int reserve :11;
	unsigned int Error :4;
	unsigned int Done :1;

} MbistSpi;

unsigned int MbistId;

enum {
	stateModuleCache,
	stateModuleMkio,
	stateModuleSw,
	stateModuleUart,
	stateModuleSpi,
	stateModuleId,
	stateModuleExit
};

enum {
	stateModuleCacheNway0, stateModuleCacheNway1, stateModuleCacheNwayExit
};

enum {
	stateModuleMkioNport0, stateModuleMkioNport1, stateModuleMkioNportExit
};

enum {
	stateModuleSwRxTx0, stateModuleSwRxTx1, stateModuleSwRxTxExit
};

enum {
	stateModuleAlgMarch0,
	stateModuleAlgMarch1,
	stateModuleAlgMarch2,
	stateModuleAlgPattern0,
	stateModuleAlgPattern1,
	stateModuleAlgPattern2,
	stateModuleAlgPattern3,
	stateModuleAlgExit
};

int mainStudent(void) {

	int stateModule = 0;
	int stateModuleGeneric = 0;
	int stateModuleAlg = 0;
	int counter = 0;

	MbistGeneric *mbistCurrentModule;
	MbistCache *mbistCache;
	unsigned int *mkioCurrent;
	unsigned int *mbistId;

	counter = LENGTH;
	while (counter--)
		Array[counter] = 0;

	//mbistCurrentModule = Array;//это для того что бы билдилось и отлаживалось на ПК

	for (;;) {

		MBIST_TEST_START:

		switch (stateModule) { //перебор модулей
		case stateModuleCache:
			mbistCurrentModule = MBIST_CACHE;
			stateModule++;
			goto MBIST_TEST_START;
			break;
		case stateModuleMkio:
			mbistCurrentModule = MBIST_MKIO;
			break;
		case stateModuleSw:
			mbistCurrentModule = MBIST_SW;
			break;
		case stateModuleUart:
			mbistCurrentModule = MBIST_UART;
			stateModule++;
			goto MBIST_TEST_START;
			break;
		case stateModuleSpi:
			mbistCurrentModule = MBIST_SPI;
			stateModule++;
			goto MBIST_TEST_START;
			break;
		case stateModuleId:
			mbistId = MBIST_ID;
			break;
		default:
			return 1;
		}

		if(mbistCurrentModule != MBIST_UART){
		*(unsigned int *)(mbistCurrentModule) = 0;
		mbistCurrentModule->Bist_En = 1;
		mbistCurrentModule->mode_test = 1;
		} else {
			counter++;
		}
		switch (stateModule) { //перебор модулей
		case stateModuleCache:

			switch (stateModuleGeneric) {
			case stateModuleCacheNway0:
				((MbistCache*) mbistCurrentModule)->Nway = 0;
				break;
			case stateModuleCacheNway1:
				((MbistCache*) mbistCurrentModule)->Nway = 1;
				break;
			case stateModuleCacheNwayExit:
				stateModuleGeneric = 0;
				stateModule++;
				goto MBIST_TEST_START;
				break;
			}
			if (stateModuleAlg < stateModuleAlgExit
					&& stateModuleGeneric < stateModuleCacheNwayExit)
				tos("Module CACHE\nNway  ");
				ton(stateModuleGeneric);
				tos("\n");
			break;
		case stateModuleMkio:

			switch (stateModuleGeneric) {
			case stateModuleMkioNport0:
				((MbistMkio*) mbistCurrentModule)->N_port = 0;
				break;
			case stateModuleMkioNport1:
				((MbistMkio*) mbistCurrentModule)->N_port = 1;
				break;
			case stateModuleMkioNportExit:
				stateModuleGeneric = 0;
				stateModule++;
				goto MBIST_TEST_START;
				break;
			}
			if (stateModuleAlg < stateModuleAlgExit
					&& stateModuleGeneric < stateModuleMkioNportExit)
				tos("Module MKIO\nN_port ");
				ton(stateModuleGeneric);
				tos("\n");
			break;
		case stateModuleSw:

			switch (stateModuleGeneric) {
			case stateModuleSwRxTx0:
				((MbistSw*) mbistCurrentModule)->RxTx = 0;
				break;
			case stateModuleSwRxTx1:
				((MbistSw*) mbistCurrentModule)->RxTx = 1;
				break;
			case stateModuleSwRxTxExit:
				stateModuleGeneric = 0;
				stateModule++;
				goto MBIST_TEST_START;
				break;
			}
			if (stateModuleAlg < stateModuleAlgExit
					&& stateModuleGeneric < stateModuleSwRxTxExit)
				tos("Module SW\nRxTx ");
				ton(stateModuleGeneric);
				tos("\n");
			break;
		case stateModuleUart:
			if (stateModuleAlg < stateModuleAlgExit && stateModuleGeneric == 0)
				tos("Module UART\n");
			switch (stateModuleGeneric) {
			case 0:
				break;
			case 1:
				stateModuleGeneric = 0;
				stateModule++;
				goto MBIST_TEST_START;
				break;
			}
			break;
		case stateModuleSpi:
			if (stateModuleAlg < stateModuleAlgExit && stateModuleGeneric == 0)
				tos("Module SPI\n");
			switch (stateModuleGeneric) {
			case 0:
				break;
			case 1:
				stateModuleGeneric = 0;
				stateModule++;
				goto MBIST_TEST_START;
				break;
			}
			break;
		case stateModuleId:
			tos("Module ID\n");
			MbistId = mbistId;
			if (MbistId == 0x2C001FFF) {            //738 205 695
				tos("Test done\n\n");
			} else
				tos("Test not done\n\n");
			stateModule++;
			goto MBIST_TEST_START;
			break;
		default:
			return 1;
		}

		switch (stateModuleAlg) {
		case stateModuleAlgMarch0:
			tos("Algorithm March LR\n");
			mbistCurrentModule->Alg = 1;
			mbistCurrentModule->Pattern = 0;
			break;
		case stateModuleAlgMarch1:
			tos("Algorithm March C-\n");
			mbistCurrentModule->Alg = 2;
			mbistCurrentModule->Pattern = 0;
			break;
		case stateModuleAlgMarch2:
			tos("Algorithm MATS++\n");
			mbistCurrentModule->Alg = 4;
			mbistCurrentModule->Pattern = 0;
			break;
		case stateModuleAlgPattern0:
			tos("Hold test 0\n");
			mbistCurrentModule->Alg = 0;
			mbistCurrentModule->Pattern = 0;
			break;
		case stateModuleAlgPattern1:
			tos("Hold test 1\n");
			mbistCurrentModule->Alg = 0;
			mbistCurrentModule->Pattern = 1;
			break;
		case stateModuleAlgPattern2:
			tos("Hold test 2\n");
			mbistCurrentModule->Alg = 0;
			mbistCurrentModule->Pattern = 2;
			break;
		case stateModuleAlgPattern3:
			tos("Hold test 3\n");
			mbistCurrentModule->Alg = 0;
			mbistCurrentModule->Pattern = 3;
			break;
		case stateModuleAlgExit:
			stateModuleGeneric++;
			stateModuleAlg = -1;
			break;
		}

		switch (stateModuleAlg) {
		case stateModuleAlgMarch0:
		case stateModuleAlgMarch1:
		case stateModuleAlgMarch2:
			//	сделать запуск теста, проверку результатов
			counter = 0;
			mbistCurrentModule->RunBist = 1;
			while (mbistCurrentModule->Done != 1) {
				if (counter > 100000) {
					tos("Timeout error\n\n");
					break;
				}
				counter++;
			}
			if (mbistCurrentModule->Done == 1)
				tos("Test done\n");
			mbistCurrentModule->Bist_En = 0;
			mbistCurrentModule->RunBist = 0;
			break;
		case stateModuleAlgPattern0:
		case stateModuleAlgPattern1:
		case stateModuleAlgPattern2:
		case stateModuleAlgPattern3:
			//	сделать запуск теста, проверку результатов
			counter = 0;
			mbistCurrentModule->Rd_Wr = 1;
			mbistCurrentModule->RunBist = 1;
			while (mbistCurrentModule->Done != 1) {
				if (counter > 100000) {
					tos("Timeout error at the first stage of testing\n");
					break;
				}
				counter++;
			}
			mbistCurrentModule->Bist_En = 0;
			mbistCurrentModule->RunBist = 0;

			//базовый адрес для регистров контроллера интефейса MKIO A: 0xBFF01000
			// область памяти контроллера MKIO A 0xBFF06000
			if (mbistCurrentModule == MBIST_MKIO) {
				mkioCurrent = 0xBFF06104;
				//*mkioCurrent = 13521;
			}
			mbistCurrentModule->Bist_En = 1;
			mbistCurrentModule->Rd_Wr = 0;
			mbistCurrentModule->RunBist = 1;
			while (mbistCurrentModule->Done != 1) {
				if (counter > 100000) {
					tos("Timeout error at the second stage of testing\n\n");
					break;
				}
				counter++;
			}
			if (mbistCurrentModule->Done == 1){
				tos("Test done\n");
				if((mbistCurrentModule == MBIST_SW) && (stateModuleAlg == stateModuleAlgPattern3)){
					mbistCurrentModule = MBIST_ID;
					stateModule = stateModuleId;
					stateModuleAlg = 0;
					stateModuleGeneric = 0;

				}
			}
			switch (stateModule) {			//перебор модулей
			case stateModuleCache:
				if (((MbistCache*) mbistCurrentModule)->Error) {
					tos("Error:  ");
					ton(((MbistCache*) mbistCurrentModule)->Error);
					tos("\n");
					tos("Address:  ");
					ton(((MbistCache*) mbistCurrentModule)->Address);
					tos("\n");
					tos("\n");
				}
				break;
			case stateModuleMkio:
				if (((MbistMkio*) mbistCurrentModule)->Error) {
					tos("Error:  ");
							ton(((MbistMkio*) mbistCurrentModule)->Error);
							tos("\n");
					tos("Address:  ");
							ton(((MbistMkio*) mbistCurrentModule)->Address);
							tos("\n");
							tos("\n");
				}
				break;
			case stateModuleSw:
				if (((MbistSw*) mbistCurrentModule)->Error) {
					tos("Error:  ");
					ton(((MbistSw*) mbistCurrentModule)->Error);
					tos("\n");
					tos("Address:  ");
					ton(((MbistSw*) mbistCurrentModule)->Address);
					tos("\n");
					tos("\n");
				}
				break;
			case stateModuleUart:
				if (((MbistUart*) mbistCurrentModule)->Error) {
					tos("Error:  ");
							ton(((MbistUart*) mbistCurrentModule)->Error);
							tos("\n");
					tos("Address:  ");
							ton(((MbistUart*) mbistCurrentModule)->Address);
							tos("\n");
							tos("\n");
				}
				break;
			case stateModuleSpi:
				if (((MbistSpi*) mbistCurrentModule)->Error) {
					tos("Error:  ");
					ton(((MbistSpi*) mbistCurrentModule)->Error);
					tos("\n");
					tos("Address:  ");
					ton(((MbistSpi*) mbistCurrentModule)->Address);
					tos("\n");
					tos("\n");
				}
				break;
			case stateModuleId:
				break;
			default:
				return 1;
			}
			mbistCurrentModule->RunBist = 0;
			mbistCurrentModule->Bist_En = 0;
			break;
		}

		stateModuleAlg++;
		if (stateModule == stateModuleExit)
			break;

	}

}

void delay(int iter) {
	for (volatile int i = 0; i < iter; ++i)
		;
}

typedef struct {
	volatile unsigned int BankNLoBorder;
	volatile unsigned int BankNHiBorder;
	volatile unsigned int BankNConfig;
	volatile unsigned int BankNdataWaitState;
	volatile unsigned int BankNDelay;
	volatile unsigned int BankNoffDelay;
	volatile unsigned int BankNburstDelayConfig;
	volatile unsigned int BankNErrorAddress;
	volatile unsigned int dummy[56];

}__attribute__ ((__packed__)) MEMCTRL_BANKREG;

typedef struct {
	MEMCTRL_BANKREG bank[4];
	volatile unsigned int dummy[705];
	volatile unsigned int UserAreaLowBorder;
	volatile unsigned int UserAreaHiBorder;
	volatile unsigned int ECC_byteValue;
	volatile unsigned int RAW_dataLow;
	volatile unsigned int RAW_dataHi;
	volatile unsigned int RAW_addr;
	volatile unsigned int RAW_config;
	volatile unsigned int dummy1[56];
	volatile unsigned int MemCtrlStat;
	volatile unsigned int MemCtrlBusError;

}__attribute__ ((__packed__)) MEMCTRL_UNIT;

//volatile const char www[50000] = {32};
//#define MEMCTRL_BASE 0xBFF08000

#define ADDR_SRAM_MIN 0x80800000
#define ADDR_SRAM_MAX 0x80DFFFFF

uint32_t t0, t1;
int ntimer = 2;

#define SCRUBADDR SCRUBADDR_VERIFY

#define SCRUBADDR_JUST_SCRUB(addr) *(addr++)
#define SCRUBADDR_VERIFY(addr) if ( (val = *(addr)) != addr) {tos("error!");addr++; } else {addr++;}

void scrub() {
	volatile register uint32_t * addr;
	volatile register uint32_t val;
	//volatile register uint32_t addr_sram_max = ADDR_SRAM_MAX;
	t0 = timer_read(ntimer);
	for (addr = ADDR_SRAM_MIN; addr <= ADDR_SRAM_MAX; addr++) {
		*addr = addr;
	}
	t1 = timer_read(ntimer);
	tos("Write SRAM time = ");
	ton10(t1 - t0);
	tos(" us\n");

	t0 = timer_read(ntimer);
	for (addr = ADDR_SRAM_MIN; addr <= ADDR_SRAM_MAX;) {
		SCRUBADDR(addr);
		SCRUBADDR(addr);
		SCRUBADDR(addr);
		SCRUBADDR(addr);

		SCRUBADDR(addr);
		SCRUBADDR(addr);
		SCRUBADDR(addr);
		SCRUBADDR(addr);

		SCRUBADDR(addr);
		SCRUBADDR(addr);
		SCRUBADDR(addr);
		SCRUBADDR(addr);

		SCRUBADDR(addr);
		SCRUBADDR(addr);
		SCRUBADDR(addr);
		SCRUBADDR(addr);
	}
	t1 = timer_read(ntimer);
	tos("Read SRAM time = ");
	ton10(t1 - t0);
	tos(" us\n");

}

volatile MEMCTRL_UNIT *MEMCTRL = (MEMCTRL_UNIT *) MEMCTRL_BASE;

volatile TIMER_UNIT * timer = (TIMER_UNIT *) TIMER2_BASE;

void vmeInit() {
	int i;
	int a = 0;
	tos("\n\nvmeInit\n");
	volatile SYS_UNIT *SYS = (SYS_UNIT *) SYSUNIT_BASE;
	volatile GPIO_UNIT *GPIO = (GPIO_UNIT *) GPIO_BASE;
	volatile UART_UNIT *UART = (UART_UNIT *) UART1_BASE;

	volatile UART_UNIT *UARTA = (UART_UNIT *) UART0_BASE;

	volatile TIMER_UNIT *TIMER2 = (TIMER_UNIT *)TIMER2_BASE;



	volatile unsigned GPIO_MODE6 = GPIO->MODE[6];
	volatile unsigned GPIO_DIR;

	//PLL_program(3, 25, 2, 4); // 50 MHz
	PLL_program(3, 25, 2, 2); // 100 MHz

	SYS->PLL_CTRL &= ~(1 << 12); // PLL normal mode (no bypass)
//	SYS->PLL_CTRL |= (1<<12); // PLL bypass mode

	volatile int pllfreq = pll_get_freq();
	tos("CPU freq : "); ton10(pllfreq); tos(" MHz\n");

	gpio_init();
	gpio_write_dir(gpioLed | (1 << clkOutPin));	// set OUT for GPIO0
	gpio_write_dir((gpioLed | (0x1 << 10)) | (1 << clkOutPin));	// set OUT for GPIO0
	GPIO_DIR = GPIO->DIR;
	gpio_write_mode(clkOutPin, 1, 0, 0, 0, 0); // turn alt func 0 ON
	GPIO_DIR = GPIO->DIR;
	gpio_set(1);
	gpio_clear(2);
	GPIO_MODE6 = GPIO->MODE[6];








	SYS->DIS_CLK = 0;
	volatile unsigned STATUS1 = SYS->STATUS1;
	volatile unsigned status2 = SYS->STATUS2;
	volatile unsigned DIS_CLK = SYS->DIS_CLK;
	volatile unsigned DIV_CTRL = SYS->DIV_CTRL;
	volatile unsigned PLL_CTRL = SYS->PLL_CTRL;




	//watchdog setup
//	TIMER2->DIV = 10;//делитель частоты
//	TIMER2->CCMP  = 0x8;//разрешение сброса по совпадению данных таймера и регистра CMP0
//	TIMER2->CMP[0] = 0x10000000;
//	TIMER2->CTRL = 0xC2;//разрешение таймера, делителя и сброса
//
//	GPIO->MODE[14] = 0x14;//enable alt function  Сигнал сброса от таймера Таймер 2
//	DIS_CLK = DIS_CLK&(~(0x1<<4));
//


	SYS->DIS_CLK= DIS_CLK;


	status2 &= ~(1<<10);//disable clock from 25 mhz
	status2 |= (1<<11);//enable clock from mux_CLK (should be 100 mhz)

	SYS->STATUS2 = status2 ;

	//MKO inversion setup for MCP
//	STATUS1 |=  0xc;
//	SYS->STATUS1 = STATUS1;
	SYS->STATUS1 = 0;
//	SYS->DIS_CLK = 0;

//	GPIO_DIR = GPIO->DIR;
//	MEMCTRL->bank[2].BankNburstDelayConfig = 0x0101 | (1 << 31); //burst delay - 4 (or burst delay -5 if CS_offDelay != 0)
//	MEMCTRL->bank[2].BankNdataWaitState = 0x0202; //1st cycle delay - 3
//	MEMCTRL->bank[2].BankNoffDelay = 0x06060202; // burst last cycle

	//	MEMCTRL->bank[2].BankNburstDelayConfig = 0x0101 | (1 << 31); //burst delay - 4 (or burst delay -5 if CS_offDelay != 0)



	//MSB | WriteWaitState[7..0] | ReadWaitState[7..0] |LSB
	//MEMCTRL->bank[3].BankNdataWaitState = 0x1f1a; //1st cycle delay -

	//MSB| CSwrite_offDelay[7..0] | WE_offDelay[7..0] | OE_offDelay[7..0] | CSread_offDelay[7..0] | LSB
	//MEMCTRL->bank[3].BankNoffDelay = 0x1f1a1f1f; // burst last cycle




//	MEMCTRL->bank[3].BankNConfig = 0x100;

	//off delay MSB| CSwrite_offDelay[7..0] | WE_offDelay[7..0] | OE_offDelay[7..0] | CSread_offDelay[7..0] | LSB

	// wait states MSB | WriteWaitState[7..0] | ReadWaitState[7..0] |LSB

	//off delay MSB| CSwrite_offDelay[7..0] | WE_offDelay[7..0] | OE_offDelay[7..0] | CSread_offDelay[7..0] | LSB

	// wait states MSB | WriteWaitState[7..0] | ReadWaitState[7..0] |LSB
//	/*CS3	  */  .word	0x00400000, 0x00500000, 0x00000000, 0x00000807, 0x08060808, 0x00000900,	0x00000003, 0 		// 4 3


	GPIO->AFIO = 0x00;
	timer_mode(ntimer, TIMER_CYCLE, TIMER_INT_DIS, TIMER_FIXFREQ);
	timer_freqdiv_set(ntimer, 23);
	timer_start(ntimer);
	volatile unsigned int LCR = UART->LCR;

	a = 50;

	gpio_invert(0x1<<10);
	gpio_invert(0x1<<10);


//	GPIO->CLR = 0;
//	GPIO->CLR = 0x1 << 10;
//	GPIO->SET = 0;
//	GPIO->SET = 0x1 << 10;
//
	//mainStudent();
	for (;;) {
		a--;
//		scrub();
	mainStudent();
		gpio_invert(gpioLed);
//		vmeSelfTestUart();
		uart_send_byte(1, 0xff);
	}
	return;
}
