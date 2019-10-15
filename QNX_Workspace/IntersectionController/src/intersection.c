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
#include "stateTasks.h"
#include "communication.h"
#include <sys/dispatch.h>
#include <fcntl.h>
#include <share.h>


//pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;


void *stateMachineThread()
{

	// means we will need to pass it by address

	while (1)
	{
		if(TrainApproachint==1)
		{
		printf("State machine nows that train approaching\n");
		}
		if (switchingMode == 1) {
			CurrentMode = FIXED;
			printf("Changing mode...\t");
		}
		switch (CurrentMode) {
		case FIXED:
			printf("Fixed SM> \t");
			CurrentState = SingleStep_TrafficLight_SM( &CurrentState ); // pass address
			while(TrainApproachint==1)
			{
				CurrentState = EWR_NSG_EWTR_NSTR_10;
			}
			break;
		case SENSOR:
			printf("Sensor SM> \t");
			CurrentState = SensorDrivenLightSequence( &CurrentState ); // pass address
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

	struct Timervalues t;
	t.NSG_car 	= 4;
	t.NSB_ped 	= 1;
	t.NSTG_car 	= 4;
	t.NSY_car 	= 2;
	t.NSTY_car 	= 2;
	t.NSR_clear	= 1;
	t.NSTR_clear= 1;

	t.EWG_car	= 4;
	t.EWB_ped	= 1;
	t.EWTG_car	= 4;
	t.EWY_car	= 2;
	t.EWTY_car	= 2;
	t.EWR_clear = 1;
	t.EWTR_clear= 1;
	setTimerValues(t);

	pthread_create(&keyboarInput, NULL, keyboard, NULL);
	pthread_create(&stateMachine,NULL,stateMachineThread,NULL);
	char val[100] = attachPoint;
	//strcpy(val, QNET_ATTACH_POINT);
	//printf("The attachpoint is located in the following directory: %s \n", val);
	pthread_t  th3_client;
	pthread_create (&th3_client, NULL, ex_client, val);

	pthread_join(stateMachine,NULL);
}
