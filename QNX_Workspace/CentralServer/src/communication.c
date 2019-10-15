/*
 * communication.c
 *
 *  Created on: 4 Oct 2019
 *      Author: bruno
 */
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/dispatch.h>
#include <pthread.h>
#include <sys/netmgr.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/neutrino.h>>
#include <stdint.h>

#include "communication.h"

#define maxClients 3

char *prognames = "timer_per1.c";
int currentMode[maxClients];
int clientsAlive = 0;
int clientsDead = 0;
int disattachPoint = 0; //for disattach message channel when shutdown central servewr
struct timespec clientLastAlive[maxClients], startTime;
int clientStatus[maxClients], clientType[maxClients], clientState[maxClients];

TrainApproachint = 0;


//for syncing
sem_t *sem_sync;
int synced = 0;
int semOpen = 0;
//for sending data in reply
int replyDataType = 0;
char replyData[100];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


///////////////////////////////////////// Keyboard input
void *keyboard(void *notused)
{
	char newInput[100], currentText[100];
	int desiredMode, type, nodeNumber, dataType;
	int count = 0, i=0 , wordNumber = 0;
	while(1)
	{
		scanf(" %s",&newInput);
		count=0;
		type = -1;
		wordNumber = 0;
		while(newInput[count] != '\0')
		{
			i=0;
			while(newInput[count] != ';' && newInput[count] != '\0')
			{
				currentText[i] = newInput[count];
				i++;
				count++;
			}
			currentText[i] = '\0';
			count++;
			wordNumber++;
			switch (wordNumber) {
			case 1:
				if(strcmp("intersection1",currentText) == 0)
				{
					type = Intersection1;
				}
				else if(strcmp("intersection2",currentText) == 0)
				{
					type = Intersection2;
				}
				else if(strcmp("boom",currentText) == 0)
				{
					type = BoomGate;
				}
				else if(strcmp("special",currentText) == 0)
				{
					type = SPECIAL;
				}
				else if(strcmp("stop",currentText) == 0)
				{
					pthread_mutex_lock(&mutex);
					disattachPoint = 1;
					pthread_mutex_unlock(&mutex);
					printf("Stop command received, stopping all services...\n");
				}
				break;
			case 2:
				dataType= atoi(currentText);
				break;
			case 3:
				if (dataType == 1) {
					pthread_mutex_lock(&mutex);
					currentMode[type] = atoi(currentText);
					pthread_mutex_unlock(&mutex);
					printf("Mode of Type %d changed to '%d'\n",type, currentMode[type]);
					if ((type==Intersection1 || type==Intersection2) && currentMode[type] <= 2)
					{
						currentMode[Intersection1] = currentMode[type];
						currentMode[Intersection2] = currentMode[type];
					}
					if (currentMode[type] == FIXED_SYNCED && (type==Intersection1 || type==Intersection2)  ) {
						synced = 0;
					}
				}
				else if(dataType == 2)
				{
					replyDataType = dataType;
					strcpy(replyData, currentText);
					printf("changing timing values\n");
					//get timing for intersection
				}
				else if(dataType == 3)
				{

				}

				break;
			default:
				break;
			}
		}
	}
}

