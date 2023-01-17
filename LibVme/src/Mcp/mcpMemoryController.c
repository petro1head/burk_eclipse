/*
 * mcpMemoryController.c
 *
 *  Created on: 17 мая 2021 г.
 *      Author: Dmitry
 */


#include "mcpMemoryController.h"
#include "vm014MemoryController.h"
#include "chip_vm014.h"
const int mcpBootRomBank = 0;
const int mcpNvRamBankStart = 1; // PZU MRAM starting CS
const int mcpNvRamBankNum = 3; // PZU MRAM number of CS


int mcpInitMemoryController()
{
	volatile MEMCTRL_UNIT *MEMCTRL = (MEMCTRL_UNIT *)MEMCTRL_BASE;

	// BOOT ROM AREA MARGINS SETUP
	MEMCTRL->bank[mcpBootRomBank].BankNLoBorder = 0x1FC00000;
	MEMCTRL->bank[mcpBootRomBank].BankNHiBorder = 0x1FE00000;

	// NV RAM AREA MARGINS SETUP - total 6Mbytes
	// CS1
	MEMCTRL->bank[mcpNvRamBankStart].BankNLoBorder = 0;
	MEMCTRL->bank[mcpNvRamBankStart].BankNHiBorder = 0x00100000; // end address 0x001FFFFF - Total 2Mbytes

	// CS2
	MEMCTRL->bank[mcpNvRamBankStart+1].BankNLoBorder = 0x00200000 ;
	MEMCTRL->bank[mcpNvRamBankStart+1].BankNHiBorder = 0x00300000; // end address 0x003FFFFF - Total 2Mbytes

	// CS3
	MEMCTRL->bank[mcpNvRamBankStart+2].BankNLoBorder = 0x00400000;
	MEMCTRL->bank[mcpNvRamBankStart+2].BankNHiBorder = 0x00500000; // end address 0x005FFFFF - Total 2Mbytes



	for (int memBank=mcpNvRamBankStart; memBank < mcpNvRamBankStart+mcpNvRamBankNum; ++memBank)
		{
		    //MSB | ConsecutiveRead_bit31 | CS_offDelay[7..0] | BurstReadDelay[7..0] |LSB
			MEMCTRL->bank[memBank].BankNburstDelayConfig = 0x0003|(1<<31); //burst delay - 4 (or burst delay -5 if CS_offDelay != 0)

			//MSB| ACK_timeOut[7..0] | WE_Delay[7..0] | OE_Delay[7..0] | CS_Delay[7..0] | LSB
			MEMCTRL->bank[memBank].BankNDelay = 0xFF000000;

			//MSB| CSwrite_offDelay[7..0] | WE_offDelay[7..0] | OE_offDelay[7..0] | CSread_offDelay[7..0] | LSB
			MEMCTRL->bank[memBank].BankNoffDelay = 0x03030303; // burst last cycle

			//MSB | WriteWaitState[7..0] | ReadWaitState[7..0] |LSB
			MEMCTRL->bank[memBank].BankNdataWaitState = 0x0303; //1st cycle delay - 3

			MEMCTRL->bank[memBank].BankNConfig = 0x1900;//0x101;//0x5900;
		}
}
