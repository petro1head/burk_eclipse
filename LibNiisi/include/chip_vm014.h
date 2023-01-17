#ifndef _CHIP_VM014_
#define _CHIP_VM014_

#define	SPI0_BASE	0xBFF00000
#define	UART0_BASE	0xBFF00100
#define	INTC_BASE	0xBFF00200
#define	TIMER0_BASE	0xBFF00300
#define	TIMER1_BASE	0xBFF00400
#define	TIMER2_BASE	0xBFF00500
#define	GPIO_BASE	0xBFF00700
#define	UART1_BASE	0xBFF00800
#define	SYSUNIT_BASE	0xBFF00B00
#define	MANCH_A_BASE	0xBFF01000
#define	MANCH_B_BASE	0xBFF01100
#define	MANCHMEM_B_BASE	0xBFF02000
#define	SPW_BASE	0xBFF04000
#define	MANCHMEM_A_BASE	0xBFF06000
#define	MEMCTRL_BASE	0xBFF08000

//############################################################
//------------------- Timer definitions --------------------//

#define TIMER0			0
#define TIMER1			1
#define TIMER_FIX_FREQUENCY 24000000

/*------For CTRL register -----*/
#define TIMER_CYCLE		0
#define TIMER_SINGLE	1
#define TIMER_STOP		0
#define TIMER_RUN		2
#define TIMER_INT_DIS	0
#define TIMER_INT_EN	4
#define TIMER_INT_REQ	8
#define TIMER_FIXFREQ	0
#define TIMER_SYSFREQ	0x10
#define TIMER_EXTFREQ	0x20
#define TIMER_PREDDIV	0x40
#define TIMER_RESET		0x100

/*------For channel setting -----*/
#define TIMER_CH0		0
#define TIMER_CH1		1
#define TIMER_CH2		2
#define TIMER_CH3		3

/*------For comparison channel setting -----*/
#define CCMP_INT_DIS		0	// interrupt is disable
#define CCMP_INT_EN			1	// interrupt enable
#define CCMP_CNT_RESET		2	// counter resrt
#define CCMP_CNT_STOP		4	// counter stop, reset activity bit( CTRL[1])
#define CCMP_SYS_RESET		8	// system reset signal generation enable

/*------For capture data channel setting -----*/
#define CCAP_EN				0	// capture data is enable
#define CCAP_FALL			0	// falling capture data 
#define CCAP_RISE			2	// rising capture data 
#define CCAP_INT_EN			4	// when data loading an interrupt is gererated


//############################################################
//-------------------- UART definitions --------------------//

#define UART0			0
#define UART1			1
#define	DEVICE_FREQUENCY	24000000
#define	BAUDRATE			57600

/*--------------------- PIO reg-------------------------*/ 

	//IIR					
#define ENABLE_FIFO			(1u<<6)
#define INT_SOURCE			(1u<<1)
#define NO_INT_PENDING		(1u<<0)

	//LCR
#define DLAB				(1u<<7)
#define PARITY_OFF			0
#define STOP_BIT_2			(1u<<2)
#define LENGTH_7	 		2

	//MCR
#define LOOP_BACK			(1u<<4)

	//LSR
#define TX_READY			(1u<<6)
#define RX_READY			(1u<<0)


//########################################################
//------------ MKIO controller definitions -------------//

#define CMD_CHANGECONTROL   0
#define CMD_SYNCRO          1
#define CMD_GIVEOS          2
#define CMD_BEGINCONTROL    3
#define CMD_LOCK            4
#define CMD_UNLOCK          5
#define CMD_LOCKFAULT       6
#define CMD_UNLOCKFAULT     7
#define CMD_RESET           8
#define CMD_GIVEVECWORD     0x10
#define CMD_SYNCROWORD      0x11
#define CMD_GIVELASTCOM     0x12
#define CMD_GIVEVSK         0x13
#define CMD_LOCKTR          0x14
#define CMD_UNLOCKTR        0x15

#define SA_IE          0x8000
#define SA_BUF_SINGLE  0x0000
#define SA_BUF_DBL     0x0400
#define SA_BUF_RING    0x0800
#define SA_BUF_OFF     0x0c00

#define MODE_BC  0
#define MODE_RT  1

// MKIO Error codes
#define NO_DONE_CMD_FOUND      1
#define ERROR_INDICATED_IN_MSW 2


//############################################################
//------------ SPI controller definitions ------------//

#define SPI0			0

	//SPI_MODE
#define DEV_ADDR				(1u << 8)
#define CFG_BIG_END				(1u << 7)
#define SS_OUT_VAL				(1u << 6)
#define SS_OUT_EN				(1u << 5)
#define MODE_FAULT_EN			(1u << 4)
#define LSB_FIRST				(1u << 3)
#define CLOCK_PHASE				(1u << 2)
#define CLOCK_POLARITY			(1u << 1)
#define MASTER					(1u << 0)
	//SPI_TIME					
#define SS_INACTIVE				(1u << 24)
#define SS_POSTAMBLE			(1u << 20)
#define SS_PREAMBLE				(1u << 16)
#define CLOCK_DIV				(1u << 0)
	//SPI_CFG
#define TX_F_DEPTH				(0xffu << 8)
#define RX_F_DEPTH				(0xffu << 0)
	//SRI_FSTAT
#define TX_F_INT_LEVEL			(1u << 24)
#define RX_F_INT_LEVEL			(1u << 16)
#define TX_F_NUM_ENTRIES		(1u << 8)
#define RX_F_NUM_ENTRIES		(1u << 0)
	//SPI_CTL
#define TX_DUMMY				(1u << 14)
#define CLEAR_TX_F				(1u << 13)
#define CLEAR_RX_F				(1u << 12)
#define END_TX					(1u << 11)
#define START_TX				(1u << 10)
#define END_RX					(1u << 9)
#define START_RX	 			(1u << 8)
#define TRF_COUNT				(1u << 0)
	//SPI_STAT
#define RX_FIFO_CNT				(1u << 24)
#define RX_PWB_CNT				(1u << 22)
#define LINK_BUSY				(1u << 17)
#define RX_ALL					(1u << 16)
#define RX_COUN					(1u << 8)
#define TX_COUNT				(1u << 0)
	//SPI_IS
#define TX_DONE					(1u << 9)
#define RX_DONE					(1u << 8)
#define TX_EMPTY				(1u << 7)
#define RX_F_FULL				(1u << 6)
#define TXF_ONE_WORD			(1u << 5)
#define RXF_ONE_WORD			(1u << 4)
#define MST_READ_OVER_FLOW		(1u << 3)
#define SLV_READ_OVER_FLOW		(1u << 2)
#define TX_ONE_BYTE_DONE		(1u << 1)
#define MODE_FAULT				(1u << 0)

//############################################################
//------------ GPIO definitions ---------------------------//

#define GPIO_LINES 32


/*==============================================================
====== Int request line numbers for K128RIO      ===============
==============================================================*/
#define	IRQ_SPI		0
#define	IRQ_SPI_A	IRQ_SPI
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
#define IRQ_MKIO_A_PAR_ERR 11
#define IRQ_MKIO_B_PAR_ERR 12
//#define IRQ_SPI_B	13
#define IRQ_SPW		14
#define IRQ_NMI		15
#define IRQ_GPIO	16
//#define IRQ_CAN		17
#define IRQ_DMA		18

#endif /* _CHIP_VM014_ */
