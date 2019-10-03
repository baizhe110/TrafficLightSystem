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


void *ex_client(void *sname_data)
{
	char *sname = "/net/VM_x86_Target02/dev/name/local/CentralServer";
	my_data msg;
	my_reply reply;

	msg.ClientID = 800; // unique number for this client (optional)

	int server_coid;
	int index = 0;

	printf("  ---> Trying to connect to server named: %s\n", sname);
	if ((server_coid = name_open(sname, 0)) == -1)
	{
		printf("\n    ERROR, could not connect to server!\n\n");
		pthread_exit(EXIT_FAILURE);
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
		printf("Client (ID:%d), sending data packet with the integer value: %d \n", msg.ClientID, msg.data);
		fflush(stdout);

		if (MsgSend(server_coid, &msg, sizeof(msg), &reply, sizeof(reply)) == -1)
		{
			printf(" Error data '%d' NOT sent to server\n", msg.data);
			// maybe we did not get a reply from the server
			break;
		}
		else
		{ // now process the reply
			printf("   -->Reply is: '%s'\n", reply.buf);
		}

		sleep(1);	// wait a few seconds before sending the next data packet
	}

	// Close the connection
	printf("\n Sending message to server to tell it to close the connection\n");
	name_close(server_coid);
}
