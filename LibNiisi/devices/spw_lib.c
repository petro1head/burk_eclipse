#include "chip.h"
#include "devdescr.h"
#include "io.h"

#include "spw.h"
#include "spw_lib.h"

#include "timer.h"
#include "sys_unit.h"
#include "chip_vm014.h"
//#define _LOG_


static volatile tSPW_DMA_HDR *pDescrHeader = 0;
static volatile tSPW_DMA_TXD_DESC *Dma_TXD_Desc = 0;	//(volatile tSPW_DMA_TXD_DESC * )SPW_TXD_DESC_BASE;
static volatile tSPW_DMA_RXD_DESC *Dma_RXD_Desc = 0;	//(volatile tSPW_DMA_RXD_DESC * )SPW_RXD_DESC_BASE;

static volatile tSPW_PATH_ADDRESS_TA_TBL *ta_base = 0;
static volatile tSPW_PATH_ADDRESS_IA_TBL *ia_base = 0;

#if 0
#if SPW_PRIME
int pll_get_freq()
{
    return ( timer_get_sys_freq(0,1) );
//	return (frequency10()/10);
}
#endif
//////////////////////////
int pll_get_freq()
{
	volatile int div_ref, div, vcod, post_d, pll_freq;
	static SYS_UNIT * SYS;

	SYS = (SYS_UNIT *)SYSUNIT_BASE;

	div_ref = (SYS->PLL_CTRL >> 24) & 0xff;
	div     = (SYS->PLL_CTRL >> 16) & 0xff;
	vcod    = 1 << ((SYS->PLL_CTRL >> 10) & 3);
	post_d  = 1 << ((SYS->PLL_CTRL >> 8)  & 3);
	pll_freq = 24 * div / div_ref / post_d;

	return pll_freq;
} // pll_get_freq
#endif

//////////////////////////
int spw_get_id()
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	return SPW->NODE.SPW_ID ;
}
//////////////////////////
int spw_get_nport()
{
#if 1
	return SPW_PORTS_NUM;

#else
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	int C0ID = read_c0_prid();
	// how many ports
	if ((C0ID == 0x01001102)||(C0ID == 0x01001300)) {
		return 2;	//nPort = 2;
	}

	switch ( SPW->NODE.SPW_ID )
	{
		case 0x600:
			return 4;
		case 0x601:
			return 8;
		case 0x610:
			return 4;
		case 0x611:
			return 8;	
		default:
			break;
	}


	return SPW_ERR_UNKNOWN_ID;
#endif
}
//////////////////////////////////

int spw_init_port( int nPort )
{
// Listopadov - disabled this eight ports check
//#if SPW_PORTS_NUM != 8
//	return 0;
//#else
//	spw_SetTimeClckDiv( int pll, int nPort ) - inside
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	int time_850 = 2*0x24;
	int time_64 = 2*0x130;

	volatile SPW_PORT_UNIT *spw_reg;
	int pll = 100; 

	int NumPort = spw_get_nport();

	//if( NumPort != 8 )
	//	return 0;

	if( nPort < 0 || nPort > NumPort-1)
		return SPW_ERR_INVALID_ARG;

	
	if( nPort < 4 )
		spw_reg = &SPW->LPORT[nPort];	
	else
		spw_reg = &SPW->HPORT[nPort-4];	

#ifdef _LOG__
	printf("timedis=%x\n", spw_reg->TIMERDISC); 
	printf("clkdiv=%x\n", spw_reg->CLKDIV);
#endif

	pll = pll_get_freq();
	pll /=2;
//	pll = 50;	// temp !!!
	
	if( pll > 0 )
	{
//		time_850 = (2*pll*73)/200;
//		time_64 = (2*pll*60)/20

//		time_850 = (2*pll*80)/200;
//		time_64 = (2*pll*70)/20;	// *600/200//

//		time_850 = ( 850*pll)/(1000*2);
//		time_64  = (6400*pll)/(1000*2);

		time_850 = ( 85*pll)/(100);
		time_64  = (640*pll)/(100);


	}

//	printf("pll=%x %x %x\n",pll,time_850,time_64);
	 
	spw_reg->TIMERDISC = (time_850<<16)|time_64;

	// Amosov - could be problems on RTL if swich odd-even
//	if( spw_reg->CLKDIV & 1 )
//		spw_reg->CLKDIV = ((pll)<<16) | 1;
//	else
//	{
//		spw_reg->CLKDIV = ((pll)<<16) | 0;
//	}
	spw_reg->CLKDIV = 0 | 0;
//	spw_reg->CLKDIV = ((pll/5)<<16) | 0;
//			spw_reg->CLKDIV = ((pll/5)<<16) | 2;


#ifdef _LOG__
	printf("timedis_new=%x\n", spw_reg->TIMERDISC);
	printf("clkdiv_new=%x\n", spw_reg->CLKDIV);

#endif

	return pll;
//#endif
}

// reset
int spw_reset_port(int nPort) 
{
	volatile SPW_PORT_UNIT *spw_reg;
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	int NumPort = spw_get_nport();

	if( nPort < 0 || nPort > NumPort - 1 )
		return SPW_ERR_INVALID_ARG;

	if( nPort < 4 )
		spw_reg = &SPW->LPORT[nPort];	

	else
	{
		if( nPort < 8 )
			spw_reg = &SPW->HPORT[nPort-4];	
		else
			return SPW_ERR_INVALID_ARG;
	}

	spw_reg->CONTROL |= CTRL_LINKSTART|CTRL_AUTOSTART;

    spw_reg->CONTROL |= CTRL_RESET;
	return 0;
}
//////////////////
int spw_disable_port(int nPort)
{
	volatile SPW_PORT_UNIT *spw_reg;
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	int NumPort = spw_get_nport();
	if( nPort < 0 || nPort > NumPort - 1 )
		return SPW_ERR_INVALID_ARG;

	if( nPort < 4 )
		spw_reg = &SPW->LPORT[nPort];	

	else
	{
		if( nPort < 8 )
			spw_reg = &SPW->HPORT[nPort-4];	
		else
			return SPW_ERR_INVALID_ARG;
	}

//	spw_reg->CONTROL &= !(CTRL_LINKSTART|CTRL_AUprintfTART);

    spw_reg->CONTROL |= CTRL_LINKDISA;

	return 0;
}
/////////////////////////////////////////
int spw_enable_port(int nPort)
{
	volatile SPW_PORT_UNIT *spw_reg;
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	int NumPort = spw_get_nport();
	if( nPort < 0 || nPort > NumPort - 1 )
		return SPW_ERR_INVALID_ARG;

	if( nPort < 4 )
		spw_reg = &SPW->LPORT[nPort];	

	else
		spw_reg = &SPW->HPORT[nPort-4];	

	spw_reg->CONTROL |= CTRL_LINKSTART|CTRL_AUTOSTART;
    spw_reg->CONTROL &= ~CTRL_LINKDISA;

	return 0;
}
/////////////////////////////////////////

