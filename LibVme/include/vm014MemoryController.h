/*
 * vm014MemoryController.h
 *
 *  Created on: 14 апр. 2021 г.
 *      Author: Dmitry
 */

#ifndef SRC_VM014MEMORYCONTROLLER_H_
#define SRC_VM014MEMORYCONTROLLER_H_
#include "chip.h"

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

}  __attribute__ ((__packed__)) MEMCTRL_BANKREG;

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

}  __attribute__ ((__packed__)) MEMCTRL_UNIT;




#endif /* SRC_VM014MEMORYCONTROLLER_H_ */
