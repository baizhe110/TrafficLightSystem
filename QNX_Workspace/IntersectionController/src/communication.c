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
#include "stateTasks.h"
#include "defines.h"
#include <time.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>
#include <errno.h>
#include <semaphore.h>

char *prognames = "timer_per1.c";

TrainApproachint = 0;

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

//	printf("%ld %ld\n", periodicTime.it_value.tv_sec, periodicTime.it_value.tv_nsec);
	timer_settime(periodicTimer_id, 0, &periodicTime, NULL);


	//char *sname = "/net/VM_x86_Target02/dev/name/local/CentralServer";
	char *sname = (char *)sname_data;
	my_data msg;
	my_reply reply;
	//msg.ClientID = 800; // unique number for this client (optional)
	strcpy(msg.ClientID, "Intersection 1");
	msg.type = intersectionType;

	int server_coid;
	int index = 0;

	printf("\n");
	printf("  ---> Trying to connect to server named: %s\n", sname);
	printf("\n");

	while((server_coid = name_open(sname, 0)) == -1)
	{
		//printf("Could not connect to server!\n");
		sleep(1);
	}
	printf("\n");
	printf("Connection established to: %s\n", sname);
	printf("\n");
	// We would have pre-defined data to stuff here
	msg.hdr.type = 0x00;
	msg.hdr.subtype = 0x00;

	// Do whatever work you wanted with server connection
	while (1) // send data packets
	{
		// set up data packet
		//msg.data=10+index;
		msg.state=CurrentState;

		// the data we are sending is in msg.data
		//printf("Client (ID:%d), sending data packet with the integer value: %d \n", msg.ClientID, msg.data);
		fflush(stdout);

		if (MsgSend(server_coid, &msg, sizeof(msg), &reply, sizeof(reply)) == -1)
		{
			printf(" Error data '%d' NOT sent to server\n", msg.data);
			printf("SendMsg: errno %d\n", errno);
			if (errno!=0) {
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

			// check if data is for updating timing values
			//printf("reply data type: %d\n", reply.data);
			if (reply.data == 2) {
				char currentText[100];
				double timingValues[14];
				int desiredMode, type, nodeNumber, dataType;
				int count = 0, i=0 , wordNumber = 0;
				while(reply.buf[count] != '\0')
				{
					i=0;
					while(reply.buf[count] != ',' && reply.buf[count] != '\0')
					{
						currentText[i] = reply.buf[count];
						i++;
						count++;
					}
					currentText[i] = '\0';
					timingValues[wordNumber] = atof(currentText);
					count++;
					printf("%lf,",timingValues[wordNumber]);
					wordNumber++;

				}
				printf("\n word number %d\n", wordNumber-1);
				struct Timervalues t;
				t.NSG_car 	= timingValues[0];
				t.NSB_ped 	= timingValues[1];
				t.NSTG_car 	= timingValues[2];
				t.NSY_car 	= timingValues[3];
				t.NSTY_car 	= timingValues[4];
				t.NSR_clear	= timingValues[5];
				t.NSTR_clear= timingValues[6];

				t.EWG_car	= timingValues[7];
				t.EWB_ped	= timingValues[8];
				t.EWTG_car	= timingValues[9];
				t.EWY_car	= timingValues[10];
				t.EWTY_car	= timingValues[11];
				t.EWR_clear = timingValues[12];
				t.EWTR_clear= timingValues[13];
				setTimerValues(t);
				printf("timingValues updated\n");
			}
			// check mode for updating. If it is synced fixed light seq then open sem for sync
			if (CurrentMode != reply.mode && switchingMode == 0) {
				if (CurrentMode == FIXED_SYNCED && reply.mode == FIXED)
				{
					CurrentMode = reply.mode;
				} else
				{
					switchingMode = 1;
					desiredMode = reply.mode;
					printf("Switching to Mode %d\n", desiredMode);
					if (desiredMode == FIXED_SYNCED) {
						if ((sem_sync = sem_open(attachPointSem, NULL)) == SEM_FAILED) {
							printf("failed to open semaphore %d\n", errno);
						}
						else
						{
							printf("Semaphore opened\n");
							syncing = 1;
						}

					}
				}
			}
		}

		TrainApproachint = reply.TrainApproach;
		MsgReceive(chid, &msg, sizeof(msg), NULL);

	}

	// Close the connection
	printf("\n Sending message to server to tell it to close the connection\n");
	name_close(server_coid);
}
