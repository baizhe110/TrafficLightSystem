/*
 * FixedLightSequence.h
 *
 *  Created on: 1 Oct 2019
 *      Author: bruno
 */

#ifndef SRC_FIXEDLIGHTSEQUENCE_H_
#define SRC_FIXEDLIGHTSEQUENCE_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

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

#endif /* SRC_FIXEDLIGHTSEQUENCE_H_ */
