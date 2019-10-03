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
enum intersection_mode mode = FIXED;

void *stateMachineThread()
{
	int Runtimes=30, counter = 0;
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
			printf("Sensor SM> \t");
			CurrentState = SensorDrivenLightSequence( &CurrentState ); // pass address
			counter++;
			break;
		default:
			break;
		}
	}
}


// Intersection Node starting point
int main(int argc, char *argv[])
{
	pthread_t stateMachine, keyboarInput;

	printf("Intersection Node started with mode: %d\n", mode);

	initTimer();
	setTimerValues();

	pthread_create(&keyboarInput, NULL, keyboard, NULL);
	pthread_create(&stateMachine,NULL,stateMachineThread,NULL);
	char *val = "/net/VM_x86_Target02/dev/name/local/CentralServer";
	//strcpy(val, QNET_ATTACH_POINT);
	printf("val: %s \n", val);
	pthread_t  th3_client;
	pthread_create (&th3_client, NULL, ex_client, NULL);

	pthread_join(stateMachine,NULL);
}
