#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <mqueue.h>
#include <errno.h>
#include <sys/iofunc.h>
#include <sys/netmgr.h>

#include "defines.h"
#include "stateTasks.h"


#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL

char *prognames = "timer_per1.c";


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // needs to be set to PTHREAD_MUTEX_INITIALIZER;

char NewCarGlobal[3]="aaa";

// for timer and message receive

struct sigevent         event;

void *keyboard(void *notused)
{
	char NewCarKey[3];
	while(1)
	{
		scanf(" %s",&NewCarKey);
		pthread_mutex_lock(&mutex);
		strcpy(NewCarGlobal,NewCarKey);
		pthread_mutex_unlock(&mutex);
		printf("'%s'\n",&NewCarGlobal);
	}
}

enum states SensorDrivenLightSequence(void *CurrentState)
{
	enum states CurState = *(enum states*)CurrentState;
	//CurState = CurrentState;

	static char NewCarReceive[3];

	switch (CurState){
	case EWR_NSR_EWTR_NSTR_0:
		DoSomething0();
		if(strcmp("ewt",NewCarReceive) == 0)
		{
			printf("Car waiting, east west right turn\n");
			CurState = EWR_NSR_EWTG_NSTR_1;
		}
		else if(strcmp("ew",NewCarReceive) == 0)
		{
			printf("Car waiting, east west\n");
			CurState = EWG_NSR_EWTR_NSTR_4;
		}
		else if(strcmp("nst",NewCarReceive) == 0)
		{
			printf("Car waiting, north south right turn\n");
			CurState = EWR_NSR_EWTR_NSTG_7;
		}
		else if(strcmp("ns",NewCarReceive) == 0)
		{
			printf("Car waiting, north south\n");
			CurState = EWR_NSG_EWTR_NSTR_10;
		}
		else
		{
			printf("No Cars waiting, east west green\n");
			CurState = EWG_NSR_EWTR_NSTR_4;
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
		DoSomething3();
		CurState = EWG_NSR_EWTR_NSTR_4;
		break;
	case EWG_NSR_EWTR_NSTR_4:
		DoSomething4();
		while(CurState == EWG_NSR_EWTR_NSTR_4)
		{
			pthread_mutex_lock(&mutex);
			strcpy(NewCarReceive,NewCarGlobal);
			strcpy(NewCarGlobal,"aaa");
			pthread_mutex_unlock(&mutex);
			if(strcmp("ewt",NewCarReceive) == 0)
			{
				printf("Car waiting, east west right turn\n");
				CurState = EWY_NSR_EWTR_NSTR_5;
			}
			else if(strcmp("nst",NewCarReceive) == 0)
			{
				printf("Car waiting, north south right turn\n");
				CurState = EWY_NSR_EWTR_NSTR_5;
			}
			else if(strcmp("ns",NewCarReceive) == 0)
			{
				printf("Car waiting, north south\n");
				CurState = EWY_NSR_EWTR_NSTR_5;
			}
			else
			{
				CurState = EWG_NSR_EWTR_NSTR_4;
			}
		}
		break;
	case EWY_NSR_EWTR_NSTR_5:
		DoSomething5();
		CurState = EWR_NSR_EWTR_NSTR_6;
		break;
	case EWR_NSR_EWTR_NSTR_6:
		DoSomething6();
		if(strcmp("ewt",NewCarReceive) == 0)
		{
			printf("Car waiting, east west right turn\n");
			CurState = EWR_NSR_EWTG_NSTR_1;
		}
		else if(strcmp("nst",NewCarReceive) == 0)
		{
			printf("Car waiting, north south right turn\n");
			CurState = EWR_NSR_EWTR_NSTG_7;
		}
		else if(strcmp("ns",NewCarReceive) == 0)
		{
			printf("Car waiting, north south\n");
			CurState = EWR_NSG_EWTR_NSTR_10;
		}
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
		DoSomething9();
		CurState = EWR_NSG_EWTR_NSTR_10;
		break;
	case EWR_NSG_EWTR_NSTR_10:
		DoSomething10();
		while(CurState == EWR_NSG_EWTR_NSTR_10)
		{
			pthread_mutex_lock(&mutex);
			strcpy(NewCarReceive,NewCarGlobal);
			strcpy(NewCarGlobal,"aaa");
			pthread_mutex_unlock(&mutex);
			if(strcmp("ewt",NewCarReceive) == 0)
			{
				printf("Car waiting, east west right turn\n");
				CurState = EWR_NSY_EWTR_NSTR_11;
			}
			else if(strcmp("ew",NewCarReceive) == 0)
			{
				printf("Car waiting, east west\n");
				CurState = EWR_NSY_EWTR_NSTR_11;
			}
			else if(strcmp("nst",NewCarReceive) == 0)
			{
				printf("Car waiting, north south right turn\n");
				CurState = EWR_NSY_EWTR_NSTR_11;
			}
			else
			{
				CurState = EWR_NSG_EWTR_NSTR_10;
			}
		}
		break;
	case EWR_NSY_EWTR_NSTR_11:
		DoSomething11();
		CurState = EWR_NSR_EWTR_NSTR_0;
		break;
	}
	return CurState;
}