// status 
// returns one of STATUS_FSM_...
// or error
int spw_get_port_status(int nPort)
{
	volatile SPW_PORT_UNIT *spw_reg;
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	int NumPort = spw_get_nport();

	if( nPort < 0 || nPort > NumPort-1 )
		return SPW_ERR_INVALID_ARG;

	if( nPort < 4 )
		spw_reg = &SPW->LPORT[nPort];	

	else
    {if(nPort < 8 )
		spw_reg = &SPW->HPORT[nPort-4];	
       else
		return SPW_ERR_INVALID_ARG;
    }

	return spw_reg->STATUS & STATUS_FSM_STATE_MASK;
}
///////////////////////////
// return ready port mask
int spw_get_port_run_mask()
{
	int nPortMask = 0;
	int i, nPort;

	nPort = spw_get_nport();
	if( nPort < 0 )
		return nPort;

		// anybody ready ?
	for(i=0;i<nPort;i++)
	{
		if( spw_get_port_status(i) == STATUS_FSM_RUN )
			nPortMask |= 1<<(i+1);
	}

	return nPortMask;
}
///////////////
static void spw_memset(char *pBuf, int cVal, int nSize)
{
	if( nSize <= 0 || !pBuf)
		return;

	for(;nSize;nSize--,pBuf++)
		*pBuf = cVal;
}
////////////////////
static void spw_memsetw(int *pnBuf, int cVal, int nSize)
{
	if( nSize <= 0 || !pnBuf)
		return;

	// from bytes to ints
	nSize /= sizeof(int);
	for(;nSize;nSize--,pnBuf++)
		*pnBuf = cVal;
}
////////////////////

// set necessary values 
// return old value
int spw_set_own_logical( int nLogicalAddr )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	int i = SPW->PROT.NODEADDR;
	SPW->PROT.NODEADDR = nLogicalAddr;

	SPW->COMM[nLogicalAddr-0x20] = OWN_PORT0_MASK;
	//spw_SetPortsLogical( nLogicalAddr, OWN_PORT0_MASK,0,0,0);

	return i;
}
/////////////////////

int spw_get_own_logical( void )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	return SPW->PROT.NODEADDR ;
}

//////////////////////
// set parameters
int spw_set_param( int nKey, int NLAen )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	nKey &= 0xFF;
	if( nKey > 0 )
		SPW->RX.CONTROL = (nKey<<8) | KE_ENABLE | (NLAen?NLA_ENABLE:0);	//0x100;
	else
		SPW->RX.CONTROL = (NLAen?NLA_ENABLE:0);	
	//SPW->RX.END_MEM_ADDR2 = 0x0700000;	//0xffffffff;
	
	return SPW->RX.CONTROL;
}
/////////////////
int spw_get_param( int *pnKey, int *pNLAen )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	*pNLAen = (SPW->RX.CONTROL & NLA_ENABLE) ? 1:0;

	if( SPW->RX.CONTROL & KE_ENABLE )
	{
		*pnKey = (SPW->RX.CONTROL >> 8 ) & 0xFF;
	}
	else
		*pnKey = 0;

	return 0;
}
/////////////////////////
int spw_set_rmap( int nBegin1, int nEnd1, int nBegin2, int nEnd2, int nMaxSize)
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	SPW->RX.BEG_MEM_ADDR1 = nBegin1;
	SPW->RX.END_MEM_ADDR1 = nEnd1;

	SPW->RX.BEG_MEM_ADDR2 = nBegin2;
	SPW->RX.END_MEM_ADDR2 = nEnd2;

	SPW->RX.MAX_DATA = nMaxSize;

	return 0;
}
////////////////////////
int spw_get_rmap( int *pnBegin1, int *pnEnd1, int *pnBegin2, int *pnEnd2, int *pnMaxSize)
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	
	*pnBegin1 = SPW->RX.BEG_MEM_ADDR1 ;
	*pnEnd1 = SPW->RX.END_MEM_ADDR1;

	*pnBegin2 = SPW->RX.BEG_MEM_ADDR2 ;
	*pnEnd2 = SPW->RX.END_MEM_ADDR2 ;

	*pnMaxSize = SPW->RX.MAX_DATA;

	return 0;
}
///////////////////////
// what ports could be used for the logical address
// set parameters
int spw_set_logical(int nLogicalAddress, int nPortsMask, int nDelHead, int nPriority, int nMulticast,  int nInvalid)
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	if( nLogicalAddress<0x20 || nLogicalAddress>=0x100 )	
		return SPW_ERR_INVALID_ARG;

	SPW->COMM[nLogicalAddress-0x20] = nPortsMask;	//|COMM_TBL_MULTICAST;
	 
	if( nDelHead )
		SPW->COMM[nLogicalAddress-0x20] |= 1 << COMM_DEL_HEAD;
	if( nPriority )
		SPW->COMM[nLogicalAddress-0x20] |= 1 << COMM_PRIORITY;
	
	if( nMulticast )
		SPW->COMM[nLogicalAddress-0x20] |= 1 << COMM_MULTICAST;
	if( nInvalid )
		SPW->COMM[nLogicalAddress-0x20] |= 1 << COMM_INVALID;

	return 0;
	
}
//////////////////////

// logical addressing
int spw_enable_logical ( int bEnable )	//enable/disable
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	if( bEnable )
		SPW->NET.SWCONTROL |=(COMM_CTRL_LOGADDREN);
	else
		SPW->NET.SWCONTROL &=~(COMM_CTRL_LOGADDREN);

	return 0;
}
////////////////////////////
// set TA, IA base
int spw_set_ta_base( int nAddr )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	SPW->TX.TA_BASE = nAddr & 0x1fffffff;

//#ifdef SCHEMA_10
	ta_base = (tSPW_PATH_ADDRESS_TA_TBL *)nAddr;
//#else
//	ta_base = (tSPW_PATH_ADDRESS_TA_TBL *)SPW->TX.TA_BASE;
//#endif

	return 0;
}
////////////////////////////
int spw_set_ia_base( int nAddr )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	SPW->TX.IA_BASE = nAddr & 0x1fffffff;

//#ifdef SCHEMA_10
	ia_base = (tSPW_PATH_ADDRESS_IA_TBL *)nAddr;
//#else
//	ia_base = (tSPW_PATH_ADDRESS_IA_TBL *)SPW->TX.IA_BASE;
//#endif

	return 0;

}
//////////////////////

