#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>
#include <errno.h>
#include <pthread.h>


#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL

char *progname = "timer_per1.c";

typedef union
{
	struct _pulse   pulse;
    // your other message structures would go here too
} my_message_t;

struct sigevent         event;
struct itimerspec       itime1, itime2;
timer_t                 timer_id;
int                     chid;
int                     rcvid;
my_message_t            msg;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // needs to be set to PTHREAD_MUTEX_INITIALIZER;

char NewCarGlobal[3]="aaa";

enum states { EWR_NSR_EWTR_NSTR_0,
	EWR_NSR_EWTG_NSTR_1,
	EWR_NSR_EWTY_NSTR_2,
	EWR_NSR_EWTR_NSTR_3,
	EWG_NSR_EWTR_NSTR_4,
	EWY_NSR_EWTR_NSTR_5,
	EWR_NSR_EWTR_NSTR_6,
	EWR_NSR_EWTR_NSTG_7,
	EWR_NSR_EWTR_NSTY_8,
	EWR_NSR_EWTR_NSTR_9,
	EWR_NSG_EWTR_NSTR_10,
	EWR_NSY_EWTR_NSTR_11,
	EWR_NSR_EWTR_NSTR_12};
enum states CurState;
//--- --- --- ---


void *keyboard(void *notused)
{
	while(1)
	{
		char NewCarKey[3];
		scanf(" %s",&NewCarKey);
	pthread_mutex_lock(&mutex);
	strcpy(NewCarGlobal,NewCarKey);
	pthread_mutex_unlock(&mutex);
	printf("%s\n",&NewCarGlobal);
	}
}

