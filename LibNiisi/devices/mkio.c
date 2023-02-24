
/*------------------------------------------------------*/
/*------------------MKIO library -----------------------*/
/*------------------------------------------------------*/

#ifndef _MKIO_C_
#define _MKIO_C_

#include "mkio.h"

#define PROT_INITIAL_ADDRESS 0x00a0

extern MKIO_UNIT* mkio;
extern unsigned short * mem;

/*----------------------------------------------------------------------------*/

void mkio_set_mode(unsigned int mkio_n, int mode, int rt_addr, int wait, int rp) {
	unsigned int w, n, d;
	if (mkio_n == 0)
		mkio = (MKIO_UNIT *)MANCH_A_BASE;
	else
		mkio = (MKIO_UNIT *)MANCH_B_BASE;

	mkio->d4 = 0;
	if (mode) { // RT
		d = rt_addr & 0x1f; // parity bit
		n = 1;
		while (d != 0) {
			if (d & 1) n++;
			d = d >> 1;
		}
		wait_free();
		mkio->Config = ((n & 1) << 10) | (1 << 7) | ((rt_addr  & 0x1f) << 2) | 1; // PAR, IAC=1, RTdev, RT
		wait_free();
		mkio->Control = 4; // Reset interrupt
		wait_free();
		mkio->IntEn = 1;   // Enable Interrupt on End of Message
	} else {	// BC
		w = (wait == 160) ? 3 : (wait == 80) ? 2 : (wait == 40) ? 0 : 1;
		wait_free();
		mkio->Config = (w << 8) | 0x80 | (rp << 2); // TR=wait, IAC=1, ARE=1
//		mkio->Config = (w << 8) | 0x00 | (rp << 2); // TR=wait, IAC=1, ARE=1 //changed signal polarity when i was debugging for new VME pribor
		wait_free();
		mkio->Control = 4; // Chain 0, Reset interrupt
		wait_free();
		mkio->IntEn = 0;   // Enable Interrupt on End of Chain
	}
} // mkio_set_mode


/*----------------------------------------------------------------------------*/
/*-----------------------------BUS CONTROLLER---------------------------------*/
/*----------------------------------------------------------------------------*/

void mkio_program_bc(unsigned int mkio_n,
                     unsigned int rt_addr, // RT address 0-31
                     unsigned int subaddr, // subaddress 1-30 in RT if com==0
                     unsigned int chain,   // chain number
                     unsigned int dir,     // transfer direction: 0 - BC->RT, 1 - BC<-RT
                     unsigned int command, // 0 - transmit/receive, 1 - control command
                     unsigned short* data, // ptr to data array: for transmit or transmit/receive for control command
                     unsigned int message_len) { // if com==0 length of message in 16-bit words

	unsigned int i;

	if (mkio_n == 0)
		mem = (unsigned short *)MANCHMEM_A_BASE;
	else
		mem = (unsigned short *)MANCHMEM_B_BASE;

	if (command) { // control command
		/*
		 *
		 */
	} else { // transmit/receive
		if (chain == 0) {   // chain A
			mem[0] = 8;     // chain A pointer current
			mem[1] = 1;     // chain A message counter current
			mem[2] = 8;     // chain A pointer initial
			mem[3] = 1;     // chain A message counter initial
			mem[8] = 0x10;  // command 1, message address
			mem[9] = 0;     // clear status word
			mem[0x10] = 0;  // 0 - use main channel (8 - use reserve channel)
			mem[0x11] = ((rt_addr << 11) |
			             (dir << 10)     |
			             (subaddr << 5)  |
			             (message_len & 0x1f));
			if (dir) {       // read
				mem[0x12]=0; // ack word
			} else {
				for (i = 0; i < message_len; i++) { // write data to BC buffer
					mem[0x12 + i] = data[i];
				}
				mem[0x12 + message_len] = 0; // clear acknoledge word
			}
		} else {// chain B
			mem[4] = 0x80;  // chain B pointer current
			mem[5] = 1;     // chain B message counter current
			mem[6] = 0x80;  // chain B pointer initial
			mem[7] = 1;     // chain B message counter initial
			mem[0x80] = 0x90; // command 1, message address
			mem[0x81] = 0;    // clear status word
			mem[0x90] = 0;    // use main channel
			mem[0x91] = ((rt_addr << 11) |
			             (dir << 10)     |
			             (rt_addr<<5)    |
			             (message_len & 0x1f));
			if (dir) {
				mem[0x92] = 0; //read ack word
			} else {
				for (i = 0; i < message_len; i++) { // write data to BC buffer
					mem[0x92 + i] = data[i];
				}
				mem[92 + message_len] = 0; // clear acknoledge word
			}
		} // if chain
	} // if command
} // mkio_program_bc