// pathes to and back 
int spw_set_path_logical(int nLogicalAddress, int nSize, char *pPath)
{
	int i;

	if( nLogicalAddress < 0 || nLogicalAddress>=0x100 )	
		return SPW_ERR_INVALID_ARG;

	if( nSize < 0 || nSize > 15 )
		return SPW_ERR_INVALID_ARG;

	if( !ta_base )
		return SPW_ERR_TA_NOT_SET;

//	ta_base = (tSPW_PATH_ADDRESS_TA_TBL *)SPW->TX.TA_BASE;

	ta_base[nLogicalAddress].size = nSize;
	for(i=0;i<nSize;i++)
		ta_base[nLogicalAddress].path[i] = pPath[i];

	return nSize;
}
//////////////////////
int spw_set_backpath_logical(int nLogicalAddress, int nSize, char *pPath)
{
	int i;

	if( nLogicalAddress < 0 || nLogicalAddress>=0x100 )	
		return SPW_ERR_INVALID_ARG;

	if( nSize < 0 || nSize > 16 )
		return SPW_ERR_INVALID_ARG;

	if( !ia_base )
		return SPW_ERR_IA_NOT_SET;

//	ia_base = (tSPW_PATH_ADDRESS_IA_TBL *)SPW->TX.IA_BASE;

	for(i=0;i<nSize;i++)
		ia_base[nLogicalAddress].path[i] = pPath[i];

	return nSize;

}
///////////////////////////
// descriptors are here 
int spw_set_txd_descr_area( int nAddr )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	Dma_TXD_Desc = (volatile tSPW_DMA_TXD_DESC * )nAddr;
	spw_memsetw( (int *)Dma_TXD_Desc, 0, 1024*sizeof(tSPW_DMA_TXD_DESC));

	SPW->PROT.D_BASE_TX = nAddr & 0x1fffffff;

	return 0;
}
////////////////////
int spw_set_txd_header_area( int nAddr )
{
	pDescrHeader = (tSPW_DMA_HDR *)nAddr;

	return 0;
}
/////////////////////
int spw_set_rxd_descr_area( int nAddr )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	Dma_RXD_Desc = (volatile tSPW_DMA_RXD_DESC * )nAddr;
	spw_memsetw( (int *)Dma_RXD_Desc, 0, 1024*sizeof(tSPW_DMA_RXD_DESC));

	SPW->PROT.D_BASE_RX = nAddr & 0x1fffffff ;

	return 0;
}
///////////////////////////////
// send
// nProtocal = 0,1
// rmap_cmd in range 0-15
// nBackPathLen in range 0-16
// return header length or an error
//
int spw_init_descr_header(int nDescr, int nProtocol, int nRmapCmd, int nAddress, int nBackPathLen, int nKey )
{

	if( !pDescrHeader )
		return SPW_ERR_INVALID_DESCR;

	if( nDescr < 0 || nDescr >= 1024 )
		return SPW_ERR_INVALID_ARG;

	if( nProtocol != DMA_RMAP_PROTOCOL_ID )
	{
		pDescrHeader[nDescr].protocol_id = nProtocol;
		return 1;
	}

//	if( nRmapCmd < 0 || nRmapCmd > 15 )
//		return SPW_ERR_INVALID_ARG;

	// no back path must be set in case no Reply - Amosov
	if( !(nRmapCmd & 2) )
		nBackPathLen = 0;
	
	if( nBackPathLen < 0 || nBackPathLen > 16 )
		return SPW_ERR_INVALID_ARG;

	pDescrHeader[nDescr].protocol_id = DMA_RMAP_PROTOCOL_ID;
	pDescrHeader[nDescr].key = nKey;
	pDescrHeader[nDescr].addr = nAddress & 0x1FFFFFFF ;
	pDescrHeader[nDescr].extended_addr = 0;

	pDescrHeader[nDescr].instr = nRmapCmd;

	//pDescrHeader[nDescr].instr = RMAP_COMMAND_PACKET | (nRmapCmd<<2) | ((nBackPathLen+3)>>2);
	
	return 8;		// bytes
}
////////////////////////////////
int spw_make_txd_descr(int nDescr, int nTargetLogAddr, int nSourceDataAddr, int nDataSize, int bPath, int bLast, int bFR, int bIE )
{
//	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	tSPW_DMA_TXD_DESC *desc;
	int nSize;

	if( nDescr < 0 || nDescr >= 1024 )
		return SPW_ERR_INVALID_ARG;

	if( !pDescrHeader )
		return SPW_ERR_INVALID_DESCR;

//	desc = (tSPW_DMA_TXD_DESC *)SPW->PROT.D_BASE_TX;
	desc = (tSPW_DMA_TXD_DESC *)Dma_TXD_Desc;

	if( !desc )
		return SPW_ERR_INVALID_DESCR;

	desc += nDescr;
	spw_memset( (char *)desc, sizeof( tSPW_DMA_TXD_DESC ), 0 );


	desc->flags 	= 0;
	if( bFR );
//		desc->flags |= SPW_FLAGS_TXD_FR;
	if( bIE )
		desc->flags 	|= SPW_FLAGS_TXD_IE;

	if( bPath )
		desc->flags 	|= SPW_FLAGS_TXD_TA;

	if( bLast )
		desc->flags |= SPW_FLAGS_TXD_LD;

	desc->flags |= SPW_FLAGS_TLA & (nTargetLogAddr<<8) ;

	desc->flags 	|= SPW_FLAGS_TXD_FV;


	if( pDescrHeader[nDescr].protocol_id == DMA_RMAP_PROTOCOL_ID )
		nSize = sizeof( tSPW_DMA_HDR );
	else
		nSize = 1;

	desc->size 		= (nSize<<24)|(nDataSize & 0xFFFFFF );
	
	desc->hdr_ptr	=  (int) (pDescrHeader + nDescr); 
	desc->hdr_ptr  &= 0x1FFFFFFF;

	desc->data_ptr 	= nSourceDataAddr & 0x1FFFFFFF;

#ifdef _LOG_
	printf("descr=%x", desc->flags);
#endif
	
	return 0;
}
////////////////////////////////
int spw_is_ready_send()	// yes/no
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	// is sending ?
	if( SPW->TX.CONTROL & SPW_DMA_TXD_CTRL_TDE )
		return 1;
	// ready
	return 0;
}
//////////////////////////////////
int spw_start_send(int nFirstDescr)		// = TDE
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	if( nFirstDescr < 0 || nFirstDescr >= 1024 )
		return SPW_ERR_INVALID_ARG;

	SPW->TX.D_INDEX = nFirstDescr;
	SPW->TX.CONTROL |= SPW_DMA_TXD_CTRL_TDE ;
	return 0;

}
////////////////////////
int spw_is_finished( int nDescr, int nReply )
{
//	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	volatile tSPW_DMA_TXD_DESC *curr_desc;

	if( nDescr < 0 || nDescr >= 1024 )
		return SPW_ERR_INVALID_ARG;

//	curr_desc = (tSPW_DMA_TXD_DESC *)SPW->PROT.D_BASE_TX;
	curr_desc = (volatile tSPW_DMA_TXD_DESC *)Dma_TXD_Desc;

	if( !curr_desc )
		return SPW_ERR_INVALID_DESCR;

	curr_desc += nDescr;

#ifdef _LOG_
	printf("%x\n", curr_desc->flags);
#endif

	// error ?
	if( curr_desc->flags & 0x7F ) {
		return -(curr_desc->flags & 0x7F);
	}

		//  all is sent ?
	if ( curr_desc->flags & SPW_FLAGS_TXD_FV ) 
		return 1;
#ifdef _LOG_
	printf("FV set\n");
#endif

	if ( !(curr_desc->flags & SPW_FLAGS_TXD_FT) ) {
		return 1;
	}

#ifdef _LOG_
	printf("FT set\n");
#endif

	// error ?
	if( curr_desc->flags & 0x7F ) {
		return -(curr_desc->flags & 0x7F);
	}

	// reply ?
	//if ( !(curr_desc->flags & SPW_FLAGS_TXD_FR ) ) {  
	if( nReply ){
		if ( !(curr_desc->flags & SPW_FLAGS_RMAP_REPLY)  )  
			return 1;
	}

	return 0;
}
////////////////
int spw_wait_finished( int nDescr, int nTimer, int nTimeout, int nReply)
{
//	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	volatile tSPW_DMA_TXD_DESC *curr_desc;
	int nTime = 0;

	if( nDescr < 0 || nDescr >= 1024 )
		return SPW_ERR_INVALID_ARG;

//	curr_desc = (tSPW_DMA_TXD_DESC *)SPW->PROT.D_BASE_TX;
	curr_desc = (volatile tSPW_DMA_TXD_DESC *)Dma_TXD_Desc;

	if( !curr_desc )
		return SPW_ERR_INVALID_DESCR;

	curr_desc += nDescr;
	
#ifdef _LOG_
	printf("%x\n", curr_desc->flags);
#endif

	if( nTimer >= 0 )
		timer_start(nTimer);

	while ( nTime < nTimeout )
	{
		//  all is sent ?
		if ( !(curr_desc->flags & SPW_FLAGS_TXD_FV) ) 
			break;
			// error ?
		if( curr_desc->flags & 0x7F ) 
			break;
		
		if( nTimer >= 0 )
			nTime = timer_read( nTimer );
		else 
			nTime++;	
	}

	if( nTimer >= 0 )
		timer_stop( nTimer );

	if( curr_desc->flags & 0x7F ) 
		return  - (curr_desc->flags & 0x7F);
		
	if( (curr_desc->flags & SPW_FLAGS_TXD_FV) && nTime >= nTimeout )
		return SPW_ERR_TIMEOUT_FV;
#ifdef _LOG_
	printf("FV set\n");
#endif

	nTime = 0;
	if( nTimer >= 0 )
		timer_start(nTimer);

	while ( nTime < nTimeout )
	{
		if ( curr_desc->flags & SPW_FLAGS_TXD_FT ) 
			break;
		if( curr_desc->flags & 0x7F ) 
			break;

		if( nTimer >= 0 )
			nTime = timer_read( nTimer );
		else 
			nTime++;	
	}
	if( nTimer >= 0 )
		timer_stop( nTimer );

	
	if( curr_desc->flags & 0x7F ) 
		return  - (curr_desc->flags & 0x7F);
	if( nTime >= nTimeout )
		return SPW_ERR_TIMEOUT_FT;


#ifdef _LOG_
	printf("FT set\n");
#endif

	// error ?
	if( curr_desc->flags & 0x7F ) {
		return  -(curr_desc->flags & 0x7F);
	}

	if( nReply )
	{
#ifdef _LOG_
	    printf("Wait reply\n");
#endif
	    nTime = 0;
	    if( nTimer >= 0 )
			timer_start(nTimer);

	    while ( nTime < nTimeout )
	    {

			// error ?
			if( curr_desc->flags & 0x7F ) 
				break;

			if ( curr_desc->flags & SPW_FLAGS_RMAP_REPLY  )  
				break;
				
			if( nTimer >= 0 )
				nTime = timer_read( nTimer );
			else 
				nTime++;	
	    }
	    
	    if( nTimer >= 0 )
	    	timer_stop (nTimer );
	    	
	    if( curr_desc->flags & 0x7F ) 
			return  -(curr_desc->flags & 0x7F);

    	if( nTime >= nTimeout )
			return SPW_ERR_TIMEOUT_REPLY;
	}  

#ifdef _LOG_
	printf("Flags= %x", curr_desc->flags);
#endif

	if( curr_desc->flags & 0x7F ) 
		return  -(curr_desc->flags & 0x7F);
  
	return 0;
}
/////////////////////////////////
int spw_set_invalid_descriptor(int nDescr)
{
	volatile tSPW_DMA_TXD_DESC *curr_desc;

	if( nDescr < 0 || nDescr >= 1024 )
		return SPW_ERR_INVALID_ARG;

	if( !Dma_TXD_Desc )
		return SPW_ERR_INVALID_DESCR;

	curr_desc = Dma_TXD_Desc + nDescr ;

	// to send just 1 descriptor - next must be invalid
	spw_memset ((char *)curr_desc, 0, sizeof(tSPW_DMA_TXD_DESC));

	return 0;	// OK
}
//////////////////////////
// send using nDescr descriptor
// wait finished
int spw_send_rmap_command( int nLogicalAddr, int nTargetAddr, int nKey, int nRmapCmd,
		int nSourceAddr, int nSize, int bPath, int nTimer, int nTimeout, int nDescr )

