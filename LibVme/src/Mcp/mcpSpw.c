/*
 * mpvvSpw.c
 *
 *  Created on: 17 ��� 2021 �.
 *      Author: k
 */
#include "mcpSpw.h"
//#include
#define Mcp_SPW_TIMEOUT 100000


//const char pathMppv1[2] = { 2 , 0 };
//const char pathMppv1Length = 2;
//const char pathMppv2[3] = { 2 , 1 , 0 };
//const char pathMppv2Length = 3;
//const char backPathMppv1[4] = { 0, 0 , 2 , 0 };
//const char backPathMppv2[4] = { 0, 1 , 2 , 0 };


//FOR MCP TO MPVV
const char pathMppv1[2] = { 2 , 0 };
const char pathMppv1Length = 2;
const char pathMppv2[3] = { 1 , 2 , 0 };
const char pathMppv2Length = 3;
const char backPathMppv1[4] = { 0, 0 , 2 , 0 };
const char backPathMppv2[4] = { 0, 2 , 1 , 0 };


//FOR MCP TO MPVV
//const char pathMppv1[2] = { 2 , 0 };
//const char pathMppv1Length = 1;
//const char pathMppv2[3] = { 1 , 0 , 0 };
//const char pathMppv2Length = 1;
//const char backPathMppv1[4] = { 0, 0 , 0 , 1 };
//const char backPathMppv2[4] = { 0, 0 , 0 , 1 };

// our working buffers
char SPW_read_area[1024] 	__attribute__ ((aligned(32)));
char SPW_write_area[1024]	__attribute__ ((aligned(32)));

char SPW_TXD_TA_BASE[16*256] __attribute__ ((aligned(32)));
char SPW_TXD_IA_BASE[16*256] __attribute__ ((aligned(32)));

char SPW_RXD_BUFFER_BASE[256]	__attribute__ ((aligned(32)));
char SPW_RXD_BUFFER_BASE2[256]	__attribute__ ((aligned(32)));

int SPW_TXD_DESC_BASE[16*1024]	__attribute__ ((aligned(32)));	// descriptors are here
int SPW_RXD_DESC_BASE[8*1024]	__attribute__ ((aligned(32)));	// descriptors are here

int SPW_TXD_HDR_BASE[8*1024]	__attribute__ ((aligned(32)));	// headers are here

#define TIMEOUT_MAX		0x400
char testSpw [256];
char testSpw2 [256];



int mcpSpwInitPort(int iPort)
{
	int nPort=0;
	int i=0;
	int timeout_counter=0;
	int nPortMask=0;

	spw_set_defaults();

	// how many ports
	nPort = spw_get_nport();
	for (i = 0;i<10;i++)
		testSpw2[i]=0;

	for(i=0;i<nPort;i++)
	{
		spw_init_port( i );
		if( (spw_get_port_status(i) & STATUS_FSM_RUN ) != STATUS_FSM_RUN )
			spw_reset_port(i );
	}

	while ( timeout_counter < TIMEOUT_MAX )
	{
		// anybody ready ?
		i=1;
		timeout_counter++;
		if( (spw_get_port_status(i) & STATUS_FSM_RUN ) == STATUS_FSM_RUN ){
			nPortMask |= 1<<(i+1);
			if( testSpw2[0] == 0 )
				testSpw2[0] = i+1;
		}
		if( nPortMask )
			break;
	}
	// what ports are ready
	if( timeout_counter >= TIMEOUT_MAX )
	{
		tos("link timeout");
		//return 1;
	}

	// set parameters
	spw_set_own_logical( HOST_ADDR );
	spw_enable_logical ( 1 );	// enable


	// descriptors area
	// non-cache !
	spw_set_txd_descr_area( (int)SPW_TXD_DESC_BASE | 0xa0000000 );

	spw_set_txd_header_area( (int)SPW_TXD_HDR_BASE | 0xa0000000 );
	spw_set_rxd_descr_area( (int)SPW_RXD_DESC_BASE | 0xa0000000 );

	// path area
	spw_set_ta_base( (int)SPW_TXD_TA_BASE );
	spw_set_ia_base( (int)SPW_TXD_IA_BASE );

	// set path, set back path
	spw_set_path_logical(HOST_ADDR, 2, testSpw2);

	// back path
	testSpw2[2] = testSpw2[0];
	testSpw2[0] = 0;
	// back path length could be 4,8,.. - filled with 0s
	spw_set_backpath_logical(HOST_ADDR, 4, testSpw2);

	spw_set_tx_int_enable( 1 );


}


