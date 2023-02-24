#ifndef _DEVDESCR_VM014_
#define _DEVDESCR_VM014_

/*--------- TIMER ----------------*/

typedef struct {
volatile unsigned int ID;		// 0x00
volatile unsigned int CTRL;		// 0x04
volatile unsigned int CNT;		// 0x08
volatile unsigned int IRQ;		// 0x0c
volatile unsigned int DIV;		// 0x10
volatile unsigned int CCMP;		// 0x14
volatile unsigned int CMP[4];	// 0x18 - 0x27
volatile unsigned int CCAP;		// 0x28
volatile unsigned int CAP[4];	// 0x2c - 0x3b
volatile unsigned int ECMP;		// 0x3c
volatile unsigned int ABSENT;	// 0x40
} TIMER_UNIT;


/*--------- SPI ----------------*/

typedef struct {
volatile unsigned int MODE;		// 0x00
volatile unsigned int TIME;		// 0x04
volatile unsigned int CONTROL;	// 0x08
volatile unsigned int STATUS;	// 0x0c
volatile unsigned int RXF;		// 0x10
volatile unsigned int TXF;		// 0x14
volatile unsigned int CFG;		// 0x18
volatile unsigned int FSTAT;	// 0x1c
volatile unsigned int IEN;		// 0x20
volatile unsigned int IS;		// 0x24
volatile unsigned int dummy[4];
volatile unsigned int ABSENT;
} SPI_UNIT;

/*--------- INTERRUPT controller ----------------*/

typedef struct {
volatile unsigned int INTMAP[32];	// 0x00-0x7c
volatile unsigned int INTREQ;		// 0x80
volatile unsigned int dummy[3];		// 0x84-0x8c
volatile unsigned int INTEN;		// 0x90
volatile unsigned int dummy1[4];
volatile unsigned int ABSENT;
} INTC_UNIT;

#define	INTC_LEV_HI	0x0
#define	INTC_LEV_LO	0x1
#define	INTC_EDGE_RISE	0x2
#define	INTC_EDGE_FALL	0x3

/*--------- GPIO controller ----------------*/

typedef struct {
volatile unsigned int IDREG;	// 0x00
volatile unsigned int IN;		// 0x04
volatile unsigned int OUT;		// 0x08
volatile unsigned int SET;		// 0x0c
volatile unsigned int CLR;		// 0x10
volatile unsigned int INVERT;	// 0x14
volatile unsigned int DIR;		// 0x18
volatile unsigned int INTFLAG;	// 0x1c

volatile unsigned int AFIO;		// 0x20
volatile unsigned int AFMODE;	// 0x24
volatile unsigned int dummy;	// 0x28
volatile unsigned int MODE[32];	// 0x2c-0xa8
} GPIO_UNIT;
 
/*----------- MKIO ----------------------*/





typedef struct {
volatile unsigned int Config;	//    0
volatile unsigned int Control;	//    4
volatile unsigned int IntEn;	//    8
volatile unsigned int Int;		//  0xc
volatile unsigned int Status;	// 0x10
volatile unsigned int Pointer;	// 0x14
volatile unsigned int Mask;		// 0x18
volatile unsigned int Timer;	// 0x1c
volatile unsigned int Address;	// 0x20
volatile unsigned int Eword;	// 0x24
volatile unsigned int Codec;	// 0x28 Coder W, Decoder R
volatile unsigned int Data;		// 0x2c
volatile unsigned int Answer;	// 0x30
volatile unsigned int d4;		// 0x34
volatile unsigned int d5;		// 0x38
volatile unsigned int Busy;		// 0x3c
volatile unsigned int dummy1[4];
volatile unsigned int ABSENT;
} MKIO_UNIT;