{
//	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	int nRes;

	nRes = spw_init_descr_header(nDescr, DMA_RMAP_PROTOCOL_ID, nRmapCmd, nTargetAddr,  bPath?1:0, nKey );
	if( nRes < 0 )
		return nRes;

	nRes = spw_make_txd_descr( nDescr, nLogicalAddr, nSourceAddr, nSize, bPath, 0,1,1);
	if( nRes < 0 )
		return nRes;

	spw_set_invalid_descriptor( nDescr < 1023 ? nDescr+1 : 0 );

	spw_start_send(nDescr);		// = TDE

#ifdef _LOG_
	printf("rmap=%x", nRmapCmd);
#endif	
	
	nRes = spw_wait_finished( nDescr, nTimer, nTimeout, nRmapCmd & 8 );
	return nRes ;

}

///////////////////////////////////
// to get not-RMAP packets
int spw_make_rxd_descr(int nDescr, int nAddress, int nMult, int bLast, int bIE )
{
//	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	volatile tSPW_DMA_RXD_DESC *desc ;
	desc = Dma_RXD_Desc;

	if( !desc )
		return SPW_ERR_INVALID_DESCR;

	if( nDescr < 0 || nDescr >= 1024 )
		return SPW_ERR_INVALID_ARG;

	desc += nDescr;

//	memsetw( (int *)desc, sizeof( tSPW_DMA_RXD_DESC ), 0 );

	desc->flags = SPW_FLAGS_RXD_FV;
	if (nMult)
   	   	desc->flags |= SPW_FLAGS_RXD_MP | (nMult & 0x01ffffff);	
	if( bLast )
	   	desc->flags |= SPW_FLAGS_RXD_LD;
	if( bIE )
		desc->flags |= SPW_FLAGS_RXD_IE;

	desc->data_ptr 	= nAddress & 0x1FFFFFFF;

#ifdef _LOG_
	printf("Rxd descr flags = %x", desc->flags);
#endif

	return 0;
}
///////////////////////////
// to get not-RMAP packets
int spw_get_rxd_descr_flags(int nDescr )
{
//	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	volatile tSPW_DMA_RXD_DESC *desc ;
	desc = Dma_RXD_Desc;

	if( !desc )
		return SPW_ERR_INVALID_DESCR;

	if( nDescr < 0 || nDescr >= 1024 )
		return SPW_ERR_INVALID_ARG;

	desc += nDescr;

	return desc->flags;
}
///////////////////////////
int spw_get_rxd_descr_data_ptr(int nDescr )
{
//	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	volatile tSPW_DMA_RXD_DESC *desc ;
	desc = Dma_RXD_Desc;

	if( !desc )
		return SPW_ERR_INVALID_DESCR;

	if( nDescr < 0 || nDescr >= 1024 )
		return SPW_ERR_INVALID_ARG;

	desc += nDescr;

	return desc->data_ptr;
}
//////////////////////////////////////////////////////
int spw_is_ready_get()	// yes/no
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	if( SPW->RX.CONTROL & SPW_DMA_RXD_CTRL_RDE)
		return 1;

	// ready
	return 0;
}
///////////////////
int spw_start_get(int nFirstDescr)	// not RMAP;	SPW->RX.CONTROL |= 1;		//RDE = 1;
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	if( nFirstDescr < 0 || nFirstDescr >= 1024 )
		return SPW_ERR_INVALID_ARG;

	SPW->RX.D_INDEX = nFirstDescr;
	SPW->RX.CONTROL |= SPW_DMA_RXD_CTRL_RDE;

	return 0;
}
/////////////////////////////
/////////////////////////////
//
// return data_len or error
//
int spw_is_rxd_finished( int nDescr, int *pAddr)
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	volatile tSPW_DMA_RXD_DESC *pDesc;
//	int timeout_counter = 0;
	int data_len = 0;

	if( nDescr < 0 || nDescr >= 1024 )
		return SPW_ERR_INVALID_ARG;

	if( !Dma_RXD_Desc )
		return SPW_ERR_INVALID_ARG;

	pDesc = Dma_RXD_Desc + nDescr;

	// error ?
	if ( pDesc->flags & (1<<30 ) )
	{
			//printf("error rxd");
		return SPW_ERR_RXD;
			
	}

	// FV not clear yet
	if( !(pDesc->flags & (1<<31 )) )
		return 0;

	if ( SPW->RX.CONTROL & 1 )
		return 0;

	if( pAddr )
		*pAddr = pDesc->data_ptr;

	data_len = pDesc->flags & 0x03ffffff;
