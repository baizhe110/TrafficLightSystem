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


void startOneTimeTimer(timer_t timerID, double time);

enum states SingleStep_TrafficLight_SM(void *CurrentState);

#endif /* SRC_FIXEDLIGHTSEQUENCE_H_ */
