#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>

#include "BoomGateController.h"

#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL


char *progname = "timer_per1.c";

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // needs to be set to PTHREAD_MUTEX_INITIALIZER;

char NewTrainGlobal[3]="aaa";

// for timer and message receive

struct Timervalues{
	double NSG_car;
	double NSB_ped;
	double NSTG_car;
	double NSY_car;
	double NSTY_car;
	double NSR_clear;
	double NSTR_clear;

	double EWG_car;
	double EWB_ped;
	double EWTG_car;
	double EWY_car;
	double EWTY_car;
	double EWR_clear;
	double EWTR_clear;
};

struct Timervalues times;

void setTimerValues()
{
	times.NSG_car 	= 4;
	times.NSB_ped 	= 1;
	times.NSTG_car 	= 4;
	times.NSY_car 	= 2;
	times.NSTY_car 	= 2;
	times.NSR_clear	= 1;
	times.NSTR_clear= 1;

	times.EWG_car	= 4;
	times.EWB_ped	= 1;
	times.EWTG_car	= 4;
	times.EWY_car	= 2;
	times.EWTY_car	= 2;
	times.EWR_clear = 1;
	times.EWTR_clear= 1;
}

typedef union
{
	struct _pulse pulse;
	// your other message structures would go here too
} my_message_t;
timer_t                 timer_id;
int                     chid;
my_message_t            msg;


// timer variables
struct itimerspec itime1;


void DoSomething0()
{
	printf("In state0: NoTrain_0\n");
	startOneTimeTimer(timer_id, times.NSR_clear);
	//timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething1()
{
	printf("In state1: TrainApproaching_1\n");
	startOneTimeTimer(timer_id, times.EWTG_car);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething2()
{
	printf("In state2: TrainApproaching_2\n");
	startOneTimeTimer(timer_id, times.EWTY_car);
	MsgReceive(chid, &msg, sizeof(msg), NULL);

}
void DoSomething3()
{
	printf("In state3: TrainCrossing_3\n");
	startOneTimeTimer(timer_id, times.EWTR_clear);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething4()
{
	printf("In state4: TrainLeaving_4\n");
	startOneTimeTimer(timer_id, times.EWG_car-times.EWB_ped);
	MsgReceive(chid, &msg, sizeof(msg), NULL);

}
void DoSomething5()
{
	printf("In state5: TrainLeaving_5\n");
	startOneTimeTimer(timer_id, times.EWY_car);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}


////////////////// - Timers -//////////////////////////////////////
void startOneTimeTimer(timer_t timerID, double time)
{
	double x = time;
	x += 0.5e-9;
	itime1.it_value.tv_sec = (long) x;
	itime1.it_value.tv_nsec = (x - itime1.it_value.tv_sec) * 1000000000L;
	//printf("%ld %ld\n", itime1.it_value.tv_sec, itime1.it_value.tv_nsec);
	timer_settime(timerID, 0, &itime1, NULL);
}

void initTimer()
{
	chid = ChannelCreate(0); // Create a communications channel

	struct sigevent         event;
	event.sigev_notify = SIGEV_PULSE;

	// create a connection back to ourselves for the timer to send the pulse on
	event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL, 0);
	if (event.sigev_coid == -1)
	{
		printf(stderr, "%s:  couldn't ConnectAttach to self!\n", progname);
		perror(NULL);
		exit(EXIT_FAILURE);
	}
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
}





void *keyboard(void *notused)
{
	char NewTrainKey[3];
	while(1)
	{
		scanf(" %s",&NewTrainKey);
		pthread_mutex_lock(&mutex);
		strcpy(NewTrainGlobal,NewTrainKey);
		pthread_mutex_unlock(&mutex);
		printf("'%s'\n",&NewTrainGlobal);
	}
}

enum states BoomGateSequence(void *CurrentState)
{
	enum states CurState = *(enum states*)CurrentState;

	static char NewTrainReceive[3];

	switch (CurState){
	case NoTrain_0:
		DoSomething0();
		while(CurState == NoTrain_0){
		pthread_mutex_lock(&mutex);
		strcpy(NewTrainReceive,NewTrainGlobal);
		strcpy(NewTrainGlobal,"aaa");
		pthread_mutex_unlock(&mutex);
		if(strcmp("T",NewTrainReceive) == 0)
		{
			printf("Train approaching\n");
			CurState = TrainApproaching_1;
		}
		else
		{
			CurState = NoTrain_0;
		}
		}

		break;
	case TrainApproaching_1:
		DoSomething1();
		CurState = TrainApproaching_2;
		break;
	case TrainApproaching_2:
		DoSomething2();
		CurState = TrainCrossing_3;
		break;
	case TrainCrossing_3:
		DoSomething3();
		CurState = TrainLeaving_4;
		break;
	case TrainLeaving_4:
		DoSomething4();
		CurState = TrainLeaving_5;
		break;
	case TrainLeaving_5:
		DoSomething5();
		CurState = NoTrain_0;
		break;
	}
	return CurState;
}

enum BoomGate_mode mode = NORMAL;

void *stateMachineThread()
{
	int Runtimes=30, counter = 0;
	enum states CurrentState=0; // Declaring the enum within the main
	// means we will need to pass it by address

	while (counter < Runtimes)
	{
		switch (mode) {
		case NORMAL:
			printf("Normal Sequence> \t");
			CurrentState = BoomGateSequence( &CurrentState ); // pass address
			counter++;
			break;
		case SPECIAL:
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

	printf("Boom gate controller started\n");


	initTimer();
	setTimerValues();

	pthread_create(&keyboarInput, NULL, keyboard, NULL);
	pthread_create(&stateMachine,NULL,stateMachineThread,NULL);

	pthread_join(stateMachine,NULL);

}