#ifdef _LOG_
	printf("Len = %d", data_len);
#endif

	return data_len;
}
/////////////////////////////////////

//
// return data_len or error
//
int spw_wait_rxd_finished( int nDescr, int nTimer, int nTimeout, int *pAddr)
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	volatile tSPW_DMA_RXD_DESC *pDesc;
	int timeout_counter = 0;
	int data_len = 0;

	if( nDescr < 0 || nDescr >= 1024 )
		return SPW_ERR_INVALID_ARG;

	if( !Dma_RXD_Desc )
		return SPW_ERR_INVALID_ARG;

	pDesc = Dma_RXD_Desc + nDescr;

#ifdef _LOG_
	printf("flags=%x", pDesc->flags );
#endif

	if( nTimer >= 0 )
		timer_start( nTimer);
	timeout_counter = 0;
		
	while( pDesc->flags & (1<<31 ) )
	{
		if( timeout_counter > nTimeout )
		{
			if( nTimer >= 0 )
				timer_stop(nTimer);
			return SPW_ERR_TIMEOUT_RXD;
		}
		
	
		if ( pDesc->flags & (1<<30 ) )
		{
			if( nTimer >= 0 )
				timer_stop(nTimer);	
			//printf("error rxd");
			return SPW_ERR_RXD;
		}
		if( nTimer >= 0 )
			timeout_counter = timer_read( nTimer );
		else	
			timeout_counter++;
	}

	if( nTimer >= 0 )
			timer_stop(nTimer);
	timeout_counter = 0;
	if( nTimer >= 0 )
			timer_start(nTimer);
	while( SPW->RX.CONTROL & 1 )
	{
		if( nTimer >= 0 )
			timeout_counter = timer_read( nTimer );
		else	
			timeout_counter++;
		if( timeout_counter > nTimeout )
		{
			if( nTimer >= 0 )
				timer_stop(nTimer);
			return SPW_ERR_TIMEOUT_RXD;
		}
	}

	if( nTimer >= 0 )
			timer_stop(nTimer);
#ifdef _LOG_
	printf("flags2=%x", pDesc->flags );
#endif

	if( pAddr )
		*pAddr = pDesc->data_ptr;

	data_len = pDesc->flags & 0x03ffffff;
#ifdef _LOG_
	printf("Len = %d", data_len);
#endif

	return data_len;
}
/////////////////////////////////////
// buffer input 
int spw_set_buffer(int nMaxData, int nStartBuffer1, int nStartBuffer2, int nCurBuffer1, int nCurBuffer2)
{
// Listopadov - disabled this eight ports check
//#if SPW_PORTS_NUM != 8
//	return 0;
//#else
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	
	SPW->RX.MAX_DATA = nMaxData;
	SPW->RX.BufStartAddr1 = nStartBuffer1;
	SPW->RX.BufStartAddr2 = nStartBuffer2;
	SPW->RX.BufCurAddr1 = nCurBuffer1;
	SPW->RX.BufCurAddr2 = nCurBuffer2;

	return 0;
//#endif
}
///////////////////////////
int spw_get_buffer(int *pnCurDescr, int *pnMaxData, int *pnStartBuffer1, int *pnStartBuffer2, int *pnCurBuffer1, int *pnCurBuffer2)
{
// Listopadov - disabled this eight ports check
//#if SPW_PORTS_NUM != 8
//	return 0;
//#else
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	
	*pnCurDescr = SPW->RX.D_INDEX;
	*pnMaxData = SPW->RX.MAX_DATA ;
	*pnStartBuffer1 = SPW->RX.BufStartAddr1;
	*pnStartBuffer2 = SPW->RX.BufStartAddr2;
	*pnCurBuffer1 = SPW->RX.BufCurAddr1;
	*pnCurBuffer2 = SPW->RX.BufCurAddr2;

	return 0;
//#endif
}
///////////////////////////
// what buffer is ready
// 1 - first
// 2 - second
// 3 - both
// 0 - no one 
int spw_is_ready_get_buffer()	// 
{
// Listopadov - disabled this eight ports check
//#if SPW_PORTS_NUM != 8
//	return 0;
//#else
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	if( SPW->RX.CONTROL & SPW_RXD_CTRL_RBE1)
	{
		if( SPW->RX.CONTROL & SPW_RXD_CTRL_RBE2)
			return 3;
		return 1;
	}

	if( SPW->RX.CONTROL & SPW_RXD_CTRL_RBE2)
		return 2;

	// not ready
	return 0;
//#endif
}
///////////////////
int spw_start_get_buffer(int nFirstDescr, int nBuffer1, int nBuffer2)	// not RMAP; buffer	
{
// Listopadov - disabled this eight ports check
//#if SPW_PORTS_NUM != 8
//	return 0;
//#else

	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	if( nFirstDescr < 0 || nFirstDescr >= 1024 )
		return SPW_ERR_INVALID_ARG;

	if( !nBuffer1 && !nBuffer2 )
		return SPW_ERR_INVALID_ARG;

	SPW->RX.D_INDEX = nFirstDescr;
	SPW->RX.CONTROL &= ~SPW_DMA_RXD_CTRL_RDE;

	if( nBuffer1 )
		SPW->RX.CONTROL |= SPW_RXD_CTRL_RBE1;
	else
		SPW->RX.CONTROL &= ~SPW_RXD_CTRL_RBE1;
	if( nBuffer2 )
		SPW->RX.CONTROL |= SPW_RXD_CTRL_RBE2;
	else
		SPW->RX.CONTROL &= ~SPW_RXD_CTRL_RBE2;

	return 0;
//#endif
}
/////////////////////////////
// interrupts
int spw_get_tx_int_req(void )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	return SPW->TX.INT_REQ ;
}
////////////////////
int spw_set_tx_int_req( int nValue )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	SPW->TX.INT_REQ = nValue;
	return SPW->TX.INT_REQ;
}
////////////////////

// return old value
int spw_set_tx_int_enable( int bEnable )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	int nOld =  SPW->TX.INT_EN ;

	if( bEnable )
		SPW->TX.INT_EN = 1;
	else
		SPW->TX.INT_EN = 0;

	return nOld;
}
////////////////////////
int spw_get_tx_int_enable( void )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	return  SPW->TX.INT_EN ;
}
////////////////
int spw_get_rx_int_req(void )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	return SPW->RX.INT_REQ ;
}
////////////////////
int spw_set_rx_int_req( int nValue )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	int nOld = SPW->RX.INT_REQ;
	
	SPW->RX.INT_REQ = nValue;
	return nOld;
}
////////////////
// return old value
int spw_set_rx_int_enable( int nValue )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	int nOld =  SPW->RX.INT_EN ;

	SPW->RX.INT_EN = nValue;

	return nOld;
}
////////////////////////

