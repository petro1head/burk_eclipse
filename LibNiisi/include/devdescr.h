#ifndef _DEVDESCR_
#define _DEVDESCR_

#define uint32 unsigned int
#define uint16 unsigned short
#define uint8  unsigned char
#define vuint32 volatile unsigned int
#define vuint16 volatile unsigned short
#define vuint8  volatile unsigned char


#ifdef VM014
	#include "devdescr_vm014.h"
#endif


#ifdef VM044
	#include "devdescr_vm044.h"
#endif


#ifdef VM056
	#include "devdescr_vm056.h"
#endif

	
	
#endif
