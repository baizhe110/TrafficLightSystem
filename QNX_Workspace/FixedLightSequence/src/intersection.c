/*
 * intersection.c
 *
 *  Created on: 1 Oct 2019
 *      Author: bruno
 */

#include <pthread.h>
#include "defines.h"
#include "FixedLightSequence.h"

enum intersection_mode mode = FIXED;

void *stateMachineThread()
{
	int Runtimes=30, counter = 0;
	enum states CurrentState=0; // Declaring the enum within the main
	// means we will need to pass it by address

	while (counter < Runtimes)
	{
		switch (mode) {
		case FIXED:
			printf("Fixed SM> \t");
			CurrentState = SingleStep_TrafficLight_SM( &CurrentState ); // pass address
			counter++;
			break;
		case SENSOR:
			break;
		default:
			break;
		}
	}
}

// Intersection Node starting point
int main(int argc, char *argv[])
{
	pthread_t stateMachine;

	printf("Intersection Node started with mode: %d\n", mode);


	initTimer();
	setTimerValues();

	pthread_create(&stateMachine,NULL,stateMachineThread,NULL);

	pthread_join(stateMachine,NULL);

}
