#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>

#include "communication.h"

int main(int argc, char *argv[])
{
	printf("Central Controller starts now...\n");
//	printf ("--> The pid of this process is %d\n", getpid() );

	char hostnm[100];
	memset(hostnm, '\0', 100);
	hostnm[99] = '\n';
	gethostname(hostnm, sizeof(hostnm));

	printf("--> Machine hostname is: '%s'\n", hostnm);
	printf("\n");
	printf("Server running\n");

	pthread_t  th1, keyboarInput, checkThread;

	// starting threads
	pthread_create(&keyboarInput, NULL, keyboard, NULL);
	pthread_create(&th1, NULL, server, NULL);
	pthread_create(&checkThread, NULL, ex_timerCheckAlive, NULL);

	pthread_join(th1,NULL);
	printf("Main (Server) Terminated....\n");
	return 0;
}