int spw_get_rx_int_enable( void )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	return SPW->RX.INT_EN ;
}
////////////////////////
int spw_get_rx_int_info(void )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	return SPW->RX.INT_INFO ;
}
////////////////////
int spw_set_rx_int_info( int nValue )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	int nOld = SPW->RX.INT_INFO;
	
	SPW->RX.INT_INFO = nValue;
	return nOld;
}
////////////////
int spw_get_rx_busy(void )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	return SPW->RX.BUSY ;
}
////////////////////
int spw_set_rx_busy( int nValue )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	int nOld = SPW->RX.BUSY;
	
	SPW->RX.BUSY = nValue;
	return nOld;
}
////////////////

// port interrupts
int spw_get_port_intpend(int nPort )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	volatile SPW_PORT_UNIT *spw_reg;

	int NumPort = spw_get_nport();

	if( nPort < 0 || nPort > NumPort-1)
		return SPW_ERR_INVALID_ARG;

	if( nPort < 4 )
		spw_reg = &SPW->LPORT[nPort];	
	else
		spw_reg = &SPW->HPORT[nPort-4];	

	return spw_reg->INTPEND ;
}
////////////////////
int spw_set_port_intpend(int nPort, int nValue )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	volatile SPW_PORT_UNIT *spw_reg;

	int NumPort = spw_get_nport();

	if( nPort < 0 || nPort > NumPort-1)
		return SPW_ERR_INVALID_ARG;

	if( nPort < 4 )
		spw_reg = &SPW->LPORT[nPort];	
	else
		spw_reg = &SPW->HPORT[nPort-4];	

	spw_reg->INTPEND = nValue;
	return spw_reg->INTPEND ;
}
////////////////////

int spw_get_port_int_en(int nPort )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	volatile SPW_PORT_UNIT *spw_reg;

	int NumPort = spw_get_nport();

	if( nPort < 0 || nPort > NumPort-1)
		return SPW_ERR_INVALID_ARG;

	if( nPort < 4 )
		spw_reg = &SPW->LPORT[nPort];	
	else
		spw_reg = &SPW->HPORT[nPort-4];	

	return spw_reg->INT_EN ;
}
////////////////////
int spw_set_port_int_en(int nPort, int nValue )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	volatile SPW_PORT_UNIT *spw_reg;

	int NumPort = spw_get_nport();

	if( nPort < 0 || nPort > NumPort-1)
		return SPW_ERR_INVALID_ARG;

	if( nPort < 4 )
		spw_reg = &SPW->LPORT[nPort];	
	else
		spw_reg = &SPW->HPORT[nPort-4];	

	spw_reg->INT_EN = nValue;
	return spw_reg->INT_EN ;
}
////////////////////
// protocol interrupt
// what ports interrupts enabled - send to interruot controller
int spw_set_prot_int_enable( int nValue )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	int nOld =  SPW->PROT.INT_EN ;

	SPW->PROT.INT_EN = nValue;

	return nOld;
}
////////////////////////
int spw_get_prot_int_enable( void )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	return  SPW->PROT.INT_EN ;
}
////////////////
int spw_get_prot_int_req(void )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	return SPW->PROT.INT_REQ ;
}
////////////////////
// port0 interrupts
int spw_set_prot_port0_int_enable( int nValue )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	int nOld =  SPW->PROT.INT_PORT_EN ;

	SPW->PROT.INT_PORT_EN = nValue;

	return nOld;
}
////////////////////////
int spw_get_prot_port0_int_enable( void )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	return  SPW->PROT.INT_PORT_EN ;
}
////////////////
int spw_get_prot_port0_int_req(void )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	return SPW->PROT.INT_PORT ;
}
////////////////////
// router interrupts
int spw_get_node_intpend( )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	return SPW->NODE.INTPEND ;
}
////////////////////
int spw_set_node_intpend( int nValue )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	SPW->NODE.INTPEND = nValue;
	return SPW->NODE.INTPEND ;
}
////////////////////

int spw_get_node_int_en( )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	return SPW->NODE.INT_EN ;
}
////////////////////
int spw_set_node_int_en( int nValue )
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	SPW->NODE.INT_EN = nValue;
	return SPW->NODE.INT_EN ;
}
////////////////////


////////////////

int spw_block_send( int tla, int cmd, int target_addr, int path, int data, int mask, int data_size, int key, int nTimer, int nTimeout )
{ 
	int head = 0;
	int timeout = 0;
	volatile SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	if( data_size < 0 || data_size > 4 )	// > 4
		return SPW_ERR_INVALID_ARG;

	SPW->TX.STATUS &= ~SPW_BLOCK_BUSY_TX;	

#if 0
	while ( Spw_Block_Send->status & SPW_BLOCK_BUSY_TX )
	{
		if( ++timeout > 0x2000 )	{	//TXD_DMA_COMPLETION_TIMEOUT ) { 
			*err_code = 4;	//TXD_DMA_COMPLETION_TIMEOUT_ERROR;	
			Spw_Block_Send->status &= ~SPW_BLOCK_BUSY_TX;	
			return 1;
		}

		if( (timeout & 0xfff) == 0 ) 	
		{
			printf( " wait block ="); ton( timeout );printf("\n");
		}

	}
#endif

	SPW->TX.TARG_ADDR = target_addr  & 0x1FFFFFFF;
	SPW->TX.DATA = data;
	SPW->TX.MASK = mask;		

//	head = (tla<<24) | (cmd<<16) | (key<<8) | (data_size<<4);
	head = (tla<<24) | ((cmd + 0x10)<<18) | (key<<8) | (data_size<<4);
	if( path )
		head |= 1<<7;

#ifdef _LOG_
	printf(" status=%x\n", SPW->TX.STATUS);
	printf("head=%x\n", head);
	printf("data=%x\n", data);
	printf("addr=%x\n", SPW->TX.TARG_ADDR);
#endif

	// experiment
	//Spw_Block_Send->status |= SPW_BLOCK_INT;		

	// start send 
	SPW->TX.HEAD = head;

	// start send data - need ?
	//Dma_TXD_Ctrl_Regs->txd_ctrl = SPW_DMA_TXD_CTRL_TDE;

	
	// wait reply
	timeout = 0;
	if( nTimer >= 0 )
		timer_start( nTimer);

	while ( SPW->TX.STATUS & SPW_BLOCK_BUSY_TX )
	{
		if( timeout > nTimeout ) { 		// 0x40000	TXD_DMA_COMPLETION_TIMEOUT
			if( nTimer >= 0 )
				timer_stop(nTimer);
			SPW->TX.STATUS &= ~SPW_BLOCK_BUSY_TX;	
			return SPW_ERR_TIMEOUT_BLOCK;
		}
		
		if( nTimer >= 0 )
			timeout = timer_read( nTimer );
		else	
			timeout++;


#ifdef _LOG_
		if( (timeout & 0xfff) == 0 ) 	
		{
			printf(" status =%x\n", SPW->TX.STATUS); 	
			printf( " wait block time =%x\n", timeout );
		}
#endif
	}

	if( nTimer >= 0 )
		timer_stop(nTimer);
	
#ifdef _LOG_
	printf( "Block send done\n");	
	printf(" status =%x\n", SPW->TX.STATUS);
	printf(" data=%x\n", SPW->TX.DATA);
#endif

	return 0;
}

