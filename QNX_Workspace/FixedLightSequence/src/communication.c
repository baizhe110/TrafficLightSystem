/*
 * communication.c
 *
 *  Created on: 3 Oct 2019
 *      Author: bruno
 */
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <share.h>
#include <pthread.h>
#include "communication.h"
#include "defines.h"
#include <time.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>
#include <errno.h>

char *prognames = "timer_per1.c";

// Sends current state to central controller every xxx seconds
void *ex_client(void *sname_data)
{
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

	printf("%ld %ld\n", periodicTime.it_value.tv_sec, periodicTime.it_value.tv_nsec);
	timer_settime(periodicTimer_id, 0, &periodicTime, NULL);


	//char *sname = "/net/VM_x86_Target02/dev/name/local/CentralServer";
	char *sname = (char *)sname_data;
	my_data msg;
	my_reply reply;
	msg.ClientID = 800; // unique number for this client (optional)

	int server_coid;
	int index = 0;

	printf("  ---> Trying to connect to server named: %s\n", sname);

	while((server_coid = name_open(sname, 0)) == -1)
	{
		printf("Could not connect to server!\n");
		sleep(1);
	}

	printf("Connection established to: %s\n", sname);

	// We would have pre-defined data to stuff here
	msg.hdr.type = 0x00;
	msg.hdr.subtype = 0x00;

	// Do whatever work you wanted with server connection
	while (1) // send data packets
	{
		// set up data packet
		//msg.data=10+index;
		msg.data=CurrentState;

		// the data we are sending is in msg.data
		//printf("Client (ID:%d), sending data packet with the integer value: %d \n", msg.ClientID, msg.data);
		fflush(stdout);

		if (MsgSend(server_coid, &msg, sizeof(msg), &reply, sizeof(reply)) == -1)
		{
			printf(" Error data '%d' NOT sent to server\n", msg.data);
			printf("SendMsg:  couldn't create a timer, errno %d\n", errno);
			if (errno==3) {
				printf("trying to reconnect to server");
				while((server_coid = name_open(sname, 0)) == -1)
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
		}
		else
		{ // now process the reply
			//printf("   -->Reply is: '%s'\n", reply.buf);
			if (CurrentMode != reply.data && switchingMode == 0) {
				switchingMode = 1;
				desiredMode = reply.data;
				printf("Switching to Mode %d\n", desiredMode);
			}


		}
		MsgReceive(chid, &msg, sizeof(msg), NULL);
		//sleep(1);	// wait a few seconds before sending the next data packet
	}

	// Close the connection
	printf("\n Sending message to server to tell it to close the connection\n");
	name_close(server_coid);
}