/*----------------------------------------------------------------------------*/

void mkio_start_bc(unsigned int mkio_n, int chain) {
	if (mkio_n == 0)
		mkio = (MKIO_UNIT *)MANCH_A_BASE;
	else
		mkio = (MKIO_UNIT *)MANCH_B_BASE;

	wait_free();
	mkio->Control = 4; // IR - reset interrupt
	wait_free();
	mkio->Control = ((chain << 5) | 0 | 2); // chain, SF - stop after chain done, Start
	wait_free();
} // mkio_start_bc

/*----------------------------------------------------------------------------*/

void mkio_reset_bcrt(unsigned int mkio_n) {
	if (mkio_n == 0)
		mkio = (MKIO_UNIT *)MANCH_A_BASE;
	else
		mkio = (MKIO_UNIT *)MANCH_B_BASE;


	mkio->Control = 1;
	wait_free();
	mkio->Control = 0;
	wait_free();
} // MKIOResetBC

/*----------------------------------------------------------------------------*/
/*-----------------------------REMOTE TERMINAL--------------------------------*/
/*----------------------------------------------------------------------------*/

void mkio_init_rt(unsigned int mkio_n) {
	int i;

	if (mkio_n == 0)
		mem = (unsigned short *)MANCHMEM_A_BASE;
	else
		mem = (unsigned short *)MANCHMEM_B_BASE;

	mem[0] = mem[2] = PROT_INITIAL_ADDRESS; // protocol buffer current and initial pointers
	mem[1] = mem[3] = 31; // protocol buffer current and initial counters

	for (i = 0; i < 32 * 4; i++) {
		mem[0xa0 + i] = 0; // FOR MODELLING
	}

	for (i = 1; i <= 30; i++) {
		mem[4 * i]     = SA_BUF_OFF; // all subaddr (1-30) are disabled
		mem[4 * i + 1] = 0;
		mem[4 * i + 2] = 0;
		mem[4 * i + 3] = 0;
	}

	// Control commands table
	// 0x7c - acceptable control commands 0-8 (remove unacceptable)
	mem[0x7c] = ((1 << CMD_CHANGECONTROL) |
	             (1 << CMD_SYNCRO)        |
	             (1 << CMD_GIVEOS)        |
	             (1 << CMD_BEGINCONTROL)  |
	             (1 << CMD_LOCK)          |
	             (1 << CMD_UNLOCK)        |
	             (1 << CMD_LOCKFAULT)     |
	             (1 << CMD_UNLOCKFAULT)   |
	             (1 << CMD_RESET));
	mem[0x7d] = mem[0x7c]; // interrupts enable for all acceptable commands
	// 0x7c - acceptable control commands 0x10-0x15 (remove unacceptable)
	mem[0x7e] = ((1 << (CMD_GIVEVECWORD - 16)) |
	             (1 << (CMD_SYNCROWORD - 16))  |
	             (1 << (CMD_GIVELASTCOM - 16)) |
	             (1 << (CMD_GIVEVSK - 16))     |
	             (1 << (CMD_LOCKTR - 16))      |
	             (1 << (CMD_UNLOCKTR - 16)));
	mem[0x7f] = mem[0x7e]; // interrupts enable for all acceptable commands
	mem[0x92] = 0x230;     // pointer to free mem for buffers

} // mkio_init_rt


