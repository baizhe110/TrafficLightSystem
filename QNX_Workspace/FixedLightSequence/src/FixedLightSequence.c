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

	switch (CurState)
	{
	case EWR_NSR_EWTR_NSTR_0:
		DoSomething0();
		CurState = EWR_NSR_EWTG_NSTR_1;
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
					//CurrentMode = FIXED;
					sem_close(sem_sync);
				}
				syncing = 0;
				// wait until data from central server signals all nodes ready
				// if server not availiable continue manually
				// if takes more than xxx time to manually
			}
			switchingMode = 0;
			printf("Mode switched\n");
		}
		break;
	case EWR_NSR_EWTG_NSTR_1:
		DoSomething1();
		CurState = EWR_NSR_EWTY_NSTR_2;
		break;
	case EWR_NSR_EWTY_NSTR_2:
		DoSomething2();
		CurState = EWR_NSR_EWTR_NSTR_3;
		break;
	case EWR_NSR_EWTR_NSTR_3:
		if (switchingMode == 1) {
			CurState = EWR_NSR_EWTR_NSTR_0;
			break;
		}
		DoSomething3();
		CurState = EWG_NSR_EWTR_NSTR_4;
		break;
	case EWG_NSR_EWTR_NSTR_4:
		DoSomething4();
		DoSomething4_1();
		CurState = EWY_NSR_EWTR_NSTR_5;
		break;
	case EWY_NSR_EWTR_NSTR_5:
		DoSomething5();
		CurState = EWR_NSR_EWTR_NSTR_6;
		break;
	case EWR_NSR_EWTR_NSTR_6:
		if (switchingMode == 1) {
			CurState = EWR_NSR_EWTR_NSTR_0;
			break;
		}
		DoSomething6();
		CurState = EWR_NSR_EWTR_NSTG_7;
		break;
	case EWR_NSR_EWTR_NSTG_7:
		DoSomething7();
		CurState = EWR_NSR_EWTR_NSTY_8;
		break;
	case EWR_NSR_EWTR_NSTY_8:
		DoSomething8();
		CurState = EWR_NSR_EWTR_NSTR_9;
		break;
	case EWR_NSR_EWTR_NSTR_9:
		if (switchingMode == 1) {
			CurState = EWR_NSR_EWTR_NSTR_0;
			break;
		}
		DoSomething9();
		CurState = EWR_NSG_EWTR_NSTR_10;
		break;
	case EWR_NSG_EWTR_NSTR_10:
		DoSomething10();
		DoSomething10_1();
		CurState = EWR_NSY_EWTR_NSTR_11;
		break;
	case EWR_NSY_EWTR_NSTR_11:
		DoSomething11();
		CurState = EWR_NSR_EWTR_NSTR_0;
		break;
	}
	return CurState;
}
