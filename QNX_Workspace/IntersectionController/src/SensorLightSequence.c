/*********************************************************************
 *						SENSOR DRIVEN STATE MACHINE
 *
 * 				Includes:
 * 					1. Keyboard input (To simulate incoming cars and pedestrians)
 * 					2. Sensor driven state machine
 *********************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "defines.h"
#include "stateTasks.h"
#include "SensorLightSequence.h"


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // needs to be set to PTHREAD_MUTEX_INITIALIZER;

char NewCarGlobal[3]="aaa";   // Declaring the global variable used for keyboard input






/*********************************************************************
 *			KEYBOARD INPUT FOR INCOMING CARS AND PEDESTRIANS
 *			Keyboard inputs:
 *					1. Car incoming from east west in the right turn lane: 	"ewt"
 *					2. Car incoming from east west straight or left lane:  	"ew"
 *					3. Car incoming from north east in the right turn lane: "nst"
 *					4. Car incoming from north east straight or left lane: 	"ns"
 *********************************************************************/
void *keyboard(void *notused)
{
	char NewCarKey[3];
	while(1)
	{
		scanf(" %s",&NewCarKey);			//Waiting here for a input from the keyboard

		pthread_mutex_lock(&mutex);
		if(strcmp("ewt",NewCarKey) == 0)
		{
			currentSensor = EWT_sensor;
		}
		else if(strcmp("ew",NewCarKey) == 0)
		{
			currentSensor = EW_sensor;
		}
		else if(strcmp("nst",NewCarKey) == 0)
		{
			currentSensor = NST_sensor;
		}
		else if(strcmp("ns",NewCarKey) == 0)
		{
			currentSensor = NS_sensor;
		}
		pthread_mutex_unlock(&mutex);
	}
}





/*********************************************************************
 *					SENSOR DRIVEN STATE MACHINE
 *********************************************************************/
enum states SensorDrivenLightSequence(void *CurrentState)
{
	enum states CurState = *(enum states*)CurrentState;

	static char NewCarReceive[3];

	// If we want to change mode to sensor driven, then do this
	if (switchingMode == 1 && desiredMode == 1) {
		printf("Mode switched\n");
		printf("Sensor SM> \t");
		switchingMode = 0;
		CurState = EWR_NSR_EWTR_NSTR_0;
	}

