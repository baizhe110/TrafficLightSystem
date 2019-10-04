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

#include "communication.h"

#define maxClients 10

char *prognames = "timer_per1.c";
int currentMode = 0;
int clientsAlive = 0;
int clientsDead = 0;
struct timespec clientLastAlive[maxClients];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

///////////////////////////////////////// Keyboard input
void *keyboard(void *notused)
{
	char newMode[3];
	while(1)
	{
		scanf(" %s",&newMode);
		pthread_mutex_lock(&mutex);
		currentMode = newMode[0]-'0';
		pthread_mutex_unlock(&mutex);
		printf("Mode changed to '%d'\n", currentMode);
	}
}

/////////////////////////////////////////// Server code
void *server()
{
	name_attach_t *attach;
	// Create a local name (/dev/name/...)
	if ((attach = name_attach(NULL, ATTACH_POINT, 0)) == NULL)
	{
		printf("\nFailed to name_attach on ATTACH_POINT: %s \n", ATTACH_POINT);
		printf("\n Possibly another server with the same name is already running !\n");
		return EXIT_FAILURE;
	}

	printf("Server Listening for Clients on ATTACH_POINT: %s \n", ATTACH_POINT);

	my_data msg;
	int rcvid=0, msgnum=0;  		// no message received yet
	int Stay_alive=1, living=0;	// server stays running (ignores _PULSE_CODE_DISCONNECT request)

	my_reply replymsg; // replymsg structure for sending back to client
	replymsg.hdr.type = 0x01;
	replymsg.hdr.subtype = 0x00;

	living =1;
	while (living)
	{
		// Do your MsgReceive's here now with the chid
		rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);

		if (rcvid == -1)  // Error condition, exit
		{
			printf("\nFailed to MsgReceive\n");
			break;
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

	if( clock_gettime( CLOCK_REALTIME, &clientLastAlive[msg->ClientID-800]) == -1 ) {
		printf( "clock gettime error" );
	}

	// put your message handling code here and assemble a reply message
	sprintf(replymsg.buf, "Current Mode: %d", currentMode);
	replymsg.data = currentMode;
	//sprintf(replymsg.buf, "Current Mode: %d", 1);
	printf("current Intersection state '%d' from client (ID:%d)\n", msg->data, msg->ClientID);

	fflush(stdout);
	sleep(1); // Delay the reply by a second (just for demonstration purposes)

	//printf("\n    -----> replying with: '%s'\n",replymsg.buf);
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
	while(1)
	{
		// wait for message/pulse
		MsgReceive(chid, NULL, NULL, NULL);

		if( clock_gettime( CLOCK_REALTIME, &now) == -1 ) {
			printf( "clock gettime error" );
		}
		clientsAlive = 0;
		clientsDead = 0;
		for (int i = 0; i < maxClients; ++i) {
			if ((now.tv_sec - clientLastAlive[i].tv_sec) < 5) {
				clientsAlive++;
			}
			else
			{
				clientsDead++;
			}
		}
		printf("clientsAlive: %d\t clientsDead: %d \n", clientsAlive, clientsDead);
	}
	pthread_exit(EXIT_SUCCESS);
}

