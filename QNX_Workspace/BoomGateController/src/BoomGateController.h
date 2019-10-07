/*
 * BoomGateSequence.h
 *
 */

#ifndef SRC_BOOMGATESEQUENCE_H_
#define SRC_BOOMGATESEQUENCE_H_

enum states BoomGateSequence(void *CurrentState);

void *keyboard(void *notused);

// enums ********************************************
enum states {
	NoTrain_0,
	TrainApproaching_1,
	TrainApproaching_2,
	TrainCrossing_3,
	TrainLeaving_4,
	TrainLeaving_5,
};

//enum substates {
//	INTERSECTION_NORMAL,
//	EWB_NSR_EWTR_NSTR_4_1,
//	EWR_NSB_EWTR_NSTR_10_1
//};

enum BoomGate_mode {
	NORMAL,
	SPECIAL
};

// global variables ****************************************
enum states CurState, CurSubstate;

#endif /* SRC_SENSORLIGHTSEQUENCE_H_ */