/*----------------------------------------------------------------------------*/
void mkio_set_sa(unsigned int mkio_n,     // alternate function to mkio_create_sa
                 unsigned int subaddr,
                 unsigned short sapar_rx, // subaddr parametres (for recieving)
                 unsigned short sapar_tx, // subaddr parametres (for sending)
                 unsigned short rx_buf,   // buffer adresses
                 unsigned short tx_buf) {

	if (mkio_n == 0)
		mem = (unsigned short *)MANCHMEM_A_BASE;
	else
		mem = (unsigned short *)MANCHMEM_B_BASE;

	mem[subaddr * 4]     = sapar_rx;
	mem[subaddr * 4 + 1] = rx_buf;
	mem[subaddr * 4 + 2] = sapar_tx;
	mem[subaddr * 4 + 3] = tx_buf;
} // mkio_set_sa


/*----------------------------------------------------------------------------*/

static unsigned int bl_in_words(unsigned int sapar) {
	unsigned int bl, bm;

	bl = sapar & 0x1f; if (bl == 0) bl = 32;	// block length
	bm = (sapar >> 10) & 3;	// buffering mode

	if(bm == 3)       return 0;         // not needbuffer - subaddr disabled
	else if (bm == 2) return (bl * 32); // buf len = bl*32 words - ring buffer
	else if (bm == 1) return (bl * 2);  // buf len = bl*2 words - double buffer
	return(bl); // single buffer
} // bl_in_words
/*----------------------------------------------------------------------------*/

void mkio_create_sa(unsigned int mkio_n,
                    unsigned int subaddr,
                    unsigned short sapar_rx,
                    unsigned short sapar_tx) {
	if (mkio_n == 0)
		mem = (unsigned short *)MANCHMEM_A_BASE;
	else
		mem = (unsigned short *)MANCHMEM_B_BASE;

	mem[subaddr * 4]     = sapar_rx;
	mem[subaddr * 4 + 1] = mem[0x92] & 0xffff;
	mem[0x92] = (mem[0x92] & 0xffff) + bl_in_words(sapar_rx); // shift free mem pointer
	mem[subaddr * 4 + 2] = sapar_tx;
	mem[subaddr * 4 + 3] = mem[0x92] & 0xffff;
	mem[0x92] = (mem[0x92] & 0xffff) + bl_in_words(sapar_tx); // shift free mem pointer
} // mkio_set_sa

/*----------------------------------------------------------------------------*/
void mkio_set_rt_busy(unsigned int mkio_n, int flag) { // 0 or 1
	if (mkio_n == 0)
		mkio = (MKIO_UNIT *)MANCH_A_BASE;
	else
		mkio = (MKIO_UNIT *)MANCH_B_BASE;

	wait_free();
	mkio->Control = (mkio->Control & ~(1 << 6)) | (flag << 6);
} // MKIOSetRTBusy

/*----------------------------------------------------------------------------*/
void mkio_set_sa_busy(unsigned int mkio_n, unsigned int subaddr, int flag) { // 0 or 1
	if (mkio_n == 0)
		mem = (unsigned short *)MANCHMEM_A_BASE;
	else
		mem = (unsigned short *)MANCHMEM_B_BASE;

	mem[subaddr * 4] = (mem[subaddr * 4] & ~(1 << 14)) | (flag << 14);
} // MKIOSetSaBusy

/*----------------------------------------------------------------------------*/
void mkio_start_rt(unsigned int mkio_n) {
	if (mkio_n == 0)
		mkio = (MKIO_UNIT *)MANCH_A_BASE;
	else
		mkio = (MKIO_UNIT *)MANCH_B_BASE;

	wait_free();
	mkio->Control |= 2;
} // mkio_start_rt

/*----------------------------------------------------------------------------*/
unsigned short * mkio_rt_get_last_done_cmd(unsigned int mkio_n) {
	int prb;
	int i;

	if (mkio_n == 0)
		mem = (unsigned short *)MANCHMEM_A_BASE;
	else
		mem = (unsigned short *)MANCHMEM_B_BASE;

	prb = mem[0];
	for (i = 0; i < 32; i++) {
		if (mem[prb] & 0x8000) { // command done
			return &mem[prb];
		} else {
			prb--;
			if (prb < PROT_INITIAL_ADDRESS) {
				prb = PROT_INITIAL_ADDRESS + 31;
			}
		}
	} // for

	return 0;
} // mkio_rt_get_last_done_cmd


