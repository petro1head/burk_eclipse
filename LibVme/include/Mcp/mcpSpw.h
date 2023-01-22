/*
 * mpvvSpw.h
 *
 *  Created on: 17 мая 2021 г.
 *      Author: k
 */

#ifndef LIBVME_MCP_MCPSPW_H_
#define LIBVME_MCP_MCPSPW_H_

#include "spw_lib.h"

#define MCP_HOST_ADDR_MCP 0x33
#define Mcp_SPW_TIMEOUT 100000

#define HOST_ADDR 0x30
#define LOGICAL_MEM_BASE	0xa0000000
#define logicalMpvv1 0x31
#define logicalMpvv2 0x32

extern const char pathMppv1[2];
extern const char pathMppv1Length;
extern const char pathMppv2[3];
extern const char pathMppv2Length ;
extern const char backPathMppv2[4];
extern const char backPathMppv1[4];

typedef struct {
	unsigned char reserved:1;
	unsigned char command:1;//0=responce
	unsigned char write:1;//0=read
	unsigned char verify:1;//0=no verify
	unsigned char acknowledge:1;// 0=no ack
	unsigned char increment:1;// 0 =no incr
	unsigned char sourcePathLength:2;
} __attribute__ ((__packed__))  rmapCommand;

int mcpSpwInitPort(int iPort);
int mcpSpwInit();
int mcpInitMemoryController();
int mcpRmapSend (int addressLogical, unsigned int addressData, unsigned char* data, int dataLength);
int mcpRmapRead (int addressLogical, unsigned int addressData, unsigned char* data, int dataLength);

int mcpSendTestMessage();
int mcpNiisiTest();

#endif /* LIBVME_MCP_MCPSPW_H_ */
