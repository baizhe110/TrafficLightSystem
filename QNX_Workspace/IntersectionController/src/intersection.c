/*********************************************************************
 *						INTERSECTION CONTROLLER
 *
 * 				Includes:
 * 					1. State machine (main) thread
 * 					2. MAIN (For intersection controller)
 *********************************************************************/


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





/*********************************************************************
 *					STATE MACHINE (MAIN) THREAD
 *				Controls which state machine to call
 *********************************************************************/
void *stateMachineThread()
{

	// means we will need to pass it by address


	while (1)
	{
		if(TrainApproachint==1)
		{
			printf("State machine knows that train approaching\n");
		}
		switch (CurrentMode) {
		case FIXED:
			printf("Fixed SM> \t");
			CurrentState = SingleStep_TrafficLight_SM( &CurrentState ); // pass address
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









/*********************************************************************
 *							INTERSECTION MAIN
 * 				Starting point for the intersection node!
 *********************************************************************/
int main(int argc, char *argv[])
{

	//Declaring threads
	pthread_t stateMachine, keyboarInput, th3_client;


	//Initialize variables, specifying what mode to start up in
	switchingMode = 0;
	CurrentMode = FIXED;
	CurrentState = EWR_NSR_EWTR_NSTR_0;

	//Setting the currentSensor value to -1, meaning nothing approaching in sensor.
	currentSensor = -1;


	//Getting the hostname and printing to console that we now start intersection
	printf("Intersection Controller starts now...\n");
	char hostnm[100];
	memset(hostnm, '\0', 100);
	hostnm[99] = '\n';
	gethostname(hostnm, sizeof(hostnm));
	printf("--> Machine hostname is: '%s'\n", hostnm);
	printf("Intersection startup mode: %d\n", CurrentMode);
	printf("Intersection startup state: %d\n",CurrentState);
	printf("\n");



	/******   Setting up the QNX timer to be used in the timing of the traffic lights (states)  ******/

	initTimer(); 					//Initialization
	struct Timervalues t;
	t.NSG_car 	= 4;
	t.NSB_ped 	= 1;
	t.NSTG_car 	= 4;
	t.NSY_car 	= 2;
	t.NSTY_car 	= 2;
	t.NSR_clear	= 1;
	t.NSTR_clear= 1;
									// Setting the initial timings for the lights
	t.EWG_car	= 4;
	t.EWB_ped	= 1;
	t.EWTG_car	= 4;
	t.EWY_car	= 2;
	t.EWTY_car	= 2;
	t.EWR_clear = 1;
	t.EWTR_clear= 1;
	setTimerValues(t); 				// Calling/sending timings to the function that sets the timer values



	// Starting up threads
	pthread_create(&keyboarInput, NULL, keyboard, NULL);  		// Keyboard input, simulating cars and pedestrians in sensor mode
	pthread_create(&stateMachine,NULL,stateMachineThread,NULL); // Starting up the state machine for the intersection

	char val[100] = attachPoint; 								// Setting the attachpoint directory in a character string
	pthread_create (&th3_client, NULL, ClientIntersection, val); 		// Starting up the client to communicated between intersection and centralserver

	pthread_join(stateMachine,NULL);							// Waiting here until the state machine thread is complete (But never happens)
}