/*----------------------------------------------------------------------------*/
void mkio_rt_put_data(unsigned int mkio_n,
                      unsigned int subaddr,
                      unsigned int buf_n,
                      unsigned int out_length,     // length of transmitting message in 16-bit words
                      unsigned short * out_data) { // ptr to data array for transmit or data for control command
	int buf;
	unsigned int i, BL;

	if (mkio_n == 0)
		mem = (unsigned short *)MANCHMEM_A_BASE;
	else
		mem = (unsigned short *)MANCHMEM_B_BASE;


	buf = mem[subaddr * 4 + 3] & 0xffff;
	BL = mem[subaddr * 4 + 2] & 0x1f;
	if (BL == 0) BL = 32;
	if(buf_n) buf += BL;

	for (i = 0; i < out_length; i++) {
		mem[buf + i] = out_data[i];
	}
} // mkio_rt_put_data

/*----------------------------------------------------------------------------*/
//0 - there is new data in SA
//1 - no new data in SA
int mkio_rt_get_data(unsigned int mkio_n,
                      unsigned int subaddr,
                      unsigned int buf_n,
                      unsigned int inp_length, // length of received message in 16-bit words (filled by event handler)
                      unsigned short * inp_data) { // ptr to data array where to put received data or data from control command

	int buf;
	unsigned int i, BL;

	if (mkio_n == 0)
		mem = (unsigned short *)MANCHMEM_A_BASE;
	else
		mem = (unsigned short *)MANCHMEM_B_BASE;

	buf = mem[subaddr * 4 + 1] & 0xffff;
	BL = mem[subaddr * 4]  & 0x1f;
	if (BL == 0) BL = 32;
	if(buf_n) buf += BL;

	if((mem[subaddr * 4])&(0x1000)){

		for (i = 0; i < inp_length; i++) {
			inp_data[i] = mem[buf + i];
		}
		mem[subaddr * 4] = mem[subaddr * 4] &0xefff;
		return 0;
	} else {
		return 1;
		for (i = 0; i < inp_length; i++) {
			inp_data[i] = 0;
		}
	}
} // mkio_rt_get_data?

/*----------------------------------------------------------------------------*/
void wait_free() {
	while(mkio->Busy != 0);
} // wait_free

/*----------------------------------------------------------------------------*/
MKIO_UNIT * mkio_get_reg_ptr(unsigned int mkio_n) {
	if (mkio_n == 0)
		return (MKIO_UNIT *)MANCH_A_BASE;
	else
		return (MKIO_UNIT *)MANCH_B_BASE;
} // mkio_get_reg_ptr

/*----------------------------------------------------------------------------*/

unsigned int mkio_get_mem_base(unsigned int mkio_n) {
	if (mkio_n == 0)
		return MANCHMEM_A_BASE;
	else
		return MANCHMEM_B_BASE;
} // mkio_get_mem_base

/*----------------------------------------------------------------------------*/
unsigned short mkio_read_mem(unsigned int mkio_n, unsigned int addr) {
	unsigned int * mem;

	if (mkio_n == 0)
		mem = (unsigned int *)MANCHMEM_A_BASE;
	else
		mem = (unsigned int *)MANCHMEM_B_BASE;

	return(mem[addr]);
} // mkio_read_mem

/*----------------------------------------------------------------------------*/

void mkio_write_mem(unsigned int mkio_n, unsigned int addr, unsigned short value) {
	unsigned int * mem;
	if (mkio_n == 0)
		mem = (unsigned int *)MANCHMEM_A_BASE;
	else
		mem = (unsigned int *)MANCHMEM_B_BASE;

	mem[addr] = value;
} // mkio_write_mem

/*--------------------------------- NEW FUCTIONS ---------------------------------*/

void mkio_write_program(unsigned int mkio_n, unsigned short * ProgrAddr, int ProgrLen) {
	int i;
	if (mkio_n == 0)
		mem = (unsigned short *)MANCHMEM_A_BASE;
	else
		mem = (unsigned short *)MANCHMEM_B_BASE;

	for (i = 0; i < ProgrLen * 2; i+= 2) {
		mem[ProgrAddr[i]] = ProgrAddr[i + 1];
	}
} // mkio_write_program

/*--------------------------------------------------------------------------------*/

