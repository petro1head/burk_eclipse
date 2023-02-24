/*
 * mpvvMemoryController.h
 *
 *  Created on: 17 мая 2021 г.
 *      Author: Dmitry
 */

#ifndef LIBVME_MPVV_MPVVMEMORYCONTROLLER_H_
#define LIBVME_MPVV_MPVVMEMORYCONTROLLER_H_

#include "vm014MemoryController.h"


extern const int mpvvBootRomBank ;
extern const int mpvvNvRamBankStart ; // PZU MRAM starting CS
extern const int mpvvNvRamBankNum ; // PZU MRAM number of CS


int mpvvInitMemoryController();


#endif /* LIBVME_MPVV_MPVVMEMORYCONTROLLER_H_ */
