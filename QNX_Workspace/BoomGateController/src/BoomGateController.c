/*********************************************************************
 *						BOOMGATE CONTROLLER
 *
 * 				Includes:
 * 					1. Set timer values
 * 					2. Initialization of timer
 * 					3. Timer function that counts
 * 					4. StateTime functions
 * 					5. Keyboard input function
 * 					6. State machine
 * 					7. State machine (main)
 * 					8. Client (Native message passing)
 * 					9. MAIN (For boom gate controller)
 *********************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>

#include "BoomGateController.h"




/*********************************************************************
 *	Declaring and defining variables etc.
 *********************************************************************/
char *progname = "timer_per1.c";

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // needs to be set to PTHREAD_MUTEX_INITIALIZER;

char NewTrainGlobal[1]="a";
enum BoomGate_mode CurrentMode;
enum states CurrentState; // Declaring the enum within the main

// for timer and message receive

typedef union
{
	struct _pulse pulse;
	// your other message structures would go here too
} my_message_t;
timer_t                 timer_id;
//int                     chid;
my_message_t            msg;
int                     chid_Timer;
int TrainApproachint = 0;

// timer variables
struct itimerspec itime1;
struct Timervalues times;





/*********************************************************************
 *	Set timer values for states
 *********************************************************************/
void setTimerValues()
{
	times.NoTrain 			= 1;	// No train time is zero to ensure zero waiting time IF train approaching
	times.TrainApproaching1 = 2;	// Blinking red lights (before boom gates go down)
	times.TrainApproaching2 = 2;	// Boom gates going down, red lights blink
	times.TrainCrossing 	= 4;	// Train crossing, boom gates down, red lights blink
	times.TrainLeaving1 	= 2;	// Boom gates going up, red lights blink
	times.TrainLeaving2		= 2;	// Boom gates up, red lights blink
}





/*********************************************************************
 *	Initialization of timer for Statetime functions
 *********************************************************************/
void initTimer()
{
	chid_Timer= ChannelCreate(0); // Create a communications channel

	struct sigevent         event;
	event.sigev_notify = SIGEV_PULSE;

	// create a connection back to ourselves for the timer to send the pulse on
	event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid_Timer, _NTO_SIDE_CHANNEL, 0);
	if (event.sigev_coid == -1)
	{
		printf(stderr, "%s:  couldn't ConnectAttach to self!\n", progname);
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	struct sched_param th_param;
	pthread_getschedparam(pthread_self(), NULL, &th_param);
	event.sigev_priority = th_param.sched_curpriority;
	event.sigev_code = MY_PULSE_CODE;

	// create the timer, binding it to the event
	if (timer_create(CLOCK_REALTIME, &event, &timer_id) == -1)
	{
		printf (stderr, "%s:  couldn't create a timer, errno %d\n", progname, errno);
		perror (NULL);
		exit (EXIT_FAILURE);
	}
}





/*********************************************************************
 *	Timer that counts for the specified input time (Used for the StateTime functions)
 *********************************************************************/
void startOneTimeTimer(timer_t timerID, double time)
{
	double x = time;
	x += 0.5e-9;
	itime1.it_value.tv_sec = (long) x;
	itime1.it_value.tv_nsec = (x - itime1.it_value.tv_sec) * 1000000000L;
	timer_settime(timerID, 0, &itime1, NULL);
}







/*********************************************************************
 *	StateTime functions for the state machine
 *********************************************************************/
void StateTime0()
{
	printf("In state0: NoTrain_0\n");
	startOneTimeTimer(timer_id, times.NoTrain);
	MsgReceive(chid_Timer, &msg, sizeof(msg), NULL);
}
void StateTime1()
{
	printf("In state1: TrainApproaching_1\n");
	startOneTimeTimer(timer_id, times.TrainApproaching1);
	MsgReceive(chid_Timer, &msg, sizeof(msg), NULL);
}
void StateTime2()
{
	printf("In state2: TrainApproaching_2\n");
	startOneTimeTimer(timer_id, times.TrainApproaching2);
	MsgReceive(chid_Timer, &msg, sizeof(msg), NULL);
}
void StateTime3()
{
	printf("In state3: TrainCrossing_3\n");
	startOneTimeTimer(timer_id, times.TrainCrossing);
	MsgReceive(chid_Timer, &msg, sizeof(msg), NULL);
}
void StateTime4()
{
	printf("In state4: TrainLeaving_4\n");
	startOneTimeTimer(timer_id, times.TrainLeaving1);
	MsgReceive(chid_Timer, &msg, sizeof(msg), NULL);

}
void StateTime5()
{
	printf("In state5: TrainLeaving_5\n");
	startOneTimeTimer(timer_id, times.TrainLeaving2);
	MsgReceive(chid_Timer, &msg, sizeof(msg), NULL);
}








