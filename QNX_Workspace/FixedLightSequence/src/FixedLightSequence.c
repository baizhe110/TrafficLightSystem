#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>
#include <errno.h>


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
	EWR_NSY_EWTR_NSTR_11};
enum states CurState;
//--- --- --- ---

void Initialise()
{
	printf("In state0: EWR-NSR\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething0()
{
	printf("In state1: EWR-NSR\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething1()
{
	printf("In state1: EWR-NSR\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething2()
{
	printf("In state2: EWG-NSR\n");
	timer_settime(timer_id, 0, &itime2, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);

}
void DoSomething3()
{
	printf("In state3: EWY-NSR\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething4()
{
	printf("In state4: EWR-NSR\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething5()
{
	printf("In state5: EWR-NSG\n");
	timer_settime(timer_id, 0, &itime2, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething6()
{
	printf("In state6: EWY-NSY\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}

enum states SingleStep_TrafficLight_SM(void *CurrentState)
{

	enum states CurState = *(enum states*)CurrentState;

	//CurState = CurrentState;

initialise();

	switch (CurState)

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
		Dosomething10();
		CurState = EWR_NSY_EWTR_NSTR_11;
		break;
	case EWR_NSY_EWTR_NSTR_11:
		DoSomething11();
		CurState = EWR_NSY_EWTR_NSTR_0;
	}
	return CurState;
}


int main(int argc, char *argv[])
{
	printf("Fixed Sequence Traffic Lights State Machine\n");
	int Runtimes=30, counter = 0;
	enum states CurrentState=0; // Declaring the enum within the main
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

	while (counter < Runtimes)
	{
		CurrentState = SingleStep_TrafficLight_SM( &CurrentState ); // pass address
		counter++;
	}
}