typedef struct {
volatile union  {
	unsigned int raw;
	struct {
		unsigned int mode								:2;
		unsigned int frameRepeatEnable					:1;
		unsigned int frameInterruptOnEndEnable			:1;
		unsigned int frameInterruptOnMessageEnable		:1;
		unsigned int frameInterruptErrorOnEndEnable		:1;
		unsigned int frameInterruptErrorOnMessageEnable	:1;
		unsigned int signalLevelInactive				:1;
		unsigned int waitTime							:2;
		unsigned int reserve							:1;
		unsigned int selfControl0						:1;
		unsigned int selfControl1						:1;
	}bitBc;
	struct {
		unsigned int mode								:2;
		unsigned int rtAddress							:5;
		unsigned int waitTime							:2;
		unsigned int rtAddressParity					:1;
		unsigned int selfControl0						:1;
		unsigned int selfControl1						:1;
		unsigned int broadcastReceiveEnable				:1;
		unsigned int broadcastDataSeparateEnable		:1;
		unsigned int bcOvertakeEnable					:1;
	}bitRt;
}Config;

volatile union {
	unsigned int raw;
	struct {
		unsigned int reset					:1;
		unsigned int rtAddressLoad			:1;
		unsigned int resetInterrupts		:1;
		unsigned int flagRtError			:1;
		unsigned int flagSystemError		:1;
		unsigned int flagRepairRequest		:1;
		unsigned int flagBusy				:1;
		unsigned int disableChannelA		:1;
		unsigned int disableChannelB		:1;
		unsigned int flagRtErrorDisable		:1;
	}bitRt;
	struct {
		unsigned int reset					:1;
		unsigned int start					:1;
		unsigned int resetInterrupts		:1;
		unsigned int interruptOnFrameEnd	:1;
		unsigned int interruptOnMessageEnd	:1;
		unsigned int frameCurrent			:1;
	}bitBc;
} Control;	//    4;


volatile unsigned int IntEn;	//    8
volatile unsigned int Int;		//  0xc
volatile unsigned int Status;	// 0x10
volatile unsigned int Pointer;	// 0x14
volatile unsigned int Mask;		// 0x18
volatile unsigned int Timer;	// 0x1c
volatile unsigned int Address;	// 0x20
volatile unsigned int Eword;	// 0x24
volatile unsigned int Codec;	// 0x28 Coder W, Decoder R
volatile unsigned int Data;		// 0x2c
volatile unsigned int Answer;	// 0x30
volatile unsigned int d4;		// 0x34
volatile unsigned int d5;		// 0x38
volatile unsigned int Busy;		// 0x3c
volatile unsigned int dummy1[4];
volatile unsigned int ABSENT;
} mkoRegisters;


/*------------- UART -------------------------*/

typedef struct {
volatile unsigned int RXTX;
volatile unsigned int IER;
volatile unsigned int IIRF;
volatile unsigned int LCR;
volatile unsigned int MCR;
volatile unsigned int LSR;
volatile unsigned int MSR;
volatile unsigned int SCR;
volatile unsigned int CTL;
} UART_UNIT;

/*------------- SYS Unit -------------------------*/

typedef struct {
volatile unsigned int PLL_CTRL;		// 000	RW
volatile unsigned int DIS_CLK;		// 004	RW
volatile unsigned int DIV_CTRL;		// 008	RW
volatile unsigned int ACC_CTRL;		// 00c	RW
volatile unsigned int STATUS1;		// 010	R
volatile unsigned int STATUS2;		// 014	R
volatile unsigned int dummy[4];
volatile unsigned int ABSENT;
} SYS_UNIT;

/*       device        bit number in DIS_CLK reg */
#define CLK_UART_A	0
#define CLK_UART_B	1
#define CLK_TIMER_A	2
#define CLK_TIMER_B	3
#define CLK_TIMER_C	4
#define CLK_MKIO_A	5
#define CLK_MKIO_B	6
#define CLK_SPI		7
#define CLK_I2C		8
#define CLK_ETH		9
#define CLK_SPW		10
#define CLK_OCP_24	11
#define CLK_CAN		12

/*       device        bits in STATUS1/STATUS2 regs */
#define SYS_S1_SPW	0
#define SYS_S1_MKIO_B	4
#define SYS_S1_MKIO_A	8
#define SYS_S1_TIMER_C	12
#define SYS_S1_TIMER_B	16
#define SYS_S1_TIMER_A	20
#define SYS_S1_UART_B	24
#define SYS_S1_UART_A	28

#define SYS_S2_I2C	0
#define SYS_S2_ETH	4
#define SYS_S2_CLK_25	6
#define SYS_S2_SPW	8
#define SYS_S2_OCP_24	12


/*------------- SPW Unit -------------------------*/

#define SPW_PORTS_NUM 2

/* --- PORT  (base 0x100, 0x200, 0x300, 0x400, 0xa00, 0xb00, 0xc00, 0xd00) */
typedef struct {
volatile unsigned int CONTROL;		// 0x0n00
volatile unsigned int STATUS;		// 0x0n04
volatile unsigned int INTPEND;		// 0x0n08
volatile unsigned int INT_EN;		// 0x0n0c
volatile unsigned int CLKDIV;		// 0x0n10
volatile unsigned int TIMERDISC;	// 0x0n14
volatile unsigned int Reserved[(0x100-0x18)/4];	// 0x0n18-0x0nff
} SPW_PORT_UNIT;

/* --- NODE registers (base 0x0000) */
typedef struct {
volatile unsigned int SPW_ID;					// 0x0000
volatile unsigned int TXD;						// 0x0004
volatile unsigned int RXD;						// 0x0008
volatile unsigned int TIMECODE;					// 0x000c
volatile unsigned int INTPEND;					// 0x0010
volatile unsigned int INT_EN;					// 0x0014
volatile unsigned int TIMECODE_RX_EN;			// 0x0018
volatile unsigned int TIMEOUT_OUTPORT_INT;		// 0x001c
volatile unsigned int TIMEOUT_INPORT_INT;		// 0x0020
volatile unsigned int TIMEOUT_OUTPORT_INT_EN;	// 0x0024
volatile unsigned int TIMEOUT_INPORT_INT_EN;	// 0x0028
volatile unsigned int Reserved[(0x100-0x2c)/4];	// 0x002c-0x00ff
} SPW_NODE_UNIT;

