/*********************************************************************
 *						DISPLAY AND INTERFACE WITH DE10-NANO HARDWARE
 *
 * 				Includes:
 * 					1. Update the Intersection Outputs and Inputs
 *********************************************************************/

#include <pthread.h>
#include "display.h"
#include "SensorLightSequence.h"

void *updateIntersection(void *notused)
{
	printf("Cyclone LED and Button Initialize\n");

	// GPIO global pointers
	uintptr_t gpio_LEDs = NULL;
	uintptr_t gpio_button_inputs = NULL;
	uintptr_t Switches_inputs = NULL;

	volatile uint32_t	val = 0;

	gpio_LEDs = mmap_device_io(PIO_SIZE, LEDs_BASE);
	if( !gpio_LEDs )
	{
		// An error has occurred
		perror("Can't map Control Base Module / gpio_LEDs");
		printf("Mapping IO ERROR! Main Terminated...!\n");
		return EXIT_SUCCESS;
	}

	gpio_button_inputs = mmap_device_io(PIO_SIZE, BUTTONs_BASE);
	if( !gpio_button_inputs )
	{
		// An error has occurred
		perror("Can't map Control Base Module / gpio_button_inputs");
		printf("Mapping IO ERROR! Main Terminated...!\n");
		return EXIT_SUCCESS;
	}



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
		printf(stderr, "%s:  couldn't ConnectAttach to self!\n", "bka");
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
		printf (stderr, "%s:  couldn't create a timer, errno %d\n", "bka", errno);
		perror (NULL);
		pthread_exit(EXIT_FAILURE);
	}
	// setup the timer
	double x = 0.001;
	x += 0.5e-9;
	periodicTime.it_value.tv_sec = (long) x;
	periodicTime.it_value.tv_nsec = (x - periodicTime.it_value.tv_sec) * 1000000000L;
	periodicTime.it_interval.tv_sec = (long) x;
	periodicTime.it_interval.tv_nsec = (x - periodicTime.it_interval.tv_sec) * 1000000000L;

	printf("%ld %ld\n", periodicTime.it_value.tv_sec, periodicTime.it_value.tv_nsec);
	timer_settime(periodicTimer_id, 0, &periodicTime, NULL);


	while(1)
	{
		switch (CurrentState)
		{
		case EWR_NSR_EWTR_NSTR_0:
			val= 1<<EWR | 1<<NSR | 1<<EWTR | 1<<NSTR;
			val|= 1<<EWPedR | 1<<NSPedR;
			break;
		case EWR_NSR_EWTG_NSTR_1:
			val= 1<<EWR | 1<<NSR | 1<<EWTG | 1<<NSTR;
			val|= 1<<EWPedR | 1<<NSPedR;
			break;
		case EWR_NSR_EWTY_NSTR_2:
			val= 1<<EWR | 1<<NSR | 1<<EWTY | 1<<NSTR;
			val|= 1<<EWPedR | 1<<NSPedR;
			break;
		case EWR_NSR_EWTR_NSTR_3:
			val= 1<<EWR | 1<<NSR | 1<<EWTR | 1<<NSTR;
			val|= 1<<EWPedR | 1<<NSPedR;
			break;
		case EWG_NSR_EWTR_NSTR_4:
			val= 1<<EWG | 1<<NSR | 1<<EWTR | 1<<NSTR;
			val|= 1<<EWPedG | 1<<NSPedR;
			break;
		case EWY_NSR_EWTR_NSTR_5:
			val= 1<<EWY | 1<<NSR | 1<<EWTR | 1<<NSTR;
			val|= 1<<EWPedR | 1<<NSPedR;
			break;
		case EWR_NSR_EWTR_NSTR_6:
			val= 1<<EWR | 1<<NSR | 1<<EWTR | 1<<NSTR;
			val|= 1<<EWPedR | 1<<NSPedR;
			break;
		case EWR_NSR_EWTR_NSTG_7:
			val= 1<<EWR | 1<<NSR | 1<<EWTR | 1<<NSTG;
			val|= 1<<EWPedR | 1<<NSPedR;
			break;
		case EWR_NSR_EWTR_NSTY_8:
			val= 1<<EWR | 1<<NSR | 1<<EWTR | 1<<NSTY;
			val|= 1<<EWPedR | 1<<NSPedR;
			break;
		case EWR_NSR_EWTR_NSTR_9:
			val= 1<<EWR | 1<<NSR | 1<<EWTR | 1<<NSTR;
			val|= 1<<EWPedR | 1<<NSPedR;
			break;
		case EWR_NSG_EWTR_NSTR_10:
			val= 1<<EWR | 1<<NSG | 1<<EWTR | 1<<NSTR;
			val|= 1<<EWPedR | 1<<NSPedG;
			break;
		case EWR_NSY_EWTR_NSTR_11:
			val= 1<<EWR | 1<<NSY | 1<<EWTR | 1<<NSTR;
			val|= 1<<EWPedR | 1<<NSPedR;
			break;
		}

		if (CurSubstate != INTERSECTION_NORMAL|| TrainApproachint == 1)
		{
			if (CurSubstate == EWR_NSB_EWTR_NSTR_10_1)
			{
				val &= ~(1<<NSPedR | 1<<NSPedG);
			}
			if (CurSubstate == EWB_NSR_EWTR_NSTR_4_1)
			{
				val &= ~(1<<EWPedR | 1<<EWPedG);
			}
			if (TrainApproachint == 1)
			{
				//printf("Value Train %d\n", TrainApproachint);
				val |= 1<<TrainR1;
			}
			out32(gpio_LEDs, val);
			usleep(80000);
			val &= ~(1<<TrainR1);
			if (CurSubstate == EWR_NSB_EWTR_NSTR_10_1)
			{
				val |= 1<<NSPedR;
			}
			if (CurSubstate == EWB_NSR_EWTR_NSTR_4_1)
			{
				val |=  1<<EWPedR;
			}
			if(TrainApproachint == 1)
			{
				val |= 1<<TrainR2;
			}
			out32(gpio_LEDs, val);
		}
		usleep(80000);
		if (TrainApproachint == 0) {
			val &= ~(1<<TrainR1 | 1<<TrainR2);
		}
		out32(gpio_LEDs, val); // write new value

		val  = in32(gpio_button_inputs);
		// read the button input
		if(((val>>EWTButton)&1) == 0)
		{
			currentSensor = EWT_sensor;
		}
		else if(((val>>EWButton)&1) == 0)
		{
			currentSensor = EW_sensor;
		}
		else if(((val>>EWPedButton)&1) == 0)
		{
			currentSensor = EW_sensor;
		}
		else if(((val>>NSTButton)&1) == 0)
		{
			currentSensor = NST_sensor;
		}
		else if(((val>>NSButton)&1) == 0)
		{
			currentSensor = NS_sensor;
		}
		else if(((val>>NSPedButton)&1) == 0)
		{
			currentSensor = NS_sensor;
		}
		//MsgReceive(chid, NULL, NULL, NULL);
	}

	munmap_device_io(gpio_LEDs, PIO_SIZE);
	munmap_device_io(gpio_button_inputs, PIO_SIZE);

	pthread_exit(EXIT_SUCCESS);
}