/////////////////////////////////////////// Server code
void *server()
{
	struct sigevent         timeOutEvent;
	timeOutEvent.sigev_notify = SIGEV_UNBLOCK;

	uint64_t timeout = 10000000000;		//for timeout service, 10s
	// setting node modes to default
	for (int i = 0; i < maxClients; ++i) {
		currentMode[i] = 0;
	}
	name_attach_t *attach;
	// Create a local name (/dev/name/...)
	if ((attach = name_attach(NULL, attachPoint, 0)) == NULL)
	{
		printf("\nFailed to name_attach on attachPoint: %s \n", attachPoint);
		printf("\n Possibly another server with the same name is already running !\n");
		return EXIT_FAILURE;
	}

	printf("Server Listening for Clients on attachPoint: %s \n", attachPoint);

	my_data msg;
	int rcvid=0, msgnum=0;  		// no message received yet
	int Stay_alive=1, living=0;	// server stays running (ignores _PULSE_CODE_DISCONNECT request)

	my_reply replymsg; // replymsg structure for sending back to client
	replymsg.hdr.type = 0x01;
	replymsg.hdr.subtype = 0x00;

	living =1;
	clock_gettime( CLOCK_REALTIME, &startTime);
	while (living)
	{
		if (disattachPoint == 1) 		//if receive stop command, stop the while loop.
		{
			pthread_mutex_lock(&mutex);
			living = 0;
			pthread_mutex_unlock(&mutex);

		}
		TimerTimeout( CLOCK_REALTIME, _NTO_TIMEOUT_RECEIVE,&timeOutEvent, &timeout, NULL ); //if no msg received after 10s, unblock the receive state

		rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);

		if (rcvid == -1)  // Error condition, exit
		{
			printf("\nFailed to MsgReceive\n");
			continue;
		}

		// did we receive a Pulse or message?
		// for Pulses:
		if (rcvid == 0)  //  Pulse received, work out what type
		{
			switch (msg.hdr.code)
			{
			case _PULSE_CODE_DISCONNECT:
				// A client disconnected all its connections by running
				// name_close() for each name_open()  or terminated
				if( Stay_alive == 0)
				{
					ConnectDetach(msg.hdr.scoid);
					printf("\nServer was told to Detach from ClientID:%d ...\n", msg.ClientID);
					living = 0; // kill while loop
					continue;
				}
				else
				{
					printf("\nServer received Detach pulse from ClientID:%d but rejected it ...\n", msg.ClientID);
				}
				break;

			case _PULSE_CODE_UNBLOCK:
				// REPLY blocked client wants to unblock (was hit by a signal
				// or timed out).  It's up to you if you reply now or later.
				printf("\nServer got _PULSE_CODE_UNBLOCK after %d, msgnum\n", msgnum);
				break;

			case _PULSE_CODE_COIDDEATH:  // from the kernel
				printf("\nServer got _PULSE_CODE_COIDDEATH after %d, msgnum\n", msgnum);
				break;

			case _PULSE_CODE_THREADDEATH: // from the kernel
				printf("\nServer got _PULSE_CODE_THREADDEATH after %d, msgnum\n", msgnum);
				break;

			default:
				// Some other pulse sent by one of your processes or the kernel
				printf("\nServer got some other pulse after %d, msgnum\n", msgnum);
				break;

			}
			continue;// go back to top of while loop
		}

		// for messages:
		if(rcvid > 0) // if true then A message was received
		{

			msgnum++;
			if (disattachPoint == 1) 		//if receive stop command, stop the server phtread.
			{
				pthread_mutex_lock(&mutex);
				living = 0;
				pthread_mutex_unlock(&mutex);
				//				name_detach(attach, 0);
				//				printf("Communication channel detached.");
			}
			// If the Global Name Service (gns) is running, name_open() sends a connect message. The server must EOK it.
			if (msg.hdr.type == _IO_CONNECT )
			{
				MsgReply( rcvid, EOK, NULL, 0 );
				printf("\n gns service is running....");
				continue;	// go back to top of while loop
			}
			// Some other I/O message was received; reject it
			if (msg.hdr.type > _IO_BASE && msg.hdr.type <= _IO_MAX )
			{
				MsgError( rcvid, ENOSYS );
				printf("\n Server received and IO message and rejected it....");
				continue;	// go back to top of while loop
			}
			// A message (presumably ours) received
			handleServerMessages(&rcvid, &msg);
		}
		else
		{
			printf("\nERROR: Server received something, but could not handle it correctly\n");
		}
	}
	// Remove the attach point name from the file system (i.e. /dev/name/local/<myname>)
	name_detach(attach, 0);
	printf("Communication service has stopped\n.");
	pthread_exit(EXIT_SUCCESS);
}

