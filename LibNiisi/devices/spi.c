/*------------------------------------------------------*/
/*------------------SPI library -----------------------*/
/*------------------------------------------------------*/

#ifndef _SPI_C_
#define _SPI_C_

#include "spi.h"

static SPI_UNIT * const S[] = {
 (SPI_UNIT *)SPI0_BASE,
#ifdef VM056
 (SPI_UNIT *)SPI1_BASE,
#endif
 };

/*--------------------------------------------------------------
	spi_init
--------------------------------------------------------------*/
void spi_init(int spi_n, int device, int divider, int mode)
{
	/* end transmitting and receiving, clear FIFOs */
	S[spi_n]->CONTROL = CLEAR_TX_F | CLEAR_RX_F | END_TX | END_RX;

	/* set up mode, and active chip select signal */
	S[spi_n]->MODE = (device << 8) | SS_OUT_EN | CLOCK_PHASE 
								| CLOCK_POLARITY | mode;
	S[spi_n]->MODE |= SS_OUT_VAL;
	
	/* set clock divider*/
	S[spi_n]->TIME |= divider;
	
	/* disable interrupt */
	S[spi_n]->IEN = 0;

	/* clear interrupt status */
	S[spi_n]->IS = -1;
	
} // spi_init

/*--------------------------------------------------------------
	spi_check_in (Rx ready)
--------------------------------------------------------------*/
unsigned int spi_check_in(int spi_n)
{
	return ( S[spi_n]->IS & RX_DONE );
} // spi_check_in

/*--------------------------------------------------------------
	spi_check_out (Tx ready)
--------------------------------------------------------------*/
unsigned int spi_check_out(int spi_n)
{
	return ( S[spi_n]->IS & TX_DONE );
} // spi_check_out

/*--------------------------------------------------------------
	rx_num_entries (data can be read )
--------------------------------------------------------------*/
unsigned int rx_num(int spi_n)
{
	return ( (S[spi_n]->FSTAT & RX_F_NUM_ENTRIES) > 0);
} // rx_num_entries

/*--------------------------------------------------------------
	tx_num_entries (data can be write)
--------------------------------------------------------------*/
unsigned int tx_num(int spi_n)
{
	unsigned int result, tx_entr, tx_depth;
	result = tx_entr = tx_depth= 0;
	
	tx_entr = S[spi_n]->FSTAT & TX_F_NUM_ENTRIES;
	tx_depth = S[spi_n]->CFG & TX_F_DEPTH;
	
	return ( tx_entr < tx_depth );	
} // tx_num_entries

/*--------------------------------------------------------------
	spi_write
--------------------------------------------------------------*/
int spi_write(int spi_n, void *buf, int nbyte)
{
int cnt, txf_cnt, byte_cnt, tmp;
unsigned char *txf_buf = buf;
cnt = 0;
tmp = 0;

//while ( tx_num_entries(spi_n); //!= 1 );

while (cnt < nbyte)
   {
   txf_cnt = nbyte - cnt;

   if (txf_cnt > 4)
      txf_cnt = 4;
   byte_cnt = 4;

   do
      {
      tmp >>= 8;
      tmp |= *txf_buf++ << 24;
      cnt++;
      byte_cnt--;
      } while ( (cnt < nbyte) && byte_cnt);

   tmp >>= 8 * byte_cnt;

   S[spi_n]->IS |= TX_DONE;
   S[spi_n]->TXF = tmp;
   S[spi_n]->CONTROL = START_TX | txf_cnt;

   /* wait for a transfer */
	while ( !(spi_check_out(spi_n)) );
   }
return cnt;
} // spi_write

/*--------------------------------------------------------------
	spi_read 
--------------------------------------------------------------*/
int spi_read(int spi_n, void *buf, int nbyte)
{
volatile int cnt, txf_cnt, tmp;
volatile unsigned char *rxf_buf = buf;


for (cnt = 0; cnt < nbyte; )
   {
   txf_cnt = nbyte - cnt;
   S[spi_n]->IS |= RX_DONE;

   if (txf_cnt > 4)
      txf_cnt = 4;

   S[spi_n]->CONTROL = TX_DUMMY | START_TX | START_RX | txf_cnt;

   /* wait for received data */
   while( !(spi_check_in(spi_n)) );

   tmp = S[spi_n]->RXF;

   do
      {
      *rxf_buf++ = (unsigned char) (tmp & 0xff);
      tmp >>= 8;
      cnt++;
      } while (cnt < nbyte && cnt % 4); //
   }
return cnt;
} // spi_read

/*--------------------------------------------------------------
	spi_close 
--------------------------------------------------------------*/
void spi_close(int spi_n)
{
/* end transmitting and receiving */
S[spi_n]->CONTROL = END_TX | END_RX;

/* set up mode and inactive chip select signal */
S[spi_n]->MODE &= ~SS_OUT_VAL;
} // spi_close

/*---------------------------------------------------------*/

#endif /* _SPI_C_ */