////////////////////
int spw_block_get_data()
{
	volatile SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	return SPW->TX.DATA;
}
////////////////
int spw_block_get_status()
{
	volatile SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	return SPW->TX.STATUS;
}
////////////////
// burst 
int spw_get_no_burst(int *pnAddr1, int *pnMask1, int *pnAddr2, int *pnMask2)
{
	volatile SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	*pnAddr1 = SPW->PROT.NO_BURST_ADDR1;
	*pnMask1 = SPW->PROT.NO_BURST_MASK1;
	*pnAddr2 = SPW->PROT.NO_BURST_ADDR2;
	*pnMask2 = SPW->PROT.NO_BURST_MASK2;
	
	return 0;
}
//////////////////////
int spw_set_no_burst(int nAddr1, int nMask1, int nAddr2, int nMask2)
{
	volatile SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	SPW->PROT.NO_BURST_ADDR1 = nAddr1 ;
	SPW->PROT.NO_BURST_MASK1 = nMask1 ;
	SPW->PROT.NO_BURST_ADDR2 = nAddr2 ;
	SPW->PROT.NO_BURST_MASK2 = nMask2 ;

	return 0;
}
//////////////////////
int spw_is_no_burst( int nAddr )
{
	volatile SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	
	unsigned int nAddrB = SPW->PROT.NO_BURST_ADDR1;
	unsigned int nMaskB = SPW->PROT.NO_BURST_MASK1;
	
	if( ((nAddr & (~nMaskB)) >> 16) == (nAddrB>>16) )
		return 1;	// no burst - area 1
		
	nAddrB = SPW->PROT.NO_BURST_ADDR2;
	nMaskB = SPW->PROT.NO_BURST_MASK2;

	if( ((nAddr & (~nMaskB)) >> 16) == (nAddrB>>16) )
		return 2;	// no burst - area 2
	
	return 0;
}
/////////////////////////
// timecodes
// get register value
int spw_timecode_get()
{
	volatile SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	return SPW->NODE.TIMECODE;
}
////////////
// allow send timecodes
int spw_timecode_set_master(int nMaster, int nTimer)
{
	volatile SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	
	if( nMaster )
	{
		SPW->NODE.TIMECODE = (1<<17);

		if( nTimer == 0 )
			SPW->NODE.TIMECODE |= (1<<26);		// don't use timer 
	}
	else
		SPW->NODE.TIMECODE = 0;
	return 0;
}
///////////////////
// allow get timecodes from some ports
int spw_timecode_set_slave(int nPortMask)
{
	volatile SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	//SPW->NODE.TIMECODE = 0;
		//spw_reg->TIMECODE_RX_EN = TCODE_TIME_RX_EN1; 
	SPW->NODE.TIMECODE_RX_EN = nPortMask;
	return 0;
}
///////////////////

// increment value to send
int spw_timecode_inc_count()
{
	volatile SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	SPW->NODE.TIMECODE |= (1<<16);
	return 0;
}
///////////////////

// set value to send
int spw_timecode_set_count(int nValue)
{
	volatile SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	
	nValue &= 0x3f;
	// remove old value
	SPW->NODE.TIMECODE &= ~(0x3f<<8);
	// set new
	SPW->NODE.TIMECODE |= nValue << 8 ;
	return 0; 
}
///////////////////

// get current value
int spw_timecode_get_count()
{
	volatile SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	return (SPW->NODE.TIMECODE & 0x3f);
}
///////////////////

// sequentional 
int spw_set_seq_first_last(int nFirst, int nLast)
{
	volatile SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	int nPort = spw_get_nport();

	if( nFirst < 1 || nFirst > nPort )
		return SPW_ERR_INVALID_ARG;
	if( nLast < 1 || nLast > nPort )
		return SPW_ERR_INVALID_ARG;

	SPW->NET.SEQ_FIRST = 1<<nFirst;
	SPW->NET.SEQ_LAST = 1<<nLast;

	return 0;
}
////////////////
int spw_get_seq_first_last(int *pnFirst, int *pnLast)
{
	volatile SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	int nPort =	spw_get_nport();
	int i;

	if( pnFirst )
	{
		*pnFirst = 0;
		for(i=1;i<=nPort;i++)
		{
			if( SPW->NET.SEQ_FIRST & (1<<i) )
			{
				*pnFirst = i;
				break;
			}
		}
	}

	if( pnLast )
	{
		*pnLast = 0;
		for(i=1;i<=nPort;i++)
		{
			if( SPW->NET.SEQ_LAST & (1<<i) )
			{
				*pnLast = i;
				break;
			}
		}
	}

	return 0;
}
////////////////
int spw_set_seq_mode()
{
	int testData[32];
	volatile SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	SPW->NET.SWCONTROL |= (1<<31);
	testData[0] = SPW->NODE.RXD ;
	testData[1] = SPW->NODE.RXD ;
	testData[2] = SPW->NODE.RXD ;
	testData[3] = SPW->NODE.RXD ;
	testData[4] = SPW->NODE.RXD ;
	SPW->NODE.TXD = 0x01;
	SPW->NODE.TXD = 0x00;
	SPW->NODE.TXD = 0x02;
	SPW->NODE.TXD = 0xaa;
	SPW->NODE.TXD = 0x140;
	testData[0] = SPW->NODE.RXD ;
	testData[1] = SPW->NODE.RXD ;
	testData[2] = SPW->NODE.RXD ;
	testData[3] = SPW->NODE.RXD ;
	testData[4] = SPW->NODE.RXD ;
	return 0;
}
////////////////
// reset arbiter
int spw_arb_reset()
{
	volatile SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	SPW->NET.SWCONTROL |= (1<<6);
	return 0;
}
////////////////


