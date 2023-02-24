#ifndef _BOARD_H_
#define _BOARD_H_

#define MEM_CAP		0x04000000	/* 64 MB */

#define CPU_FREQUENCY_NOMINAL  100	/* MHz */

#define	SPI_BASE	0xBFF00000
#define	UART0_BASE	0xBFF00100
//#define	UART0_BASE	0xBFF70000
#define	INTC_BASE	0xBFF00200
#define	TIMER0_BASE	0xBFF00300
#define	TIMER_A_BASE	TIMER0_BASE
#define	TIMER1_BASE	0xBFF00400
#define	TIMER_B_BASE	TIMER1_BASE
#define	TIMER2_BASE	0xBFF00500
#define	TIMER_C_BASE	TIMER2_BASE
#define ETH_BASE        0xbff00600
#define	GPIO_BASE	0xBFF00700
#define	UART1_BASE	0xBFF00800
#define	I2C_BASE	0xBFF00900
#define	NANDCTRL_BASE	0xBFF00A00
#define	SYSUNIT_BASE	0xBFF00B00
#define	MANCH_A_BASE	0xBFF01000
#define	MANCH_B_BASE	0xBFF01100
#define	MANCHMEM_B_BASE	0xBFF02000
#define	SPW_BASE	0xBFF04000
#define	MANCHMEM_A_BASE	0xBFF06000
#define	MEMCTRL_BASE	0xBFF08000


//------- rs232 definitions --------//
#define	DEVICE_FREQUENCY	24000000
#define	BAUDRATE		9600

#define	RxD	0x03
#define	TxD	0x03
#define	DivLSB	0x03
#define	DivMSB	0x07
#define	IIR	0x0b
#define	FCR	0x0b
#define	EFR	0x0b
#define	LCR	0x0F
#define	IER	0x07
#define	MCR	0x13
#define	LSR	0x17
#define	MSR	0x1B
#define	SCR	0x1f
#define	CTL	0x23

#define	RxDw	0x00
#define	TxDw	0x00
#define	DivLSBw	0x00
#define	DivMSBw	0x04
#define	IIRw	0x08
#define	FCRw	0x08
#define	EFRw	0x08
#define	LCRw	0x0c
#define	IERw	0x04
#define	MCRw	0x10
#define	LSRw	0x14
#define	MSRw	0x18
#define	SCRw	0x1c
#define	CTLw	0x20
//----------------------------------//

//############################################################
//------------ SPI controller definitions ------------//
#define SPI_MODE        (SPI_BASE+0x0000)
#define SPI_TIME        (SPI_BASE+0x0004)
#define SPI_CTL         (SPI_BASE+0x0008)
#define SPI_STAT        (SPI_BASE+0x000c)
#define SPI_RXF         (SPI_BASE+0x0010)
#define SPI_TXF         (SPI_BASE+0x0014)
#define SPI_CFG         (SPI_BASE+0x0018)
#define SPI_FSTAT       (SPI_BASE+0x001c)
#define SPI_IEN         (SPI_BASE+0x0020)
#define SPI_IS          (SPI_BASE+0x0024)

//############################################################
//------------ Interrupt controller definitions ------------//
#define INTC_SYS_INTMAP0  (INTC_BASE+0x00)
#define INTC_SYS_INTMAP1  (INTC_BASE+0x04)
#define INTC_SYS_INTMAP2  (INTC_BASE+0x08)
#define INTC_SYS_INTMAP3  (INTC_BASE+0x0c)
#define INTC_SYS_INTMAP4  (INTC_BASE+0x10)
#define INTC_SYS_INTMAP5  (INTC_BASE+0x14)
#define INTC_SYS_INTMAP6  (INTC_BASE+0x18)
#define INTC_SYS_INTMAP7  (INTC_BASE+0x1c)
#define INTC_SYS_INTMAP8  (INTC_BASE+0x20)
#define INTC_SYS_INTMAP9  (INTC_BASE+0x24)
#define INTC_SYS_INTMAP10 (INTC_BASE+0x28)
#define INTC_SYS_INTMAP11 (INTC_BASE+0x2c)
#define INTC_SYS_INTMAP12 (INTC_BASE+0x30)
#define INTC_SYS_INTMAP13 (INTC_BASE+0x34)
#define INTC_SYS_INTMAP14 (INTC_BASE+0x38)
#define INTC_SYS_INTMAP15 (INTC_BASE+0x3c)
#define INTC_SYS_INTMAP16 (INTC_BASE+0x40)
#define INTC_SYS_INTMAP17 (INTC_BASE+0x44)
#define INTC_SYS_INTMAP18 (INTC_BASE+0x48)
#define INTC_SYS_INTMAP19 (INTC_BASE+0x4c)
#define INTC_SYS_INTMAP20 (INTC_BASE+0x50)
#define INTC_SYS_INTMAP21 (INTC_BASE+0x54)
#define INTC_SYS_INTMAP22 (INTC_BASE+0x58)
#define INTC_SYS_INTMAP23 (INTC_BASE+0x5c)
#define INTC_SYS_INTMAP24 (INTC_BASE+0x60)
#define INTC_SYS_INTMAP25 (INTC_BASE+0x64)
#define INTC_SYS_INTMAP26 (INTC_BASE+0x68)
#define INTC_SYS_INTMAP27 (INTC_BASE+0x6c)
#define INTC_SYS_INTMAP28 (INTC_BASE+0x70)
#define INTC_SYS_INTMAP29 (INTC_BASE+0x74)
#define INTC_SYS_INTMAP30 (INTC_BASE+0x78)
#define INTC_SYS_INTMAP31 (INTC_BASE+0x7c)
#define INTC_SYS_INTREQ   (INTC_BASE+0x80)
#define INTC_SYS_INTENABLE (INTC_BASE+0x90)