/*********************************************************************
 *			KEYBOARD INPUT FOR INCOMING CARS AND PEDESTRIANS
 *			Keyboard inputs:
 *					1. Train approaching: 	"T"
 *********************************************************************/
void *keyboard(void *notused)
{
	char NewTrainKey[1];
	while(1)
	{
		scanf(" %s",&NewTrainKey);
		pthread_mutex_lock(&mutex);
		strcpy(NewTrainGlobal,NewTrainKey);
		pthread_mutex_unlock(&mutex);
		printf("Done Reading input\n");
	}
}




/*********************************************************************
 *							STATE MACHINE
 *********************************************************************/
enum states BoomGateSequence(void *CurrentState)
{
	enum states CurState = *(enum states*)CurrentState;

	static char NewTrainReceive[1];

	switch (CurState){
	case NoTrain_0:
		StateTime0();
		while(CurState == NoTrain_0 && CurrentMode == NORMAL){
			pthread_mutex_lock(&mutex);
			strcpy(NewTrainReceive,NewTrainGlobal);
			strcpy(NewTrainGlobal,"aaa");
			pthread_mutex_unlock(&mutex);
			if(strcmp("T",NewTrainReceive) == 0)
			{
				printf("Train approaching\n");
				CurState = TrainApproaching_1;
				pthread_mutex_lock(&mutex);
				TrainApproachint = 1;
				pthread_mutex_unlock(&mutex);
			}
			else
			{
				CurState = NoTrain_0;
			}
		}

		break;
	case TrainApproaching_1:
		StateTime1();
		CurState = TrainApproaching_2;
		break;
	case TrainApproaching_2:
		StateTime2();
		CurState = TrainCrossing_3;
		break;
	case TrainCrossing_3:
		StateTime3();
		while(CurrentMode == SPECIAL)
		{
		}
		CurState = TrainLeaving_4;
		break;
	case TrainLeaving_4:
		StateTime4();
		CurState = TrainLeaving_5;
		break;
	case TrainLeaving_5:
		StateTime5();
		CurState = NoTrain_0;
		pthread_mutex_lock(&mutex);
		TrainApproachint = 0;
		pthread_mutex_unlock(&mutex);
		break;
	}
	return CurState;
}





/*********************************************************************
 *					STATE MACHINE (MAIN) THREAD
 *			Controls which mode to be in (Normal SM or Special)
 *********************************************************************/
void *stateMachineThread()
{
	// means we will need to pass it by address

	while (1)
	{
		switch (CurrentMode) {
		case NORMAL:
			printf("Normal Sequence> \t");
			CurrentState = BoomGateSequence( &CurrentState );
			break;
		case SPECIAL:
			if (CurrentState == NoTrain_0)
			{
				CurrentState = TrainApproaching_1;
			}
			else
			{
				printf("Special mode> \t");
				CurrentState = BoomGateSequence( &CurrentState );
			}
			break;
		default:
			break;
		}
	}
}





/*********************************************************************
 * 					Client (Native message passing)
 *********************************************************************/