void Initialise()
{
	printf("Start of fixed light sequence\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}

void DoSomething0()
{
	printf("In state0: EWR_NSR_EWTR_NSTR_0 bla bla\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}

void DoSomething1()
{
	printf("In state1: EWR_NSR_EWTG_NSTR_1\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething2()
{
	printf("In state2: EWR_NSR_EWTY_NSTR_2\n");
	timer_settime(timer_id, 0, &itime2, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}

void DoSomething3()
{
	printf("In state3: EWR_NSR_EWTR_NSTR_3\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}

void DoSomething4()
{
	printf("In state4: EWG_NSR_EWTR_NSTR_4\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}

void DoSomething5()
{
	printf("In state5: EWY_NSR_EWTR_NSTR_5\n");
	timer_settime(timer_id, 0, &itime2, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}

void DoSomething6()
{
	printf("In state6: EWR_NSR_EWTR_NSTR_6\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}

void DoSomething7()
{
	printf("In state7: EWR_NSR_EWTR_NSTG_7\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}

void DoSomething8()
{
	printf("In state8: EWR_NSR_EWTR_NSTY_8\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}

void DoSomething9()
{
	printf("In state9: EWR_NSR_EWTR_NSTR_9\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}

void DoSomething10()
{
	printf("In state10: EWR_NSG_EWTR_NSTR_10\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}

void DoSomething11()
{
	printf("In state11: EWR_NSY_EWTR_NSTR_11\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}

void DoSomething12()
{
	printf("In state12: EWR_NSR_EWTR_NSTR_12\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}

enum states FixedLightSequence(void *CurrentState)

{

	enum states CurState = *(enum states*)CurrentState;


	//CurState = CurrentState;

	switch (CurState){


	case EWR_NSR_EWTR_NSTR_0:
		DoSomething0();
		CurState = EWR_NSR_EWTG_NSTR_1;
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
		CurState = EWY_NSR_EWTR_NSTR_5;
		break;
	case EWY_NSR_EWTR_NSTR_5:
		DoSomething5();
		CurState = EWR_NSR_EWTR_NSTR_6;
		break;
	case EWR_NSR_EWTR_NSTR_6:
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
		DoSomething9();
		CurState = EWR_NSG_EWTR_NSTR_10;
		break;
	case EWR_NSG_EWTR_NSTR_10:
		DoSomething10();
		CurState = EWR_NSY_EWTR_NSTR_11;
		break;
	case EWR_NSY_EWTR_NSTR_11:
		DoSomething11();
		CurState = EWR_NSR_EWTR_NSTR_0;
		break;
	}
	return CurState;
}


enum states SensorDrivenLightSequence(void *CurrentState)

{

	enum states CurState = *(enum states*)CurrentState;


	//CurState = CurrentState;

	char NewCarReceive[3];

	switch (CurState){

	case EWR_NSR_EWTR_NSTR_0:
		DoSomething0();
		pthread_mutex_lock(&mutex);
			strcpy(NewCarReceive,NewCarGlobal);
			strcpy(NewCarGlobal,"aaa");
		pthread_mutex_unlock(&mutex);
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
		else if(strcmp("ewp",NewCarReceive) == 0)
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
			CurState = EWR_NSR_EWTR_NSTR_0;
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
		break;
	case EWR_NSY_EWTR_NSTR_11:
		DoSomething11();
		CurState = EWR_NSR_EWTR_NSTR_12;
		break;
	case EWR_NSR_EWTR_NSTR_12:
		DoSomething12();
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
		break;
	}
	return CurState;
}


int main(int argc, char *argv[])
{
	printf("Fixed Sequence Traffic Lights State Machine\n");
	int Runtimes=100, counter = 0;
	enum states CurrentState=0;
	int Sequence=1, OldSequence=0;

	// means we will need to pass it by address

		chid = ChannelCreate(0); // Create a communications channel

		event.sigev_notify = SIGEV_PULSE;

		// create a connection back to ourselves for the timer to send the pulse on
		event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL, 0);
		if (event.sigev_coid == -1)
		{
		   printf(stderr, "%s:  couldn't ConnectAttach to self!\n", progname);
		   perror(NULL);
		   exit(EXIT_FAILURE);
		}
		//event.sigev_priority = getprio(0);  // this function is depreciated in QNX 700
		struct sched_param th_param;
		pthread_getschedparam(pthread_self(), NULL, &th_param);
		event.sigev_priority = th_param.sched_curpriority;    // old QNX660 version getprio(0);

		event.sigev_code = MY_PULSE_CODE;

		// create the timer, binding it to the event
		if (timer_create(CLOCK_REALTIME, &event, &timer_id) == -1)
		{
		   printf (stderr, "%s:  couldn't create a timer, errno %d\n", progname, errno);
		   perror (NULL);
		   exit (EXIT_FAILURE);
		}

		// setup the timer (1.5s initial delay value, 1.5s reload interval)
		itime1.it_value.tv_sec = 1;			  // 1 second
		itime1.it_value.tv_nsec = 0;    // 500 million nsecs = .5 secs
		itime1.it_interval.tv_sec = 0;          // 1 second
		itime1.it_interval.tv_nsec = 0; // 500 million nsecs = .5 secs

		// setup the timer (1.5s initial delay value, 1.5s reload interval)
		itime2.it_value.tv_sec = 2;			  // 1 second
		itime2.it_value.tv_nsec = 0;    // 500 million nsecs = .5 secs
		itime2.it_interval.tv_sec = 0;          // 1 second
		itime2.it_interval.tv_nsec = 0; // 500 million nsecs = .5 secs


		pthread_mutex_init(&mutex,NULL);
		pthread_t  th1;
		pthread_create(&th1, NULL, keyboard, NULL);


	while (counter < Runtimes)
	{
		if(Sequence != OldSequence)
		{
			CurrentState = 0;
			if(Sequence==1)
				{
				printf("Start of sensor driven light sequence\n");
				}
			else if(Sequence == 0)
				{
				printf("Start of fixed light sequence\n");
				}

		}

		if(Sequence == 0)
		{
		CurrentState = FixedLightSequence( &CurrentState ); // pass address
		counter++;
		}
		else if (Sequence == 1)
		{
		CurrentState = SensorDrivenLightSequence( &CurrentState ); // pass address
		counter++;
		}
		OldSequence = Sequence;
	}


}