// advanced 
int spw_get_address(void)
{
	return SPW_BASE;
}
/////////////////////////////
////////////////////////
// send RMAP command to test connected module is ready
// 
////////////////////////
// return error code < 0
// 0 if OK
// 1 - if mark not set
int spw_test_mark(  int work_addr, int target_addr, int nPath, int nTimer, int nTimeout, int nAddr, int nMark, int nKey )
{
	int i;
	int *pInfo;
	char *pError = 0;
	int err_code = 0;

	// Prepare data
	//*(int *)RMW_CMD_BUFF = 0;							// data
	//*(int *)(RMW_CMD_BUFF+sizeof(int)) = 0xffffffff;	// mask
	pInfo = (int *)work_addr;
	*pInfo = 0;

	err_code = spw_send_rmap_command( target_addr, // TARGET LOGICAL ADDRESS
		nAddr , // TARGET ADDRESS
		nKey,			// TARGET KEY
		RMAP_READ_INC_ADDR_CMD, // command
//		RMAP_WRITE_SINGLE_ADDR_INC_NO_VERIFY_NO_REPLY_CMD, // command
		(int)pInfo, //int nSourceAddr, 
		4,		//	data size int nSize, 
		nPath,			// path addressing
		nTimer,
		nTimeout,
		0 
		);

	if( err_code < 0 ) // error
	{
#ifdef _LOG_
		printf("error mark = %d\n", err_code); 
		
		pError = spw_get_error_string( err_code );
		if( pError )
			printf (pError );
#endif
		return err_code;
	}

	i = pInfo[0] ;
#ifdef _LOG_
	printf("i=%x", i);
#endif

	if( i!= nMark )	
	{
#ifdef _LOG_
		printf(  "Not ready...\n" );
#endif
		return 1;
	}

	return 0;
}
////////////////////////
//
// test 4 bytes at address nAddr in connected module with target_logical_address - must be equal to nMark
// use path addressing (nPath > 0) or logical
// not more nLoopConnect, every loop nTimeout
// delay between loops
// work_addr - working address at source
//
// return 0 - OK
// else error
// 
int spw_test_ready( int work_addr, int target_log_addr, int nPath, int nTimer, int nTimeout, int nLoopConnect, int nConnectDelay, int nAddr, int nMark, int nKey  )
{
	int err_code = 0;
	int i, j;
	int timeout_counter = 0;

	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;

	// no sending ?
	SPW->TX.CONTROL = 0; 
	
//	printf(" Trying to connect... \n\r");
	for( i=0; i < nLoopConnect; i++ )
	{
		timeout_counter = 0;
		err_code = 0;

		// ready to send ?
		if( nTimer >= 0 )
			timer_start(nTimer);
		while( spw_is_ready_send() )
		{
			if( nTimer >= 0 )
				timeout_counter = timer_read( nTimer );
			else	
				timeout_counter++;
			if( timeout_counter > nTimeout )
			{
				if( nTimer >= 0 )
					timer_stop( nTimer );
				err_code = SPW_ERR_TIMEOUT_SEND_READY;
				return err_code;
			}
		}
		if( nTimer >= 0 )
			timer_stop( nTimer );
			
//		printf( " time "); ton(timeout_counter);

		// send RMAP command to test second module is ready

		err_code = spw_test_mark( work_addr, target_log_addr, nPath, nTimer, nTimeout, nAddr, nMark, nKey );
		if( err_code )
		{
#ifdef _LOG_
			printf("Connect Loop %x", i);

			if( err_code < 0 )
			{
				printf(" Error %d", err_code);
			}
#endif
		}
		else
		{
#ifdef _LOG_
			printf(" Success connect\n"); 
#endif
			break;
		}

		// delay
		if( nTimer >= 0 )
			timer_start( nTimer);
		for(j=0; j < nConnectDelay; )
		{
			if( nTimer >= 0 )
				j = timer_read( nTimer );
			else	
				j++;
		}
		
		if( nTimer >= 0 )
			timer_stop( nTimer );

	}

	if( err_code > 0 )
		err_code = SPW_ERR_CONNECTED_NOT_READY;

	return err_code;
}
///////////////////////
// standard initialize 
// return < 0 - error 
//	else returns nPortMask (active ports)
//	
int spw_init_standard(int bUsePll, int bWaitAllPorts, 
				int nLogicAddr, 
				int nTxDescrArea,  int nTxDescrHead, int nRxDescrArea, 
				int nTAarea, int nIAarea,
				int nTimer, int nTimeout)
{
	int nPort;
	int i,nNotReady;
	int nPortMask = 0;
	int timeout_counter = 0;

	// how many ports
	nPort = spw_get_nport();
#ifdef _LOG_	
	printf("nPort = %x", nPort);
#endif

// pll on first module only - Amosov 24.12.2014
	if( bUsePll )
	{
		for(i=0;i<nPort;i++)
		{
			spw_init_port( i );
		}
	}

	timeout_counter = 0;
	nPortMask = 0;
	
	if( nTimer >= 0 )
		timer_start(nTimer);
		
	for(;;)
	{
		nPortMask = 0;
		nNotReady = 0;
		for(i=0;i<nPort;i++)
		{
			if( spw_get_port_status(i) == STATUS_FSM_RUN )
				nPortMask |= 1<<(i+1);
			else
				nNotReady++;
		}
		
		if( nNotReady== 0 )	// all ready
			break;
		if( !bWaitAllPorts	&& nPortMask )
			break;
			
		if( nTimer >= 0 )
			timeout_counter = timer_read( nTimer);
		else		
			timeout_counter++;
			
		if( timeout_counter >= nTimeout )	
			break;
	}
	
	if( nTimer >= 0 )
		timer_stop( nTimer );

	// what ports are ready
#ifdef _LOG_	
	printf("nPortMask =%x", nPortMask); 
	printf(" time=%x", timeout_counter); 
#endif

	if( timeout_counter >= nTimeout )
	{
#ifdef _LOG_	
		printf("Error: link timeout");
#endif		
		return SPW_ERR_TIMEOUT_LINK;
	}


 	// set own parameters - logical address, own port 0
	spw_set_own_logical( nLogicAddr );
	spw_enable_logical ( 1 );	// enable

	// descriptors area
	if( nTxDescrArea )
		spw_set_txd_descr_area( nTxDescrArea  );
	if( nTxDescrHead )
		spw_set_txd_header_area( nTxDescrHead  );

	if( nRxDescrArea )
		spw_set_rxd_descr_area( nRxDescrArea  );
	
	// pathes area	
	if( nTAarea )
		spw_set_ta_base( nTAarea );
	if( nIAarea )	
		spw_set_ia_base( nIAarea );

	// rmap areas, maximal data size
	spw_set_rmap( 0, 0xffffffff, 0, 0xffffffff, 0x100000);

	return nPortMask;
}
/////////////////
// set some default parameters		
void spw_set_defaults()
{
	SPW_UNIT *SPW = (SPW_UNIT *)SPW_BASE;
	int nPort = spw_get_nport();
	int i;

	for(i=0;i<nPort;i++)
		spw_enable_port(i);

	// own logical address
//	spw_set_own_logical( 0 );

	// disable logical
//	spw_enable_logical(0);

	// key and more
	SPW->RX.CONTROL = 0;

	// rmap areas, maximal data size
	spw_set_rmap( 0, 0xffffffff, 0, 0xffffffff, 0x100000);

}
//////////////////


#define MAX_ERROR	17


// RMAP error codes 0-13
static char *arrRmapErrors[MAX_ERROR] =
{
	"NO_ERROR",	//0
	"RMAP General error",
	"RMAP Unused command",
	"RMAP Wrong key",
	"RMAP CRC error",	//4
	"RMAP Wrong EOP",
	"RMAP Data overflow",
	"RMAP EOP error",
	"Reserved",
	"RMAP DataCheck overflow",
	"RMAP Wrong data address or invalid command",	// 10
	"RMAP Wrong RMW data size",
	"RMAP Invalid logical address",
	"Reserved",			// 13
	"Reserved",			// 14
	"Reserved"			// 15
	"Reserved",			// 16
};

static char *arrErrors[MAX_ERROR] =
{
	"Reserved",			// 0
	"SPW_ERR_MEMORY",					
	"SPW_ERR_INVALID_ARG",				
	"SPW_ERR_INVALID_DESCR",		
	"Timeout - FV",
	"Timeout - FT",
	"TXD_RMAP_REPLY_NOT_RECEIVED_TIMEOUT",
	"Timeout - RXD FV",			// 7
	"RXD get error",
	"TA table not set",
	"IA table not set",
	"Unknown SPW ID",
	"Connected module not ready",
	"Timeout - Block Send",			// 13
	"Timeout - link",			// 14
	"Timeout - Send Not Ready"			// 15
	"RXD data CRC error"		// 16
};

char *spw_get_error_string( int nError )
{
	nError = -nError;
	if( nError <= MAX_ERROR)
		return arrRmapErrors[nError];
	if( nError >= 20 &&  nError < 20 + MAX_ERROR )
		return arrErrors[nError-20];

	return "Unknown error";
}
///////////////////
