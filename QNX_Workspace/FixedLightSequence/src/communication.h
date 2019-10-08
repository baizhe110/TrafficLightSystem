/*
 * communication.h
 *
 *  Created on: 3 Oct 2019
 *      Author: bruno
 */

#ifndef SRC_COMMUNICATION_H_
#define SRC_COMMUNICATION_H_
#include <sys/dispatch.h>

#define BUF_SIZE 100
#define attachPoint "/net/GerardHost/dev/name/local/CentralServer"  // hostname using full path, change myname to the name used for server
#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL

void *ex_client(void *sname_data);

typedef struct
{
	struct _pulse hdr; // Our real data comes after this header
	int ClientID; // our data (unique id from client)
	int data;     // our data
	int type;
} my_data;

typedef struct
{
	struct _pulse hdr;  // Our real data comes after this header
	char buf[BUF_SIZE]; // Message we send back to clients to tell them the messages was processed correctly.
	int data;
} my_reply;

#endif /* SRC_COMMUNICATION_H_ */