/* --- RX channel (base 0x0500) */
typedef struct {
volatile unsigned int CONTROL;			// 0x0500
volatile unsigned int MAX_DATA;			// 0x0504
volatile unsigned int D_INDEX;			// 0x0508
volatile unsigned int BEG_MEM_ADDR1;	// 0x050c
volatile unsigned int END_MEM_ADDR1;	// 0x0510
volatile unsigned int BEG_MEM_ADDR2;	// 0x0514
volatile unsigned int END_MEM_ADDR2;	// 0x0518
volatile unsigned int INT_INFO;			// 0x051c
volatile unsigned int INT_EN;			// 0x0520
volatile unsigned int INT_REQ;			// 0x0524
volatile unsigned int BUSY;				// 0x0528
// Listopadov - from swhub devstruct
volatile unsigned int BufStartAddr1;	// 0x052C
volatile unsigned int BufStartAddr2;	// 0x0530
volatile unsigned int BufCurAddr1;	// 0x0534
volatile unsigned int BufCurAddr2;	// 0x0538
volatile unsigned int RMAP_ERR;		// 0x053C
volatile unsigned int Reserved[(0x100-0x40)/4];	// 0x0540-0x05ff
} SPW_RX_UNIT;

/* --- Global registers PROTOCOL level (base 0x0600) */
typedef struct {
volatile unsigned int D_BASE_TX;	// 0x0600
volatile unsigned int D_BASE_RX;	// 0x0604
volatile unsigned int INT_REQ;		// 0x0608
volatile unsigned int INT_EN;		// 0x060c
volatile unsigned int NODEADDR;		// 0x0610
volatile unsigned int INT_PORT;		// 0x0614
volatile unsigned int INT_PORT_EN;	// 0x0618
volatile unsigned int SPW_mod;		// 0x061C
volatile unsigned int NO_BURST_ADDR1;	// 0x0620
volatile unsigned int NO_BURST_MASK1;	// 0x0624
volatile unsigned int NO_BURST_ADDR2;	// 0x0628
volatile unsigned int NO_BURST_MASK2;	// 0x062C
volatile unsigned int Reserved[(0x100-0x30)/4];	// 0x0630-0x06ff 
} SPW_PROTOCOL_UNIT;

/* --- Pocket TX block */
typedef struct {
volatile unsigned int HEAD;			// 0x0700
volatile unsigned int TARG_ADDR;	// 0x0704
volatile unsigned int DATA;			// 0x0708
volatile unsigned int MASK;			// 0x070c
volatile unsigned int STATUS;		// 0x0710
/* TX channel */
volatile unsigned int CONTROL;		// 0x0714
volatile unsigned int TA_BASE;		// 0x0718
volatile unsigned int IA_BASE;		// 0x071c
volatile unsigned int D_INDEX;		// 0x0720
volatile unsigned int INT_REQ;		// 0x0724
volatile unsigned int INT_EN;		// 0x0728
volatile unsigned int Reserved[(0x100-0x2c)/4];	// 0x072c-0x07ff
} SPW_TX_UNIT;

/* --- Global registers NET level (base 0x0800)  */
typedef struct {
volatile unsigned int SWCONTROL;	// 0x0800
volatile unsigned int PORT_RS;		// 0x0804
volatile unsigned int TIMEOUT_LIMIT_TX;	// 0x0808
volatile unsigned int TIMEOUTS_IN_EN;	// 0x080c
volatile unsigned int TIMEOUTS_OUT_EN;	// 0x0810
volatile unsigned int TIMEOUT_LIMIT_IN;	// 0x0814
volatile unsigned int TIMEOUT_LIMIT_OUT;// 0x0818
volatile unsigned int PORT_ACT;		// 0x081c
volatile unsigned int PORT_NUM[32];	// 0x0820-0x089f
volatile unsigned int SEQ_FIRST;	// 0x08a0
volatile unsigned int SEQ_LAST;		// 0x08a4
volatile unsigned int Reserved[(0x100-0xa8)/4];	// 0x08a8-0x08ff
} SPW_NET_UNIT;

typedef struct {
SPW_NODE_UNIT		NODE;		// 0x0000
SPW_PORT_UNIT		LPORT[4];	// 0x0100-0x04ff
SPW_RX_UNIT		RX;		// 0x0500
SPW_PROTOCOL_UNIT	PROT;		// 0x0600 контроллер ПДП (прямой доступ в память)
SPW_TX_UNIT		TX;		// 0x0700
SPW_NET_UNIT		NET;		// 0x0800 регистры маршрутизатора
int			Reserved0[0x100/4];		// 0x0900-0x09ff
SPW_PORT_UNIT		HPORT[4];			// 0x0a00-0x0dff
int			Reserved1[(0x1080-0x0e00)/4];	// 0x0e00-0x107f
volatile unsigned int 	COMM[(0x1400-0x1080)/4];	// 0x1080-0x13ff таблица коммутации
} SPW_UNIT;


#endif /* _DEVDESCR_VM014_ */
