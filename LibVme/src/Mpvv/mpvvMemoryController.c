/*
 * mpvvMemoryController.c
 *
 *  Created on: 17 мая 2021 г.
 *      Author: Dmitry
 */
#include "mpvvMemoryController.h"

const int mpvvBootRomBank = 0;
const int mpvvNvRamBankStart = 1; // PZU MRAM starting CS
const int mpvvNvRamBankNum = 3; // PZU MRAM number of CS



int mpvvInitMemoryController()
{
	volatile MEMCTRL_UNIT *MEMCTRL = (MEMCTRL_UNIT *)MEMCTRL_BASE;

	// BOOT ROM AREA MARGINS SETUP
	MEMCTRL->bank[mpvvBootRomBank].BankNLoBorder = 0x1FC00000;
	MEMCTRL->bank[mpvvBootRomBank].BankNHiBorder = 0x1FE00000;

	// NV RAM AREA MARGINS SETUP - total 6Mbytes
	// CS1
	MEMCTRL->bank[mpvvNvRamBankStart].BankNLoBorder = 0;
	MEMCTRL->bank[mpvvNvRamBankStart].BankNHiBorder = 0x00100000; // end address 0x001FFFFF - Total 2Mbytes

	// CS2
	MEMCTRL->bank[mpvvNvRamBankStart+1].BankNLoBorder = 0x00200000 ;
	MEMCTRL->bank[mpvvNvRamBankStart+1].BankNHiBorder = 0x00300000; // end address 0x003FFFFF - Total 2Mbytes

	// CS3
	MEMCTRL->bank[mpvvNvRamBankStart+2].BankNLoBorder = 0x00400000;
	MEMCTRL->bank[mpvvNvRamBankStart+2].BankNHiBorder = 0x00500000; // end address 0x005FFFFF - Total 2Mbytes



	for (int memBank=mpvvNvRamBankStart; memBank < mpvvNvRamBankStart+mpvvNvRamBankNum; ++memBank)
		{
		    //MSB | ConsecutiveRead_bit31 | CS_offDelay[7..0] | BurstReadDelay[7..0] |LSB
			MEMCTRL->bank[memBank].BankNburstDelayConfig = 0x0202|(0<<31); //burst delay - 4 (or burst delay -5 if CS_offDelay != 0)

			//MSB| ACK_timeOut[7..0] | WE_Delay[7..0] | OE_Delay[7..0] | CS_Delay[7..0] | LSB
			MEMCTRL->bank[memBank].BankNDelay = 0xFF020200;

			//MSB| CSwrite_offDelay[7..0] | WE_offDelay[7..0] | OE_offDelay[7..0] | CSread_offDelay[7..0] | LSB
			MEMCTRL->bank[memBank].BankNoffDelay = 0x06050909; // burst last cycle

			//MSB | WriteWaitState[7..0] | ReadWaitState[7..0] |LSB
			MEMCTRL->bank[memBank].BankNdataWaitState = 0x0608; //1st cycle delay - 3

			MEMCTRL->bank[memBank].BankNConfig = 0x1900;//0x101;//0x5900;
		}
}