int mcpSpwInit()
{
//	spw_set_defaults();
	mcpSpwInitPort(0);
	spw_set_path_logical(logicalMpvv1,2,pathMppv1);
	spw_set_path_logical(logicalMpvv2,3,pathMppv2);
	spw_set_backpath_logical(logicalMpvv1,4,backPathMppv1);
	spw_set_backpath_logical(logicalMpvv2,4,backPathMppv2);

}


int mcpSendTestMessage()
{
	int ret;
	int i = 0;
	testSpw2[0]  = 0xaa ;
	testSpw2[1]  = 0xbb ;
	testSpw2[2]  = 0xcc ;
	testSpw2[3]  = 0xdd ;
	char *pCharRead = (char *)SPW_read_area;

	i = spw_get_rx_int_req( );
	spw_set_rx_int_req( i );
	i = spw_get_rx_int_req( );
	spw_set_rx_int_enable( 0 );

	// create descriptor
	// prepare to get
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
	spw_make_rxd_descr( 0, 	// descriptor number 0
		(int)SPW_write_area,	//pCharWrite, // area
		0, 		// not multi
		1,		// last
		0);		// IE

// start get data
	ret = spw_is_ready_get();
	spw_start_get(0);

	for(i=0;i<12;i++)
		pCharRead[i] = testSpw2[i];

	return	spw_send_rmap_command(HOST_ADDR,testSpw,0,11,testSpw2,0x0f,1,-1,10000,0);
}


int mcpRmapSend (int addressLogical, unsigned int addressData, unsigned char* data, int dataLength)
{
	int i;
	int ret;
	char *pCharRead = (char *)SPW_read_area;

	rmapCommand command;
	command.acknowledge=1;
	command.command=1;
	command.increment=1;
	command.reserved = 0;
	command.sourcePathLength = 1;
	command.verify = 0;
	command.write = 1;

//	i = spw_get_rx_int_req( );
//	spw_set_rx_int_req( i );
//	i = spw_get_rx_int_req( );
//	spw_set_rx_int_enable( 0 );
//
//	// create descriptor
//	// prepare to get
//	i++;
//	i++;
//	i++;
//	i++;
//	i++;
//	i++;
//	spw_make_rxd_descr( 0, 	// descriptor number 0
//		(int)SPW_write_area,	//pCharWrite, // area
//		0, 		// not multi
//		1,		// last
//		0);		// IE
//
//// start get data
//	ret = spw_is_ready_get();
//	spw_start_get(0);
////
//	for(i=0;i<12;i++)
//		pCharRead[i] = testSpw2[i];


		return	spw_send_rmap_command(addressLogical,addressData,0,*(unsigned char*)&command,data,dataLength,1,-1,100,0);

}


int mcpRmapRead (int addressLogical, unsigned int addressData, unsigned char* data, int dataLength)
{
	int i;
	int ret;
	char *pCharRead = (char *)SPW_read_area;

	rmapCommand command;
	command.acknowledge=1;
	command.command=1;
	command.increment=1;
	command.reserved = 0;
	command.sourcePathLength = 1;
	command.verify = 0;
	command.write = 0;

	i = spw_get_rx_int_req( );
	spw_set_rx_int_req( i );
	i = spw_get_rx_int_req( );
	spw_set_rx_int_enable( 0 );

	// create descriptor
	// prepare to get
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
	spw_make_rxd_descr( 0, 	// descriptor number 0
		(int)SPW_write_area,	//pCharWrite, // area
		0, 		// not multi
		1,		// last
		0);		// IE

// start get data
	ret = spw_is_ready_get();
	spw_start_get(0);
//
//	for(i=0;i<12;i++)
//		pCharRead[i] = testSpw2[i];


		return	spw_send_rmap_command(addressLogical,addressData,0,*(unsigned char*)&command,data,dataLength,1,-1,100,0);

}





