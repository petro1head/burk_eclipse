#ifndef _SPW_H_
#define _SPW_H_
// Path address which resides in TAT and IAT
typedef struct {
	char size;
	char path[15];
} tSPW_PATH_ADDRESS_TA_TBL;

typedef struct {
	char path[16];
} tSPW_PATH_ADDRESS_IA_TBL;

typedef struct tagDPW_DMA_HDR{
	char protocol_id;
	char instr;
	char key;
	char extended_addr;
	int  addr; 
} tSPW_DMA_HDR;

typedef struct {
	int flags;
	int size;
	int data_ptr;
	int hdr_ptr; 
} tSPW_DMA_TXD_DESC;

#define SPW_FLAGS_TLA				0xff00
#define SPW_FLAGS_NCHL				0xff0000
#define SPW_FLAGS_TXD				0xff000000
#define SPW_FLAGS_TXD_IE			(1<<24)	// interrupt ena
#define SPW_FLAGS_TXD_GD			(1<<25)	// CRC generate for data
#define SPW_FLAGS_TXD_GH			(1<<26)	// CRC generate for header
#define SPW_FLAGS_TXD_TA			(1<<27) // RMAP: use TAT for source path addr 
#define SPW_FLAGS_TXD_LD			(1<<28)	// last descriptor
#define SPW_FLAGS_TXD_FT			(1<<29)	// TXD completed, set by HW, clr by SW
//#define SPW_FLAGS_TXD_FR			(1<<7)	// reply received, set by HW, clr by SW 
#define SPW_FLAGS_TXD_FR			(1<<30)	// reply received, set by HW, clr by SW 
//#define SPW_FLAGS_TXD_RESERVED		(1<<30)	// valid, set by SW, clr by HW - old version
#define SPW_FLAGS_TXD_FV			(1<<31)	// valid, set by SW, clr by HW


#define SPW_FLAGS_RMAP_REPLY		(1<<7)	// reply received, set by HW, clr by SW 

typedef struct {
	int flags;
	int data_ptr;
} tSPW_DMA_RXD_DESC;

#define SPW_FLAGS_RXD_MP			(1<<25)	// mp
#define SPW_FLAGS_RXD_IE			(1<<26)	// ie
#define SPW_FLAGS_RXD_LE			(1<<27)	// >max size
#define SPW_FLAGS_RXD_CP			(1<<28) // checksum error 
#define SPW_FLAGS_RXD_LD			(1<<29)	// last descriptor
#define SPW_FLAGS_RXD_FE			(1<<30)	// error 
#define SPW_FLAGS_RXD_FV			(1<<31)	// valid, set by SW, clr by HW

#define RMAP_COMMAND_PACKET				0x40
#define DMA_RMAP_PROTOCOL_ID			1

#define SPW_DMA_TXD_CTRL_TDE			(0x1<<0)

// input parameters
#define SPW_DMA_RXD_CTRL_RDE			(0x1<<0)
#define SPW_RXD_CTRL_RBE1			(0x1<<5)
#define SPW_RXD_CTRL_RBE2			(0x1<<6)

#define KE_ENABLE	(1<<16)
#define NLA_ENABLE	2

#define	COMM_CTRL_LOGADDREN			(0x1<<9)

#define COMM_DEL_HEAD	28
#define COMM_PRIORITY	29
#define COMM_MULTICAST	30
#define COMM_INVALID	31

#define CTRL_LINKDISA				(0x1<<3)
#define CTRL_LINKSTART				(0x1<<2)
#define CTRL_AUTOSTART				(0x1<<1)
#define CTRL_RESET					(0x1<<0)

#define OWN_PORT0_MASK  0x1

#define STATUS_FSM_STATE_MASK				0x7

#endif		// _SPW_H_
