/*
 * communication.h
 *
 *  Created on: 4 Oct 2019
 *      Author: bruno
 */

#ifndef SRC_COMMUNICATION_H_
#define SRC_COMMUNICATION_H_

#include <sys/iofunc.h>

// Define what the channel is called. It will be located at <hostname>/dev/name/local/myname"
#define attachPoint "CentralServer"
#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL
#define BUF_SIZE 100



typedef struct
{
	struct _pulse hdr; // Our real data comes after this header
	int ClientID; // our data (unique id from client)
	int data;     // our data
	int type;
	int state;
} my_data;

typedef struct
{
	struct _pulse hdr;  // Our real data comes after this header
	char buf[BUF_SIZE]; // Message we send back to clients to tell them the messages was processed correctly.
	int data;
	int mode;
} my_reply;


void *keyboard(void *notused);
void *server();
void *handleServerMessages(void *rcvid_passed, void *msg_passed);
void *ex_timerCheckAlive(void * val);

enum intersection_modes {
	FIXED,
	SENSOR,
	SPECIAL
};

enum INTERSECTION_TYPE {
	Intersection1,
	Intersection2,
	BoomGate,
};

#endif /* SRC_COMMUNICATION_H_ */