void *ClientBoomGate(void *notused){


	/********** Setup Stuff and error cheching **********/
	// Timer Setup
	struct sigevent         event;
	struct itimerspec       periodicTime;
	timer_t                 periodicTimer_id;
	int                     chid;
	chid = ChannelCreate(1); // Create a communications channel

	//struct sigevent         event;
	event.sigev_notify = SIGEV_PULSE;

	// create a connection back to ourselves for the timer to send the pulse on
	event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL, 0);
	if (event.sigev_coid == -1)
	{
		//printf(stderr, "%s:  couldn't ConnectAttach to self!\n", prognames);
		perror(NULL);
		pthread_exit(EXIT_FAILURE);
	}
	struct sched_param th_param;
	pthread_getschedparam(pthread_self(), NULL, &th_param);
	event.sigev_priority = th_param.sched_curpriority;    // old QNX660 version getprio(0);
	event.sigev_code = MY_PULSE_CODE;

	// create the timer, binding it to the event
	if (timer_create(CLOCK_REALTIME, &event, &periodicTimer_id) == -1)
	{
		//printf (stderr, "%s:  couldn't create a timer, errno %d\n", prognames, errno);
		perror (NULL);
		pthread_exit(EXIT_FAILURE);
	}
	// setup the timer
	double x = 1;
	x += 0.5e-9;
	periodicTime.it_value.tv_sec = (long) x;
	periodicTime.it_value.tv_nsec = (x - periodicTime.it_value.tv_sec) * 1000000000L;
	periodicTime.it_interval.tv_sec = (long) x;
	periodicTime.it_interval.tv_nsec = (x - periodicTime.it_interval.tv_sec) * 1000000000L;

	//	printf("%ld %ld\n", periodicTime.it_value.tv_sec, periodicTime.it_value.tv_nsec);
	timer_settime(periodicTimer_id, 0, &periodicTime, NULL);


	//char *sname = "/net/VM_x86_Target02/dev/name/local/CentralServer";

	my_data msg;		// Data send to server
	my_reply reply;		// Data received from server

	strcpy(msg.ClientID, "BoomGate");
	msg.type = BoomGate;

	int server_coid;

	printf("\n");
	printf("  ---> Trying to connect to server named: %s\n", QNET_ATTACH_POINT);
	printf("\n");

	while((server_coid = name_open(QNET_ATTACH_POINT, 0)) == -1)
	{
		printf("Could not connect to server!\n");
		sleep(1);
	}

	printf("\n");
	printf("Connection established to: %s\n", QNET_ATTACH_POINT);
	printf("\n");

	// We would have pre-defined data to stuff here
	msg.hdr.type = 0x00;
	msg.hdr.subtype = 0x00;




	/*********************************************************************
	 *				Here we send and receive data to and from the server
	 *			Data sent:
	 *				1. CurrentState send through msg.state
	 *				2. If train is approaching, set in msg.TrainApproach
	 *			Data received:
	 *				1. CurrentMode received in reply.mode
	 *********************************************************************/
	while (1) // send data packets
	{

		msg.state=CurrentState;
		pthread_mutex_lock(&mutex);
		msg.TrainApproach = TrainApproachint;
		pthread_mutex_unlock(&mutex);


		fflush(stdout);


		/******** Here we send data, and check for errors. *********/
		if (MsgSend(server_coid, &msg, sizeof(msg), &reply, sizeof(reply)) == -1)
		{
			printf(" Error data '%d' NOT sent to server\n", msg.data);
			if (errno!=0) {
				printf("trying to reconnect to server");
				while((server_coid = name_open(QNET_ATTACH_POINT, 0)) == -1)
				{
					printf("Could not reconnect to server!\n");
					sleep(1);
				}
				// continue with sending next pice of data to server
				continue;
			}
			else
			{
				// break if other error than connection lost to server
				break;
			}
			// maybe we did not get a reply from the server
			break;
		}

		/******* Here we received the data from the server ******/
		else
		{
			CurrentMode = reply.mode;
		}
		MsgReceive(chid, &msg, sizeof(msg), NULL);
	}

	// Close the connection
	printf("\n Sending message to server to tell it to close the connection\n");
	name_close(server_coid);
}







/*********************************************************************
 *							BOOMGATE MAIN
 * 				Starting point for the boomgate node!
 *********************************************************************/
int main(int argc, char *argv[])
{

	//Declaring threads
	pthread_t stateMachine, keyboarInput, ClientBoomGateThread;

	//Initialize variables, specifying what mode to start up in
	CurrentMode = NORMAL;
	CurrentState = NoTrain_0;

	//Getting the hostname and printing to console that we now start intersection
	printf("Boom Gate Controller starts now...\n");
	char hostnm[100];
	memset(hostnm, '\0', 100);
	hostnm[99] = '\n';
	gethostname(hostnm, sizeof(hostnm));
	printf("--> Machine hostname is: '%s'\n", hostnm);
	printf("Boom gate startup mode: %d\n", CurrentMode);
	printf("Boom gate startup state: %d\n",CurrentState);
	printf("\n");



	/******   Setting up the QNX timer to be used in the timing of the traffic lights (states)  ******/
	initTimer();
	setTimerValues();


	// Starting up threads
	pthread_create(&ClientBoomGateThread, NULL, ClientBoomGate, NULL);		// Communication between boomgate(client) and CentralServer(server)
	pthread_create(&keyboarInput, NULL, keyboard, NULL);					// Keyboard input
	pthread_create(&stateMachine,NULL,stateMachineThread,NULL);				// State machine

	pthread_join(stateMachine,NULL);

}

