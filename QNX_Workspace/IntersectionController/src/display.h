/*
 * display.h
 *
 *  Created on: 16 Oct 2019
 *      Author: bruno
 */

#ifndef SRC_DISPLAY_H_
#define SRC_DISPLAY_H_

#include <stdio.h>
#include <stdlib.h>
#include <hw/inout.h>      // for in32() and out32();
#include <sys/mman.h>      // for mmap_device_io();
#include <sys/neutrino.h>  // for ThreadCtl( _NTO_TCTL_IO_PRIV , NULL);
#include <stdint.h>		   // for unit32 types
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>
#include <errno.h>

#include "defines.h"
#include "hps.h"
#include "hps_0.h"

#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL

#define PIO_SIZE			0x0000001F
#define PIO_SIZE_Switches	0x0000000F
#define LEDs_BASE			0xFF206000  //(LEDs - 8 bits wide - output only)
#define BUTTONs_BASE		0xFF207000  //(GPIO_1 - JP7 header - 16 bits wide - Input only)

enum displayStates {
	NSPedR = 2,
	EWPedG = 4,
	EWPedR = 6,
	NSR = 10,
	NSY = 12,
	NSG = 16,
	NSTG = 18,
	NSTY = 20,
	NSTR = 24,
	NSPedG = 26,
	TrainR1 = 13,
	TrainR2 = 9,
	EWTR = 15,
	EWTY = 19,
	EWTG = 23,
	EWG = 25,
	EWY = 27,
	EWR = 31
};

enum Buttons {
	EWButton,
	EWTButton,
	EWPedButton,
	NSButton,
	NSTButton,
	NSPedButton
};

void *updateIntersection(void *notused);


#endif /* SRC_DISPLAY_H_ */
