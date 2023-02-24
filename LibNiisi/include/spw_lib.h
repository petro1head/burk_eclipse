#ifndef _SPW_LIB_H_

#define _SPW_LIB_H_

// get id
int spw_get_id(void);
// how many ports
int spw_get_nport(void);
// init
int spw_init_port( int nPort );
int spw_reset_port(int nPort); 
int spw_disable_port(int nPort);
int spw_enable_port(int nPort);
int spw_get_port_status(int nPort);
// set own logical
int spw_set_own_logical( int nLogicalAddr );
int spw_get_own_logical( void );

// key, node logical enable
int spw_set_param( int nKey, int NLAen );
int spw_get_param( int *pnKey, int *pNLAen );

int spw_set_rmap( int nBegin1, int nEnd1, int nBegin2, int nEnd2, int nMaxSize);
int spw_get_rmap( int *pnBegin1, int *pnEnd1, int *pnBegin2, int *pnEnd2, int *pnMaxSize);

// set parameters for logical address
int spw_set_logical(int nLogicalAddress, int nPortsMask, int nDelHead, int nPriority, int nMulticast,  int nInvalid);
int spw_enable_logical ( int bEnable );	//enable/disable

//
int spw_set_ta_base( int nAddr );
int spw_set_ia_base( int nAddr );

int spw_set_path_logical(int nLogicalAddress, int nSize, char *pPath);
int spw_set_backpath_logical(int nLogicalAddress, int nSize, char *pPath);

int spw_set_txd_descr_area( int nAddr );
int spw_set_txd_header_area( int nAddr );
int spw_set_rxd_descr_area( int nAddr );
int spw_get_rxd_descr_flags(int nDescr );
int spw_get_rxd_descr_data_ptr(int nDescr );
// send
// nProtocal = 0,1
// rmap_cmd in range 0-15
// nBackPathLen in range 0-16
// return header length or an error
//
int spw_init_descr_header(int nDescr, int nProtocol, int nRmapCmd, int nAddress, int nBackPathLen, int nKey );
//int spw_make_txd_descr(int nDescr, int nTargetLogAddr, int nSourceDataAddr, int nDataSize, int bPath, int bLast);
int spw_make_txd_descr(int nDescr, int nTargetLogAddr, int nSourceDataAddr, int nDataSize, int bPath, int bLast, int bFR, int bIE );
int spw_is_ready_send(void);	// yes/no

int spw_start_send(int nFirstDescr);		// = TDE
int spw_is_finished( int nDescr, int nReply );
// use nTimer if >=0 or loop count ( < 0 )
int spw_wait_finished( int nDescr, int nTimer, int nTimeout, int nReply);

int spw_set_invalid_descriptor(int nDescr);
// send 1 command using nDescr descriptor
// wait finished
int spw_send_rmap_command( int nLogicalAddr, int nTargetAddr, int nKey, int nRmapCmd, int nSourceAddr, int nSize, int bPath, int nTimer, int nTimeout, int nDescr );

// to get not-RMAP packets
int spw_make_rxd_descr(int nDescr, int nAddress, int nMult, int bLast, int bIE );
int spw_is_ready_get(void);	// yes/no
int spw_start_get(int nFirstDescr);	// not RMAP;	SPW->RX.CONTROL |= 1;		//RDE = 1;

//
// return data_len or error
//
int spw_is_rxd_finished( int nDescr, int *pAddr);
//
// return data_len or error
//
int spw_wait_rxd_finished( int nDescr, int nTimer, int nTimeout, int *pAddr);

// buffer input 
int spw_set_buffer(int nMaxData, int nStartBuffer1, int nStartBuffer2, int nCurBuffer1, int nCurBuffer2);
int spw_get_buffer(int *pnCurDescr, int *pnMaxData, int *pnStartBuffer1, int *pnStartBuffer2, int *pnCurBuffer1, int *pnCurBuffer2);
// what buffer is ready
// 1 - first
// 2 - second
// 3 - both
// 0 - no one 
int spw_is_ready_get_buffer();	// 

// get data using buffer(s)
int spw_start_get_buffer(int nFirstDescr, int nBuffer1, int nBuffer2);	// not RMAP; buffer	

// interrupts
// tx interrupts
int spw_get_tx_int_req(void );
int spw_set_tx_int_req( int nValue );
// return old value
int spw_set_tx_int_enable( int bEnable );
int spw_get_tx_int_enable( void );

// rx interrupts
int spw_get_rx_int_req(void );
int spw_set_rx_int_req( int nValue );
int spw_set_rx_int_enable( int nValue );
int spw_get_rx_int_enable( void );

int spw_get_rx_int_info(void );
int spw_set_rx_int_info( int nValue );
int spw_get_rx_busy(void );
int spw_set_rx_busy( int nValue );

// protocol interrupt
// what ports interrupts enabled - send to interruot controller
int spw_set_prot_int_enable( int nValue );
int spw_get_prot_int_enable( void );
int spw_get_prot_int_req(void );
////////////////////
// port0 interrupts
int spw_set_prot_port0_int_enable( int nValue );
int spw_get_prot_port0_int_enable( void );
int spw_get_prot_port0_int_req(void );
//////////////////////////////

// port interrupts
int spw_get_port_intpend(int nPort );
int spw_set_port_intpend(int nPort, int nValue );
int spw_get_port_int_en(int nPort );
int spw_set_port_int_en(int nPort, int nValue );

/////////////////////
// router interrupts
int spw_get_node_intpend( );
int spw_set_node_intpend( int nValue );
int spw_get_node_int_en( );
int spw_set_node_int_en( int nValue );

