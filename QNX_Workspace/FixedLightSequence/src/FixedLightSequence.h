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

// functions **************************************************
// initialisations



void Initialise();

enum states SingleStep_TrafficLight_SM(void *CurrentState);

#endif /* SRC_FIXEDLIGHTSEQUENCE_H_ */
