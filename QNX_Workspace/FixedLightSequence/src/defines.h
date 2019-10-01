/*
 * defines.h
 *
 *  Created on: 26 Sep 2019
 *      Author: bruno
 */

#ifndef SRC_DEFINES_H_
#define SRC_DEFINES_H_

// enums ********************************************
enum states {
	EWR_NSR_EWTR_NSTR_0,
	EWR_NSR_EWTG_NSTR_1,
	EWR_NSR_EWTY_NSTR_2,
	EWR_NSR_EWTR_NSTR_3,
	EWG_NSR_EWTR_NSTR_4,	// Peds Crossing
	EWY_NSR_EWTR_NSTR_5,
	EWR_NSR_EWTR_NSTR_6,
	EWR_NSR_EWTR_NSTG_7,
	EWR_NSR_EWTR_NSTY_8,
	EWR_NSR_EWTR_NSTR_9,
	EWR_NSG_EWTR_NSTR_10,	// Peds Crossing
	EWR_NSY_EWTR_NSTR_11
};

enum substates {
	INTERSECTION_NORMAL,
	EWB_NSR_EWTR_NSTR_4_1,
	EWR_NSB_EWTR_NSTR_10_1
};

enum intersection_mode {
	FIXED,
	SENSOR,
	SPECIAL
};

// global variables ****************************************
enum states CurState, CurSubstate;

#endif /* SRC_DEFINES_H_ */
