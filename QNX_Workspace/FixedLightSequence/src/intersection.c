/*
 * intersection.c
 *
 *  Created on: 1 Oct 2019
 *      Author: bruno
 */

#include <pthread.h>
#include "defines.h"
#include "FixedLightSequence.h"
#include "SensorLightSequence.h"
#include "communication.h"
#include <sys/dispatch.h>
#include <fcntl.h>
#include <share.h>


//pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;


void *stateMachineThread()
{
	int Runtimes=30, counter = 0;
	// means we will need to pass it by address

	while (counter < Runtimes)
	{
		if (switchingMode == 1) {
			CurrentMode = FIXED;
			printf("Changing mode...\n");
		}
		switch (CurrentMode) {
		case FIXED:
			printf("Fixed SM> \t");
			CurrentState = SingleStep_TrafficLight_SM( &CurrentState ); // pass address
			counter++;
			break;
		case SENSOR:
			printf("Sensor SM> \t");
			CurrentState = SensorDrivenLightSequence( &CurrentState ); // pass address
			counter++;
			break;
		case SPECIAL:
			CurrentState = EWR_NSR_EWTR_NSTG_7;
			printf("Special state> %d\t", CurrentState);
			while(switchingMode == 0)
			{
			}
			break;
		default:
			//printf("Unexpected mode %d\n", CurrentMode);
			break;
		}
	}
}


// Intersection Node starting point
int main(int argc, char *argv[])
{
	pthread_t stateMachine, keyboarInput;

	printf("Intersection Node started with mode: %d\n", CurrentMode);

	switchingMode = 0;
	CurrentMode = FIXED;
	initTimer();
	setTimerValues();

	pthread_create(&keyboarInput, NULL, keyboard, NULL);
	pthread_create(&stateMachine,NULL,stateMachineThread,NULL);
	char val[100] = attachPoint;
	//strcpy(val, QNET_ATTACH_POINT);
	printf("val: %s \n", val);
	pthread_t  th3_client;
	pthread_create (&th3_client, NULL, ex_client, val);

	pthread_join(stateMachine,NULL);
}