int mcpNiisiTest()
{
	char arrTestBuf[256] = "Test lib buffer";

	int i;
	int timeout_counter = 0;
	char *pCharRead = (char *)SPW_read_area;
	char *pCharWrite = (char *)SPW_write_area;
	char *pError = 0;
	int nSize = 12;		// to send
	int nPort = 0;
	char arrPath[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int nRes;
	int nPortMask = 0;
	int nTimeout = 0x100000;
	int nDescr, nBuf1, nBuf2, nCur1, nCur2, nMaxData;
	int nLoop ;
	int bIE;
	int nSave1, nSave2;
	int nError = 0;
	int ret;

//	tos( "\n\n\r------- LIBRARY SPACEWIRE TEST -------\n\n\r" );

	spw_set_defaults();

	// how many ports
	nPort = spw_get_nport();

	for(i=0;i<nPort;i++)
	{
		spw_init_port( i );
		if( (spw_get_port_status(i) & STATUS_FSM_RUN ) != STATUS_FSM_RUN )
			spw_reset_port(i );
	}

	while ( timeout_counter < TIMEOUT_MAX )
	{
		// anybody ready ?
		i=1;
		if( (spw_get_port_status(i) & STATUS_FSM_RUN ) == STATUS_FSM_RUN ){
			nPortMask |= 1<<(i+1);
			if( arrPath[0] == 0 )
				arrPath[0] = i+1;
		}
		if( nPortMask )
			break;
	}
	// what ports are ready
	if( timeout_counter >= TIMEOUT_MAX )
	{
		tos("link timeout");
		return 1;
	}

	// set parameters
	spw_set_own_logical( HOST_ADDR );
	spw_enable_logical ( 1 );	// enable


	// descriptors area
	// non-cache !
	spw_set_txd_descr_area( (int)SPW_TXD_DESC_BASE | 0xa0000000 );

	spw_set_txd_header_area( (int)SPW_TXD_HDR_BASE | 0xa0000000 );
	spw_set_rxd_descr_area( (int)SPW_RXD_DESC_BASE | 0xa0000000 );

	// path area
	spw_set_ta_base( (int)SPW_TXD_TA_BASE );
	spw_set_ia_base( (int)SPW_TXD_IA_BASE );

	// set path, set back path
	spw_set_path_logical(HOST_ADDR, 2, arrPath);

	// back path
	arrPath[2] = arrPath[0];
	arrPath[0] = 0;
	// back path length could be 4,8,.. - filled with 0s
	spw_set_backpath_logical(HOST_ADDR, 4, arrPath);

	spw_set_tx_int_enable( 1 );

	for(nLoop=0; nLoop < 2; nLoop++ )
	{
		// diffferent interrupt settings
		if( nLoop & 1 )
			bIE = 1;
		else
			bIE = 0;

		i = spw_get_rx_int_req( );
		spw_set_rx_int_req( i );
		i = spw_get_rx_int_req( );
		if( nLoop &2 )
			spw_set_rx_int_enable( 0 );
		else
			spw_set_rx_int_enable( 0xF );

		// create descriptor
		// prepare to get
		spw_make_rxd_descr( 0, 	// descriptor number 0
			(int)SPW_write_area,	//pCharWrite, // area
			0, 		// not multi
			1,		// last
			bIE);		// IE

	// start get data
		i = spw_is_ready_get();
		if( i != 0 )
		{
			tos("Error: not ready to get \n\r ");
			return 1;
		}

		spw_start_get(0);


	// prepare data to send
		for(i=0;i<nSize;i++)
			pCharRead[i] = arrTestBuf[i];


		ret = spw_send_rmap_command(HOST_ADDR,testSpw,0,11,arrTestBuf,0x0f,1,1,10000,0);

		i=ret ;
		return 0 ;


	// prepare descriptor header to send data
		nRes = spw_init_descr_header(0, 0, 0, 0, 0, 0 );
		if( nRes < 0 )
		{
			i = nRes;
			goto _EXIT_;
		}

	// prepare descriptor to send data
		nRes = spw_make_txd_descr( 0, 		// descriptor number 0
			HOST_ADDR,
			(int)SPW_read_area,
			nSize, 		// size
			1,	//1, 		// path addressing
			1, 			// last
			nLoop&1,			// FR	0
			1 );		// IE	0

		if( nRes < 0 ) {
			i = nRes;
			goto _EXIT_;
		}

		i = spw_get_tx_int_req( );

		/// send data
		spw_start_send(0);		// start send from 0 txd descriptor

		nRes = spw_wait_finished( 0, -1, nTimeout, 0 );
		if( nRes < 0 )
		{
			i = nRes;
			goto _EXIT_;
		}

		i = spw_get_tx_int_req( );

		// wait get data
		i = spw_wait_rxd_finished( 0, -1, nTimeout, 0 );

		if( i < 0 )
		{
			// error
			goto _EXIT_;
		}

		if( i>=0 && i != nSize )
		{
			tos("Error: invalid data len \n\r");
			return 1;
		}

		i = spw_get_tx_int_req( );
		i = spw_set_tx_int_req( i );

		// look at result
		for( i=0; i < nSize; i++)
		{
			if( pCharWrite[i] != arrTestBuf[i] )
			{
				tos("error compare "); ton10(i);
				return 1;
			}
		}

		// what interruptsa were set
		i = spw_get_rx_int_enable( );

		i = spw_get_rx_int_req( );
	}

	_EXIT_:
	return 0;
}