	switch (CurState){
	case EWR_NSR_EWTR_NSTR_0:
		StateTime0();

		if (switchingMode == 1) {
			CurrentMode = desiredMode;
			break;
		}

		//First checking if any train is approaching. If so, going to NSG
		if (TrainApproachint==1)
		{
			printf("Train approaching, changing to NSG\n");
			CurState = EWR_NSG_EWTR_NSTR_10;
			break;
		}

		//Checking if any car is approaching. If not, going through to EWG
		if(currentSensor == EWT_sensor)
		{
			printf("Car waiting, east west right turn\n");
			CurState = EWR_NSR_EWTG_NSTR_1;
			currentSensor=NO_input;
		}
		else if(currentSensor == EW_sensor)
		{
			printf("Car waiting, east west\n");
			CurState = EWG_NSR_EWTR_NSTR_4;
			currentSensor=NO_input;
		}
		else if(currentSensor == NST_sensor)
		{
			printf("Car waiting, north south right turn\n");
			CurState = EWR_NSR_EWTR_NSTG_7;
			currentSensor=NO_input;
		}
		else if(currentSensor == NS_sensor)
		{
			printf("Car waiting, north south\n");
			CurState = EWR_NSG_EWTR_NSTR_10;
			currentSensor=NO_input;
		}
		else
		{
			printf("No Cars waiting, east west green\n");
			CurState = EWG_NSR_EWTR_NSTR_4;
			currentSensor=NO_input;
		}
		break;

	case EWR_NSR_EWTG_NSTR_1:
		StateTime1();
		CurState = EWR_NSR_EWTY_NSTR_2;
		break;

	case EWR_NSR_EWTY_NSTR_2:
		StateTime2();
		CurState = EWR_NSR_EWTR_NSTR_3;
		break;

	case EWR_NSR_EWTR_NSTR_3:
		StateTime3();

		//Checking if any train is approaching. If so, going to NSG
		if (TrainApproachint==1)
		{
			CurState = EWR_NSG_EWTR_NSTR_10;
			break;
		}

		CurState = EWG_NSR_EWTR_NSTR_4;
		break;

	case EWG_NSR_EWTR_NSTR_4:
		StateTime4();

		/*Staying in this state until:
		 * 			1. If any car approaching
		 * 			2. If we want to change mode (To Fixed or Special)
		 * 			3. If train is approaching
		 */
		while(CurState == EWG_NSR_EWTR_NSTR_4 && switchingMode == 0 && TrainApproachint != 1)
		{
			pthread_mutex_lock(&mutex);
			strcpy(NewCarReceive,NewCarGlobal);
			strcpy(NewCarGlobal,"aaa");
			pthread_mutex_unlock(&mutex);
			if(currentSensor == EWT_sensor)
			{
				printf("Car waiting, east west right turn\n");
				CurState = EWY_NSR_EWTR_NSTR_5;
			}
			else if(currentSensor == NST_sensor)
			{
				printf("Car waiting, north south right turn\n");
				CurState = EWY_NSR_EWTR_NSTR_5;
			}
			else if(currentSensor == NS_sensor)
			{
				printf("Car waiting, north south\n");
				CurState = EWY_NSR_EWTR_NSTR_5;
			}
			else
			{
				CurState = EWG_NSR_EWTR_NSTR_4;
			}
		}

		//If train approaching, we are out of while loop, and then goes through state 5-6 and then to 10 (NSG)
		if(TrainApproachint == 1)
		{
			printf("Train approaching, changing to NSG\n");
			CurState = EWY_NSR_EWTR_NSTR_5;
		}

		StateTime4_1();	//Substate for pedestrian lights
		if (switchingMode == 1) {
			CurState = EWR_NSR_EWTR_NSTR_0;
		}
		break;

	case EWY_NSR_EWTR_NSTR_5:
		StateTime5();
		CurState = EWR_NSR_EWTR_NSTR_6;
		break;

	case EWR_NSR_EWTR_NSTR_6:
		StateTime6();

		//Checking if any train is approaching. If so, going to NSG
		if (TrainApproachint==1)
		{
			CurState = EWR_NSG_EWTR_NSTR_10;
			break;
		}

		//Checking if any car is approaching. If not, going through to NSG
		if(currentSensor == EWT_sensor)
		{
			printf("Car waiting, east west right turn\n");
			CurState = EWR_NSR_EWTG_NSTR_1;
			currentSensor=NO_input;
		}
		else if(currentSensor == NST_sensor)
		{
			printf("Car waiting, north south right turn\n");
			CurState = EWR_NSR_EWTR_NSTG_7;
			currentSensor=NO_input;
		}
		else if(currentSensor == NS_sensor)
		{
			printf("Car waiting, north south\n");
			CurState = EWR_NSG_EWTR_NSTR_10;
			currentSensor=NO_input;
		}
		break;
	case EWR_NSR_EWTR_NSTG_7:
		StateTime7();
		CurState = EWR_NSR_EWTR_NSTY_8;
		break;
	case EWR_NSR_EWTR_NSTY_8:
		StateTime8();
		CurState = EWR_NSR_EWTR_NSTR_9;
		break;
	case EWR_NSR_EWTR_NSTR_9:
		StateTime9();
		CurState = EWR_NSG_EWTR_NSTR_10;
		break;
	case EWR_NSG_EWTR_NSTR_10:
		StateTime10();
		if(TrainApproachint == 1)
		{
			printf("Train approaching, staying in NSG\n");
		}
		while(TrainApproachint==1)
		{
		}
		while(CurState == EWR_NSG_EWTR_NSTR_10 && switchingMode == 0 && TrainApproachint != 1)
		{
			pthread_mutex_lock(&mutex);
			strcpy(NewCarReceive,NewCarGlobal);
			strcpy(NewCarGlobal,"aaa");
			pthread_mutex_unlock(&mutex);
			if(currentSensor == EWT_sensor)
			{
				printf("Car waiting, east west right turn\n");
				CurState = EWR_NSY_EWTR_NSTR_11;
			}
			else if(currentSensor == EW_sensor)
			{
				printf("Car waiting, east west\n");
				CurState = EWR_NSY_EWTR_NSTR_11;
			}
			else if(currentSensor == NST_sensor)
			{
				printf("Car waiting, north south right turn\n");
				CurState = EWR_NSY_EWTR_NSTR_11;
			}
			else
			{
				CurState = EWR_NSG_EWTR_NSTR_10;
			}
		}
		if(TrainApproachint == 1)
		{
			CurState = EWR_NSG_EWTR_NSTR_10;
		}
		if (switchingMode == 1) {
			CurState = EWR_NSR_EWTR_NSTR_0;
		}
		StateTime10_1();
		break;
	case EWR_NSY_EWTR_NSTR_11:
		StateTime11();
		CurState = EWR_NSR_EWTR_NSTR_0;
		break;
	}
	return CurState;
}
