/*
 * stateTasks.c
 *
 *  Created on: 1 Oct 2019
 *      Author: bruno
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/iofunc.h>
#include <sys/netmgr.h>

#include "stateTasks.h"

char *progname = "timer_per1.c";

void DoSomething0()
{
	printf("In state0: EWR_NSR_EWTR_NSTR_0\n");
	startOneTimeTimer(timer_id, times.NSR_clear);
	//timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething1()
{
	printf("In state1: EWR_NSR_EWTG_NSTR_1\n");
	startOneTimeTimer(timer_id, times.EWTG_car);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething2()
{
	printf("In state2: EWR_NSR_EWTY_NSTR_2\n");
	startOneTimeTimer(timer_id, times.EWTY_car);
	MsgReceive(chid, &msg, sizeof(msg), NULL);

}
void DoSomething3()
{
	printf("In state3: EWR_NSR_EWTR_NSTR_3\n");
	startOneTimeTimer(timer_id, times.EWTR_clear);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething4()
{
	printf("In state4: EWG_NSR_EWTR_NSTR_4\n");
	startOneTimeTimer(timer_id, times.EWG_car-times.EWB_ped);
	MsgReceive(chid, &msg, sizeof(msg), NULL);

}
void DoSomething4_1()
{
	printf("In state4_1: EWB_NSR_EWTR_NSTR_4_1\n");
	startOneTimeTimer(timer_id, times.EWB_ped);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething5()
{
	printf("In state5: EWY_NSR_EWTR_NSTR_5\n");
	startOneTimeTimer(timer_id, times.EWY_car);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething6()
{
	printf("In state6: EWR_NSR_EWTR_NSTR_6\n");
	startOneTimeTimer(timer_id, times.EWR_clear);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething7()
{
	printf("In state7: EWR_NSR_EWTR_NSTG_7\n");
	startOneTimeTimer(timer_id, times.NSTG_car);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething8()
{
	printf("In state8: EWR_NSR_EWTR_NSTY_8\n");
	startOneTimeTimer(timer_id, times.NSTY_car);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething9()
{
	printf("In state9: EWR_NSR_EWTR_NSTR_9\n");
	startOneTimeTimer(timer_id, times.NSTR_clear);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething10()
{
	printf("In state10: EWR_NSG_EWTR_NSTR_10\n");
	startOneTimeTimer(timer_id, times.NSG_car-times.NSB_ped);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething10_1()
{
	printf("In state10_1: EWR_NSB_EWTR_NSTR_10_1\n");
	startOneTimeTimer(timer_id, times.NSB_ped);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething11()
{
	printf("In state11: EWR_NSY_EWTR_NSTR_11\n");
	startOneTimeTimer(timer_id, times.NSY_car);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}


////////////////// - Timers -//////////////////////////////////////
void startOneTimeTimer(timer_t timerID, double time)
{
	double x = time;
	x += 0.5e-9;
	itime1.it_value.tv_sec = (long) x;
	itime1.it_value.tv_nsec = (x - itime1.it_value.tv_sec) * 1000000000L;
	//printf("%ld %ld\n", itime1.it_value.tv_sec, itime1.it_value.tv_nsec);
	timer_settime(timerID, 0, &itime1, NULL);
}

void initTimer()
{
	chid = ChannelCreate(0); // Create a communications channel

	struct sigevent         event;
	event.sigev_notify = SIGEV_PULSE;

	// create a connection back to ourselves for the timer to send the pulse on
	event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL, 0);
	if (event.sigev_coid == -1)
	{
		printf(stderr, "%s:  couldn't ConnectAttach to self!\n", progname);
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	struct sched_param th_param;
	pthread_getschedparam(pthread_self(), NULL, &th_param);
	event.sigev_priority = th_param.sched_curpriority;    // old QNX660 version getprio(0);
	event.sigev_code = MY_PULSE_CODE;

	// create the timer, binding it to the event
	if (timer_create(CLOCK_REALTIME, &event, &timer_id) == -1)
	{
		printf (stderr, "%s:  couldn't create a timer, errno %d\n", progname, errno);
		perror (NULL);
		exit (EXIT_FAILURE);
	}
}