/////////////////////
// no burst areas
int spw_get_no_burst(int *pnAddr1, int *pnMask1, int *pnAddr2, int *pnMask2);
int spw_set_no_burst(int nAddr1, int nMask1, int nAddr2, int nMask2);
// is burst or not	
// return 1 - area 1
// return 2 - area 2
// 0 - burst
int spw_is_no_burst( int nAddr );
//////////////////////////////////
// timecodes
// get register value
int spw_timecode_get();
// allow send timecodes
int spw_timecode_set_master(int bMaster, int nTimer);
// allow get timecodes from some ports
int spw_timecode_set_slave(int nPortMask);
// increment value to send
int spw_timecode_inc_count();
// set value to send
int spw_timecode_set_count(int nValue);
// get current value
int spw_timecode_get_count();
////////////////////
// test 4 bytes at address nAddr in connected module with target_logical_address - must be equal to nMark
// use path addressing (nPath > 0) or logical
// not more nLoopConnect, every loop nTimeout
// delay between loops
// work_addr - working address at source
//
// return 0 - OK
// else error
// 
int spw_test_ready( int work_addr, int target_log_addr, int nPath, int nTimer, int nTimeout, int nLoopConnect, int nConnectDelay, int nAddr, int nMark, int nKey  );

// return ready port mask
int spw_get_port_run_mask();

// block send commands
int spw_block_send( int tla, int cmd, int target_addr, int path, int data, int mask, int data_size, int key, int nTimer, int nTimeout );
int spw_block_get_data();
int spw_block_get_status();

// sequentional 
int spw_set_seq_first_last(int nFirst, int nLast);
int spw_get_seq_first_last(int *pnFirst, int *pnLast);
int spw_set_seq_mode();
// reset arbiter
int spw_arb_reset();

// advanced 

// standard initialize 
// return < 0 - error 
//	else returns nPortMask (active ports)
//	
int spw_init_standard(int bUsePll, int bWaitAllPorts, 
			int nLogicAddr, 
			int nTxDescrArea,  int nTxDescrHead, int nRxDescrArea, 
			int nTAarea, int nIAarea,
			int nTimer, int nTimeout);

// set some default parameters without full reset		
void spw_set_defaults();
		
// get SPW address
int spw_get_address(void);

// get error as string
char *spw_get_error_string( int nError );

// return codes used by spw_get_port_status()
#define STATUS_FSM_ERROR_RESET	0
#define STATUS_FSM_ERROR_WAIT	1
#define STATUS_FSM_READY        2
#define STATUS_FSM_STARTED      3
#define STATUS_FSM_CONNECING	4
#define STATUS_FSM_RUN          5

// block head
#define SPW_BLOCK_TLA	0xFF000000
#define SPW_BLOCK_INSTR	0x00FF0000
#define SPW_BLOCK_KEY	0x0000FF00
#define SPW_BLOCK_TAT	0x00000080
#define SPW_BLOCK_DL	0x00000070
#define SPW_BLOCK_EA	0x0000000F

// block status
#define SPW_BLOCK_STATUS	0x0000000F
#define SPW_BLOCK_INT		0x00008000
#define SPW_BLOCK_BUSY_TX	0x80000000


// list of RMAP commands
#define RMAP_INVALID0_CMD									(0x00)
#define RMAP_INVALID1_CMD									(0x01)
#define RMAP_READ_SINGLE_ADDR_CMD							(0x02)
#define RMAP_READ_INC_ADDR_CMD								(0x03)
#define RMAP_INVALID2_CMD									(0x04)
#define RMAP_INVALID3_CMD									(0x05)
#define RMAP_INVALID4_CMD									(0x06)
#define RMAP_READ_MW_CMD									(0x07)
#define RMAP_WRITE_SINGLE_ADDR_NO_VERIFY_NO_REPLY_CMD		(0x08)
#define RMAP_WRITE_SINGLE_ADDR_INC_NO_VERIFY_NO_REPLY_CMD	(0x09)
#define RMAP_WRITE_SINGLE_ADDR_NO_VERIFY_YES_REPLY_CMD		(0x0A)
#define RMAP_WRITE_SINGLE_ADDR_INC_NO_VERIFY_YES_REPLY_CMD	(0x0B)
#define RMAP_WRITE_SINGLE_ADDR_YES_VERIFY_NO_REPLY_CMD		(0x0C)
#define RMAP_WRITE_SINGLE_ADDR_INC_YES_VERIFY_NO_REPLY_CMD	(0x0D)
#define RMAP_WRITE_SINGLE_ADDR_YES_VERIFY_YES_REPLY_CMD		(0x0E)
#define RMAP_WRITE_SINGLE_ADDR_INC_YES_VERIFY_YES_REPLY_CMD	(0x0F)

// RMAP command bits
#define INC_BIT	    0x1	    // bit 1
#define REPLY_BIT	0x2		// bit 2
#define VERIFY_BIT	0x4		
#define WRITE_BIT	0x8


// errors 0 - -15 RMAP errors 
#define SPW_ERR_MEMORY			-21
#define SPW_ERR_INVALID_ARG		-22
#define SPW_ERR_INVALID_DESCR   -23
#define SPW_ERR_TIMEOUT_FV		-24
#define SPW_ERR_TIMEOUT_FT		-25
#define SPW_ERR_TIMEOUT_REPLY   -26
#define SPW_ERR_TIMEOUT_RXD		-27
#define SPW_ERR_RXD             -28
#define SPW_ERR_TA_NOT_SET		-29
#define SPW_ERR_IA_NOT_SET		-30
#define SPW_ERR_UNKNOWN_ID		-31
#define SPW_ERR_CONNECTED_NOT_READY	-32
#define SPW_ERR_TIMEOUT_BLOCK		-33
#define SPW_ERR_TIMEOUT_LINK		-34
#define SPW_ERR_TIMEOUT_SEND_READY	-35

#endif		// _SPW_LIB_H_
