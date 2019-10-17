/*********************************************************************
 *					FIXED LIGHT SEQUENCE STATE MACHINE
 *
 * 				Includes:
 * 					1. Fixed light sequence state machine
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <semaphore.h>

#include "defines.h"
#include "stateTasks.h"
#include "FixedLightSequence.h"





enum states SingleStep_TrafficLight_SM(void *CurrentState)
{
	enum states CurState = *(enum states*)CurrentState;

	if (switchingMode == 1 && desiredMode == 0) {
		printf("Mode switched\n");
		printf("Fixed SM> \t");
		switchingMode = 0;
		//CurState = EWR_NSR_EWTR_NSTR_0;
	}

	switch (CurState)
	{
	case EWR_NSR_EWTR_NSTR_0:
		StateTime0();
		if (TrainApproachint==1)
		{
			printf("Train approaching, changing to NSG\n");
			CurState = EWR_NSG_EWTR_NSTR_10;
			break;
		}
		if (switchingMode == 1) {
			CurrentMode = desiredMode;
			if(syncing)
			{
				//printf("Trying to sync with other nodes\n");
				int semValue;
				sem_getvalue(sem_sync, &semValue);
				printf("Clients to sync with: %d\n",semValue);
				if (semValue > 0) {
					//printf("start synced\n");
					sem_wait(sem_sync);
					while(semValue != 0)
					{
						sem_getvalue(sem_sync, &semValue);
					}
					printf("Successfully synced with all clients\n");
					CurrentMode = FIXED;
					desiredMode = FIXED;
					sem_close(sem_sync);
				}
				syncing = 0;
				// wait until data from central server signals all nodes ready
				// if server not availiable continue manually
				// if takes more than xxx time to manually

			}
			switchingMode = 0;
			break;
		}
		CurState = EWR_NSR_EWTG_NSTR_1;
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
		if (TrainApproachint==1)
		{
			printf("Train approaching, changing to NSG\n");
			CurState = EWR_NSG_EWTR_NSTR_10;
			break;
		}
		else if (switchingMode == 1) {
			CurState = EWR_NSR_EWTR_NSTR_0;
			break;
		}
		CurState = EWG_NSR_EWTR_NSTR_4;
		break;
	case EWG_NSR_EWTR_NSTR_4:
		StateTime4();
		StateTime4_1();
		CurState = EWY_NSR_EWTR_NSTR_5;
		break;
	case EWY_NSR_EWTR_NSTR_5:
		StateTime5();
		CurState = EWR_NSR_EWTR_NSTR_6;
		break;
	case EWR_NSR_EWTR_NSTR_6:
		StateTime6();
		if (TrainApproachint==1)
		{
			printf("Train approaching, changing to NSG\n");
			CurState = EWR_NSG_EWTR_NSTR_10;
			break;
		}
		else if (switchingMode == 1) {
			CurState = EWR_NSR_EWTR_NSTR_0;
			break;
		}
		CurState = EWR_NSR_EWTR_NSTG_7;
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
		if (switchingMode == 1) {
			CurState = EWR_NSR_EWTR_NSTR_0;
			break;
		}
		CurState = EWR_NSG_EWTR_NSTR_10;
		break;
	case EWR_NSG_EWTR_NSTR_10:
		StateTime10();
		if(TrainApproachint==1)
		{
			printf("Train approaching, staying in NSG\n");
		}
		while(TrainApproachint==1)
		{

		}
		StateTime10_1();
		CurState = EWR_NSY_EWTR_NSTR_11;
		break;
	case EWR_NSY_EWTR_NSTR_11:
		StateTime11();
		CurState = EWR_NSR_EWTR_NSTR_0;
		break;
	}
	return CurState;
}
