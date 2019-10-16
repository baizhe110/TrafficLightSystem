/*
 * defines.h
 *
 *  Created on: 26 Sep 2019
 *      Author: bruno
 */

#ifndef SRC_DEFINES_H_
#define SRC_DEFINES_H_

enum INTERSECTION_TYPE {
	Intersection1,
	Intersection2,
	BoomGate,
};
/* Example intersection input

change the timing: 	intersection1;2;10,2,3,4,5,6,7,10,9,10,11,12,13,14
change the mode:	intersection1;1;1
change the mode of boomgate: boom;1;1
*/
#define attachPoint "/net/VM_x86_Target02/dev/name/local/CentralServer"
//#define attachPoint "/net/EA42e8b8/dev/name/local/CentralServer"
//#define attachPointSem "/net/EA42e8b8/dev/sem/sync"
#define attachPointSem "/net/VM_x86_Target02/dev/sem/sync"
//#define attachPoint "/net/GerardHost/dev/name/local/CentralServer"
#define intersectionType Intersection1

#include <semaphore.h>

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

enum sensorInput {
	EW_sensor,
	EWT_sensor,
	NS_sensor,
	NST_sensor,
	TRAIN_sensor
};

enum substates {
	INTERSECTION_NORMAL,
	EWB_NSR_EWTR_NSTR_4_1,
	EWR_NSB_EWTR_NSTR_10_1
};

enum intersection_modes {
	FIXED,
	SENSOR,
	FIXED_SYNCED,
	SPECIAL
};

// global variables ****************************************
enum states CurrentState, CurSubstate;
enum intersection_modes CurrentMode, desiredMode;
enum sensorInput currentSensor; // clear sensor input after working through it

int switchingMode;
int syncing;
sem_t *sem_sync;


int TrainApproachint;


#endif /* SRC_DEFINES_H_ */
