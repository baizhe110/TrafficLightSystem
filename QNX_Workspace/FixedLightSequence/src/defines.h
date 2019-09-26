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

// structs ****************************************
struct Timervalues{
	double NSG_car;
	double NSB_ped;
	double NSTG_car;
	double NSY_car;
	double NSTY_car;
	double NSR_clear;
	double NSTR_clear;

	double EWG_car;
	double EWB_ped;
	double EWTG_car;
	double EWY_car;
	double EWTY_car;
	double EWR_clear;
	double EWTR_clear;
};

// global variables ****************************************
enum states CurState, CurSubstate;

// timer variables
struct itimerspec itime1;

struct Timervalues times;


// functions **************************************************
// initialisations
void initTimer();
void setTimerValues();

// functions for each state
void Initialise();
void DoSomething0();
void DoSomething1();
void DoSomething2();
void DoSomething3();
void DoSomething4();
void DoSomething5();
void DoSomething6();
void DoSomething7();
void DoSomething8();
void DoSomething9();
void DoSomething10();
void DoSomething11();

void startOneTimeTimer(timer_t timerID, double time);

enum states SingleStep_TrafficLight_SM(void *CurrentState);


#endif /* SRC_DEFINES_H_ */
