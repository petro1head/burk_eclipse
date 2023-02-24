
/*------------------------------------------------------*/
/*------------------MKIO library------------------------*/
/*------------------------------------------------------*/

#ifndef _MKIO_H_
#define _MKIO_H_

#include "chip.h"
#include "devdescr.h"

/*----------------- Prototypes -------------------------*/

MKIO_UNIT* mkio;
mkoRegisters* mko;
unsigned short * mem;
unsigned short memA[2048];
unsigned short memB[2048];


void mkio_set_mode(unsigned int mkio_n,
                   int mode,    // 0 - BC, 1 - RT
                   int rt_addr, // index of RT
                   int wait,    // timeout for ancwer word
                   int rp);     // permission to try again

MKIO_UNIT* mkio_get_reg_ptr(unsigned int mkio_n);

void mkio_program_bc(unsigned int mkio_n,
                     unsigned int rt_addr, // RT address 0-31
                     unsigned int subaddr, // subaddress 1-30 in RT if com==0
                     unsigned int chain,   // chain number
                     unsigned int dir,     // transfer direction: 0 - BC->RT, 1 - BC<-RT
                     unsigned int command, // 0 - transmit/receive, 1 - control command
                     unsigned short* data, // ptr to data array: for transmit or transmit/receive for control command
                     unsigned int message_len); // if com==0 length of message in 16-bit words

void mkio_start_bc(unsigned int mkio_n, int chain);

void mkio_reset_bcrt(unsigned int mkio_n);

void mkio_init_rt(unsigned int mkio_n);

void mkio_create_sa(unsigned int mkio_n,
                    unsigned int subaddr,
                    unsigned short sapar_rx,
                    unsigned short sapar_tx);

void mkio_set_sa(unsigned int mkio_n,
                 unsigned int subaddr,
                 unsigned short sapar_rx,
                 unsigned short sapar_tx,
                 unsigned short rx_buf,
                 unsigned short tx_buf);

void mkio_start_rt(unsigned int mkio_n);

void mkio_set_rt_busy(unsigned int mkio_n, int flag);

void mkio_set_sa_busy(unsigned int mkio_n,
                      unsigned int subaddr,
                      int flag);

unsigned short * mkio_rt_get_last_done_cmd(unsigned int mkio_n);

void mkio_rt_put_data(unsigned int mkio_n,
                      unsigned int subaddr,
                      unsigned int buf_n,
                      unsigned int out_length,   // length of transmitting message in 16-bit words
                      unsigned short* out_data); // ptr to data array for transmit or data for control command

int  mkio_rt_get_data(unsigned int mkio_n,
                      unsigned int subaddr,
                      unsigned int buf_n,
                      unsigned int inp_length,   // length of received message in 16-bit words (filled by event handler)
                      unsigned short* inp_data); // ptr to data array where to put received data or data from control command

unsigned int mkio_get_mem_base(unsigned int mkio_n);

unsigned short mkio_read_mem(unsigned int mkio_n, unsigned int addr);

void mkio_write_mem(unsigned int mkio_n, unsigned int addr, unsigned short value);

void wait_free();

void mkio_write_program(unsigned int mkio_n, unsigned short * ProgrAddr, int ProgrLen);

int mkio_chain_done(unsigned int mkio_n, int chain);

int mkio_get_current_chain(unsigned int mkio_n);

int mkio_check_int(unsigned int mkio_n);

void mkio_clear_int(unsigned int mkio_n);

void mkio_fill_mem(unsigned int mkio_n);

void mkio_copy_mem(unsigned int mkio_n);

int mkioInit(unsigned int mkio_n, unsigned int bcOrRtAddress);

#endif /* _MKIO_H_ */
