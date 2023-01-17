/*
 * mpvvSpw.h
 *
 *  Created on: 17 мая 2021 г.
 *      Author: Dmitry
 */

#ifndef LIBVME_MPVV_MPVVSPW_H_
#define LIBVME_MPVV_MPVVSPW_H_

#include "spw_lib.h"

#define MPVV_HOST_ADDR_MPVVV1 0x30


int mpvvSpwInitPort(int iPort);
int mpvvSpwInit();
int mpvvInitMemoryController();

#endif /* LIBVME_MPVV_MPVVSPW_H_ */