/////////////////////////////////////////////////////////////////////// Handle Server received Messages
void *handleServerMessages(void *rcvid_passed, void *msg_passed)
{
	my_data *msg = (my_data *)msg_passed;
	int rcvid = *(int *)rcvid_passed;

	my_reply replymsg; // replymsg structure for sending back to client
	replymsg.hdr.type = 0x01;
	replymsg.hdr.subtype = 0x00;

	if( clock_gettime( CLOCK_REALTIME, &clientLastAlive[msg->type]) == -1 ) {
		printf( "clock gettime error" );
	}
	//clientType[msg->ClientID] = msg->type;
	clientState[msg->type] = msg->state;
	clientType[msg->type] = msg->type;

	if(clientType[msg->type]==BoomGate)
	{
	TrainApproachint = msg->TrainApproach;
	if(TrainApproachint==1)
	{
		printf("Train approaching\n",TrainApproachint);
	}
	}
	// put your message handling code here and assemble a reply message

			// put your message handling code here and assemble a reply message

	// command mode to know what to do ex. tell slaves that every node is synched give node a name... to know if it is an train intersection

	sprintf(replymsg.buf, "Current Mode: %d", currentMode[msg->type]);
	replymsg.data = 0;
	replymsg.TrainApproach = TrainApproachint;

	// send timer values data
	if(replyDataType == 2)
	{
		replymsg.data = replyDataType;
		replyDataType = 0;
		strcpy(replymsg.buf, replyData);
	}
	replymsg.mode = currentMode[msg->type];
	//sprintf(replymsg.buf, "Current Mode: %d", 1);
	printf("current node state '%d' from client (ID:%d)\n", msg->state, msg->ClientID);

	fflush(stdout);

	if (synced == 0 && currentMode[msg->type] == FIXED_SYNCED) {
		int syncClients = 0;
		if (semOpen == 0) {
			printf("start Semaphore sync\n");
			struct timespec now;
			if( clock_gettime( CLOCK_REALTIME, &now) == -1 ) {
				printf( "clock gettime error" );
			}
			clientsAlive = 0;
			clientsDead = 0;
			for (int i = 0; i < maxClients; ++i) {
				if (clientLastAlive[i].tv_sec > startTime.tv_sec ) {
					if ((now.tv_sec - clientLastAlive[i].tv_sec) < 5) {
						if (clientType[i] == Intersection1 || clientType[i] == Intersection2) {
							syncClients++;
						}
						clientsAlive++;
						clientStatus[i] = 1;
					}
					else
					{
						clientsDead++;
						clientStatus[i] = 0;
					}
				}
			}
			sem_unlink("/sync");
			sem_sync = sem_open("/sync", O_CREAT,S_IRWXG, syncClients);
			int value= 10;
			sem_getvalue(sem_sync, &value);

			printf("Sync started for %d clients\n", value);
			semOpen = 1;
		}
		int value= 10;
		sem_getvalue(sem_sync, &value);
		if (value==0) {
			printf("Nodes sync completed\n");
			currentMode[Intersection1] = FIXED;
			currentMode[Intersection2] = FIXED;
			synced = 1;
			//sem_close(sem_sync);
			sem_unlink("/sync");
			semOpen = 0;
		}
	}

	sleep(1); // Delay the reply by a second (just for demonstration purposes)

	//printf("\n    -----> replying with: mode: '%d'\n",replymsg.mode);
	MsgReply(rcvid, EOK, &replymsg, sizeof(replymsg));

	return 0;
}

///////////////////////////////////////////////////////////////// check for alive clients
void *ex_timerCheckAlive(void * val)
{
	struct sigevent         event;
	struct itimerspec       periodicTime;
	timer_t                 periodicTimer_id;
	int                     chid;
	chid = ChannelCreate(0); // Create a communications channel

	//struct sigevent         event;
	event.sigev_notify = SIGEV_PULSE;

	// create a connection back to ourselves for the timer to send the pulse on
	event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL, 0);
	if (event.sigev_coid == -1)
	{
		printf(stderr, "%s:  couldn't ConnectAttach to self!\n", prognames);
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
		printf (stderr, "%s:  couldn't create a timer, errno %d\n", prognames, errno);
		perror (NULL);
		pthread_exit(EXIT_FAILURE);
	}

	// setup the timer
	double x = 2;
	x += 0.5e-9;
	periodicTime.it_value.tv_sec = (long) x;
	periodicTime.it_value.tv_nsec = (x - periodicTime.it_value.tv_sec) * 1000000000L;
	periodicTime.it_interval.tv_sec = (long) x;
	periodicTime.it_interval.tv_nsec = (x - periodicTime.it_interval.tv_sec) * 1000000000L;
	//printf("%ld %ld\n", periodicTime.it_value.tv_sec, periodicTime.it_value.tv_nsec);
	timer_settime(periodicTimer_id, 0, &periodicTime, NULL);

	struct timespec now;
	// while checking loop
	while(!disattachPoint) //using disattachPoint signal as flag
	{
		// wait for message/pulse
		MsgReceive(chid, NULL, NULL, NULL);

		if( clock_gettime( CLOCK_REALTIME, &now) == -1 ) {
			printf( "clock gettime error" );
		}
		clientsAlive = 0;
		clientsDead = 0;
		for (int i = 0; i < maxClients; ++i) {
			if (clientLastAlive[i].tv_sec > startTime.tv_sec ) {
				if ((now.tv_sec - clientLastAlive[i].tv_sec) < 5) {
					clientsAlive++;
					clientStatus[i] = 1;
				}
				else
				{
					clientsDead++;
					clientStatus[i] = 0;
				}
			}
		}
		printf("clientsAlive: %d\t clientsDead: %d \n", clientsAlive, clientsDead);
	}
	printf("Thread alive checking service has stopped.\n");
	pthread_exit(EXIT_SUCCESS);
}

