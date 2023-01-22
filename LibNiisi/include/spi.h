/*------------------------------------------------------*/
/*------------------SPI library------------------------*/
/*------------------------------------------------------*/

#ifndef _SPI_H_
#define _SPI_H_

#include "chip.h"
#include "devdescr.h"

/*----------------- Prototypes ---------------------------------*/
void spi_mode(int spi_n, 
				int device, 
				int divider, 
				int master_slave);
unsigned int spi_check_in(int spi_n);
unsigned int spi_check_out(int spi_n);
unsigned int rx_num(int spi_n);
unsigned int tx_num(int spi_n);
int spi_write(int spi_n, void *buf, int nbyte);
int spi_read(int spi_n, void *buf, int nbyte);
void spi_close(int spi_n);

#endif /* _SPI_H_ */
