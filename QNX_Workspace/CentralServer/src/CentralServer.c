/*********************************************************************
 *						CENTRAL SERVER MAIN
 *
 * 				Includes:
 * 					1. Main thread
 *********************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>

#include "communication.h"

int main(int argc, char *argv[])
{
	//Declaring threads
	pthread_t  th1, keyboarInput, checkThread;

	//Getting the hostname and printing to console that we now start central server
	printf("Central Controller starts now...\n");
	char hostnm[100];
	memset(hostnm, '\0', 100);
	hostnm[99] = '\n';
	gethostname(hostnm, sizeof(hostnm));
	printf("--> Machine hostname is: '%s'\n", hostnm);
	printf("\n");


	// starting threads
	pthread_create(&keyboarInput, NULL, keyboard, NULL);
	pthread_create(&th1, NULL, server, NULL);
	pthread_create(&checkThread, NULL, timerCheckAlive, NULL);

	pthread_join(th1,NULL);
	printf("Main (Server) Terminated....\n");
	return 0;
}