/*==============================================================
====== Int request line numbers for K128RIO      ===============
==============================================================*/
#define	IRQ_SPI		0
#define IRQ_UART_A	1
#define IRQ_ETHERNET	2
#define IRQ_UART_B	3
#define IRQ_I2C		4
#define IRQ_TIMER_A	5
#define IRQ_MKIO_A	6
#define IRQ_MEMCTRL	7
#define IRQ_MKIO_B	8
#define IRQ_TIMER_B	9
#define IRQ_TIMER_C	10
/* #define IRQ_GPIO	13 */
#define IRQ_SPW		14
#define IRQ_NMI		15

#define IRQ_GPIO	16

//############################################################
//------------ GPIO definitions ---------------------------//
#define GPIO_IDREG           (GPIO_BASE+0x0000)
#define GPIO_IN              (GPIO_BASE+0x0004)
#define GPIO_OUT             (GPIO_BASE+0x0008)
#define GPIO_SET             (GPIO_BASE+0x000c)
#define GPIO_CLR             (GPIO_BASE+0x0010)
#define GPIO_INVERT          (GPIO_BASE+0x0014)
#define GPIO_DIR             (GPIO_BASE+0x0018)
#define GPIO_INTFLAG         (GPIO_BASE+0x001c)
#define GPIO_AFIO            (GPIO_BASE+0x0020)
#define GPIO_AFMODE          (GPIO_BASE+0x0024)
#define GPIO_MODE0           (GPIO_BASE+0x002c)

#define GPIO_LINES 32

//############################################################
//------------ I2C definitions ---------------------------//
#define I2C_ADDR        (I2C_BASE+0x0000)
#define I2C_DATA        (I2C_BASE+0x0004)
#define I2C_CNTR        (I2C_BASE+0x0008)
#define I2C_STAT        (I2C_BASE+0x000c)
#define I2C_CCR         (I2C_BASE+0x000c)
#define I2C_XADDR       (I2C_BASE+0x0010)
#define I2C_SRST        (I2C_BASE+0x001c)


//############################################################
//------------ MEM Controller definitions ---------------------------//

#define	MEM_0_BASE	(MEMCTRL_BASE+0*0x100)
#define	MEM_1_BASE	(MEMCTRL_BASE+1*0x100)
#define	MEM_2_BASE	(MEMCTRL_BASE+2*0x100)
#define	MEM_3_BASE	(MEMCTRL_BASE+3*0x100)
#define	MEM_4_BASE	(MEMCTRL_BASE+4*0x100)
#define	MEM_5_BASE	(MEMCTRL_BASE+5*0x100)
#define	MEM_6_BASE	(MEMCTRL_BASE+6*0x100)
#define	MEM_7_BASE	(MEMCTRL_BASE+7*0x100)

#define	oLO_BORDER	0x00
#define	oHI_BORDER	0x04
#define	oCONFIG		0x08
#define	oWAIT_STATES	0x0C
#define	oDELAY		0x10
#define	oOFF_DELAY	0x14
#define	oBURST_CONFIG	0x18
#define	oERR_ADDR	0x1C

#define ECC_RW_ON             (0x1<<12)
#define ECC_ON                (0x1<<11)
#define ECC_INTERLEAVING_ON   (0x1<<9)
#define ACK_EN		      (0x1<<2)

#endif /* _BOARD_H_ */