int mkio_chain_done(unsigned int mkio_n, int chain) {
	//unsigned int status_word;

	if (mkio_n == 0)
		mkio = (MKIO_UNIT *)MANCH_A_BASE;
	else
		mkio = (MKIO_UNIT *)MANCH_B_BASE;

	if (mkio->Config & 3)                       return -1; // not BC
	if ((mkio->Control & 0x20) != ((unsigned) chain << 5)) return 1;  // current chain != chain

	// Current chain done?
	//status_word = mem[mem[0 + 4 * chain] + 1]; // old checking variant

	if (((mkio->Control & 0x20) == ((unsigned) chain << 5)) & // current chain == chain
	    ((mkio->Status & 2) == 2))                 // this chain now run
		return 0;
	return 1;
} // mkio_chain_done

/*--------------------------------------------------------------------------------*/

int mkio_get_current_chain(unsigned int mkio_n) {
	if (mkio_n == 0)
		mkio = (MKIO_UNIT *)MANCH_A_BASE;
	else
		mkio = (MKIO_UNIT *)MANCH_B_BASE;
		
	if(mkio->Control & 0x20) return 1;
	return 0;
}

/*--------------------------------------------------------------------------------*/

int mkio_check_int(unsigned int mkio_n) {
	if (mkio_n == 0)
		return ((((MKIO_UNIT *)MANCH_A_BASE)->Int & 0x8000) >> 0xf);
	else   
		return ((((MKIO_UNIT *)MANCH_B_BASE)->Int & 0x8000) >> 0xf);
}  // MKIOClearInt

/*--------------------------------------------------------------------------------*/

void mkio_clear_int(unsigned int mkio_n) {
	if (mkio_n == 0)
		mkio = (MKIO_UNIT *)MANCH_A_BASE;
	else
		mkio = (MKIO_UNIT *)MANCH_B_BASE;

	wait_free();
	mkio->Int = 0xffff;
}  // MKIOClearInt

/*--------------------------------------------------------------------------------*/

void mkio_fill_mem(unsigned int mkio_n) {
	int i;

	if (mkio_n == 0)
		mem = (unsigned short *)MANCHMEM_A_BASE;
	else
		mem = (unsigned short *)MANCHMEM_B_BASE;

	for (i = 0; i < 2048; i++) mem[i] = 0;
} // mkio_fill_mem



void mkio_copy_mem(unsigned int mkio_n) {
	int i;

	if (mkio_n == 0){
		mem = (unsigned short *)MANCHMEM_A_BASE;
		for (i = 0; i < 2048; i++) memA[i] = mem[i];
	}else{
		mem = (unsigned short *)MANCHMEM_B_BASE;
		for (i = 0; i < 2048; i++) memB[i] = mem[i];
	}
} // mkio_fill_mem







