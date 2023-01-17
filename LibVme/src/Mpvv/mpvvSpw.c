/*
 * mpvvSpw.c
 *
 *  Created on: 17 мая 2021 г.
 *      Author: Dmitry
 */
#include "mpvvSpw.h"

#define MPVV_SPW_TIMEOUT 100000


int mpvvSpwInitPort(int iPort)
{



	spw_init_port( iPort );
	if( (spw_get_port_status(iPort) & STATUS_FSM_RUN ) != STATUS_FSM_RUN )
		spw_reset_port(iPort);
	int timeout_counter = 1;


		while ( timeout_counter < MPVV_SPW_TIMEOUT )
		{

				if( (spw_get_port_status(iPort) & STATUS_FSM_RUN ) == STATUS_FSM_RUN )
				{
					break;
				}

		}

		// set parameters
		spw_set_own_logical( MPVV_HOST_ADDR_MPVVV1 );
		spw_enable_logical ( 1 );	// enable
}


int mpvvSpwInit()
{
	spw_set_defaults();
	mpvvSpwInitPort(0);
}