//mkioInit inits the mkio controller
//channel - 0 or 1
//bcOrRtAddress - 0 is BC, 1-30 is RT
//
int mkioInit(unsigned int mkio_n, unsigned int bcOrRtAddress)
{
	unsigned int addressParity;
	unsigned int w, n, d;
	if (mkio_n == 0){
		mem = (unsigned short *)MANCHMEM_A_BASE;
		mko = (mkoRegisters*)MANCH_A_BASE;
	}else{
		mem = (unsigned short *)MANCHMEM_B_BASE;
		mko = (mkoRegisters*)MANCH_B_BASE;
	}

	mkio->d4 = 0;

	if (bcOrRtAddress == 0) { // BC

		mko->Config.raw											= 0;
		mko->Config.bitBc.mode									= 0;
		mko->Config.bitBc.frameRepeatEnable						= 0;
		mko->Config.bitBc.frameInterruptOnEndEnable				= 0;
		mko->Config.bitBc.frameInterruptOnMessageEnable			= 0;
		mko->Config.bitBc.frameInterruptErrorOnEndEnable		= 0;
		mko->Config.bitBc.frameInterruptErrorOnMessageEnable	= 0;
		mko->Config.bitBc.signalLevelInactive					= 1;
		mko->Config.bitBc.waitTime								= 0;
		mko->Config.bitBc.reserve								= 0;
		mko->Config.bitBc.selfControl0							= 0;
		mko->Config.bitBc.selfControl1							= 0;
		wait_free();
		mko->Control.raw 							= 0;
		mko->Control.bitBc.reset					= 0;
		mko->Control.bitBc.start					= 0;
		mko->Control.bitBc.resetInterrupts		    = 1;
		mko->Control.bitBc.interruptOnFrameEnd	    = 0;
		mko->Control.bitBc.interruptOnMessageEnd	= 0;
		mko->Control.bitBc.frameCurrent				= 0;
		wait_free();
		mkio->IntEn = 0;   // Enable Interrupt on End of Chain
		wait_free();


//		w = 0;
//		mkio->Config = (w << 8) | 0x80 | (rp << 2); // TR=wait, IAC=1, ARE=1
//		mkio->Control = 4; // Chain 0, Reset interrupt


	} else if (bcOrRtAddress < 31){

		addressParity = 0x1&(1 + bcOrRtAddress&0x1 +
								(bcOrRtAddress>>1)&0x1 +
								(bcOrRtAddress>>2)&0x1 +
								(bcOrRtAddress>>3)&0x1 +
								(bcOrRtAddress>>4)&0x1);

		wait_free();
		mko->Config.bitRt.mode							= 1;
		mko->Config.bitRt.rtAddress						= bcOrRtAddress;
		mko->Config.bitRt.waitTime						= 0;
		mko->Config.bitRt.rtAddressParity               = addressParity;
		mko->Config.bitRt.selfControl0                  = 0;
		mko->Config.bitRt.selfControl1                  = 0;
		mko->Config.bitRt.broadcastReceiveEnable        = 0;
		mko->Config.bitRt.broadcastDataSeparateEnable   = 0;
		mko->Config.bitRt.bcOvertakeEnable              = 0;



		wait_free();
		mko->Control.raw 						=0;
		mko->Control.bitRt.reset				=0;
		mko->Control.bitRt.rtAddressLoad		=0;
		mko->Control.bitRt.resetInterrupts		=1;
		mko->Control.bitRt.flagRtError			=0;
		mko->Control.bitRt.flagSystemError		=0;
		mko->Control.bitRt.flagRepairRequest	=0;
		mko->Control.bitRt.flagBusy				=0;
		mko->Control.bitRt.disableChannelA		=0;
		mko->Control.bitRt.disableChannelB		=0;
		mko->Control.bitRt.flagRtErrorDisable	=0;

//		mkio->Control = 4; // Reset interrupt
		wait_free();
		mkio->IntEn = 0;   // Enable Interrupt on End of Message




	} else {
		return 1;//wrong bcOrRtAddress
	}

//	if (bcOrRtAddress) { // RT
//
//		d = rt_addr & 0x1f; // parity bit
//		n = 1;
//		while (d != 0) {
//			if (d & 1) n++;
//			d = d >> 1;
//		}
//		wait_free();
//		mkio->Config = ((n & 1) << 10) | (1 << 7) | ((rt_addr  & 0x1f) << 2) | 1; // PAR, IAC=1, RTdev, RT
//		wait_free();
//		mkio->Control = 4; // Reset interrupt
//		wait_free();
//		mkio->IntEn = 1;   // Enable Interrupt on End of Message
//	} else {	// BC
//	}


	return 0;
}






//void mkio_program_bc(unsigned int mkio_n,
//                     unsigned int rt_addr, // RT address 0-31
//                     unsigned int subaddr, // subaddress 1-30 in RT if com==0
//                     unsigned int chain,   // chain number
//                     unsigned int dir,     // transfer direction: 0 - BC->RT, 1 - BC<-RT
//                     unsigned int command, // 0 - transmit/receive, 1 - control command
//                     unsigned short* data, // ptr to data array: for transmit or transmit/receive for control command
//                     unsigned int message_len) { // if com==0 length of message in 16-bit words


int mkioAddMessage(unsigned int mkio_n,
					unsigned int rtAddress,
					unsigned int rtSubaddr,
					unsigned int chain,
					unsigned int direction,
					unsigned short* messageData,
					unsigned int messageLength
					)
{
	if (mkio_n == 0){
		mem = (unsigned short *)MANCHMEM_A_BASE;
		mko = (mkoRegisters*)MANCH_A_BASE;
	}else{
		mem = (unsigned short *)MANCHMEM_B_BASE;
		mko = (mkoRegisters*)MANCH_B_BASE;
	}

	return 0;
}






























#endif /* _MKIO_C_ */
