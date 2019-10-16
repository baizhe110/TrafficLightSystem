/*********************************************************************
 *			STATE MACHINE FUNCTIONS AND FUNCTIONS FOR TIMERS
 *
 * 				Includes:
 * 					1. StateTime state machine functions
 * 					2. Timer function: startOneTimeTimer. (Counter for the StateTime functions)
 * 					3.
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/iofunc.h>
#include <sys/netmgr.h>
// BBB LCD
#include "stdint.h"
#include <fcntl.h>
#include <devctl.h>
#include <hw/i2c.h>
#include <errno.h>
#include <unistd.h>
#include <sys/neutrino.h>

#include "defines.h"
#include "stateTasks.h"




char *progname = "timer_per1.c"; 		//Pointer for timer



/*********************************************************************
 *	StateTime functions for the state machine (Used both for fixed sequence and sensor driven)
 *********************************************************************/
void StateTime0()
{
	print_Data_LCD(0,"EWR_NSR_EWTR_NSTR_0");
	printf("In state0: EWR_NSR_EWTR_NSTR_0\n");
	startOneTimeTimer(timer_id, times.NSR_clear);
	//timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, NULL, NULL, NULL);
}
void StateTime1()
{
	print_Data_LCD(0,"EWR_NSR_EWTG_NSTR_1");
	printf("In state1: EWR_NSR_EWTG_NSTR_1\n");
	startOneTimeTimer(timer_id, times.EWTG_car);
	MsgReceive(chid, NULL, NULL, NULL);
}
void StateTime2()
{
	print_Data_LCD(0,"EWR_NSR_EWTY_NSTR_2");
	printf("In state2: EWR_NSR_EWTY_NSTR_2\n");
	startOneTimeTimer(timer_id, times.EWTY_car);
	MsgReceive(chid, NULL, NULL, NULL);

}
void StateTime3()
{
	print_Data_LCD(0,"EWR_NSR_EWTR_NSTR_3");
	printf("In state3: EWR_NSR_EWTR_NSTR_3\n");
	startOneTimeTimer(timer_id, times.EWTR_clear);
	MsgReceive(chid, NULL, NULL, NULL);
}
void StateTime4()
{
	print_Data_LCD(0,"EWG_NSR_EWTR_NSTR_4");
	printf("In state4: EWG_NSR_EWTR_NSTR_4\n");
	startOneTimeTimer(timer_id, times.EWG_car-times.EWB_ped);
	MsgReceive(chid, NULL, NULL, NULL);

}
void StateTime4_1()
{
	if(CurrentMode == FIXED)
	{
		printf("Fixed SM>\tIn state4_1: EWB_NSR_EWTR_NSTR_4_1 (Ped. light blink)\n");
	}
	else if(CurrentMode == SENSOR)
	{
		printf("Sensor SM>\tIn state4_1: EWB_NSR_EWTR_NSTR_4_1 (Ped. light blink)\n");
	}
	startOneTimeTimer(timer_id, times.EWB_ped);
	MsgReceive(chid, NULL, NULL, NULL);
}
void StateTime5()
{
	print_Data_LCD(0,"EWY_NSR_EWTR_NSTR_5");
	printf("In state5: EWY_NSR_EWTR_NSTR_5\n");
	startOneTimeTimer(timer_id, times.EWY_car);
	MsgReceive(chid, NULL, NULL, NULL);
}
void StateTime6()
{
	print_Data_LCD(0,"EWR_NSR_EWTR_NSTR_6");
	printf("In state6: EWR_NSR_EWTR_NSTR_6\n");
	startOneTimeTimer(timer_id, times.EWR_clear);
	MsgReceive(chid, NULL, NULL, NULL);
}
void StateTime7()
{
	print_Data_LCD(0,"EWR_NSR_EWTR_NSTG_7");
	printf("In state7: EWR_NSR_EWTR_NSTG_7\n");
	startOneTimeTimer(timer_id, times.NSTG_car);
	MsgReceive(chid, NULL, NULL, NULL);
}
void StateTime8()
{
	print_Data_LCD(0,"EWR_NSR_EWTR_NSTY_8");
	printf("In state8: EWR_NSR_EWTR_NSTY_8\n");
	startOneTimeTimer(timer_id, times.NSTY_car);
	MsgReceive(chid, NULL, NULL, NULL);
}
void StateTime9()
{
	print_Data_LCD(0,"EWR_NSR_EWTR_NSTR_9");
	printf("In state9: EWR_NSR_EWTR_NSTR_9\n");
	startOneTimeTimer(timer_id, times.NSTR_clear);
	MsgReceive(chid, NULL, NULL, NULL);
}
void StateTime10()
{
	print_Data_LCD(0,"EWR_NSG_EWTR_NSTR_10");
	printf("In state10: EWR_NSG_EWTR_NSTR_10\n");
	startOneTimeTimer(timer_id, times.NSG_car-times.NSB_ped);
	MsgReceive(chid, NULL, NULL, NULL);
}
void StateTime10_1()
{
	if(CurrentMode == FIXED)
	{
		printf("Fixed SM>\tIn state10_1: EWR_NSB_EWTR_NSTR_10_1 (Ped. light blink)\n");
	}
	else if(CurrentMode == SENSOR)
	{
		printf("Sensor SM>\tIn state10_1: EWR_NSB_EWTR_NSTR_10_1 (Ped. light blink)\n");
	}
	startOneTimeTimer(timer_id, times.NSB_ped);
	MsgReceive(chid, NULL, NULL, NULL);
}
void StateTime11()
{
	print_Data_LCD(0,"EWR_NSY_EWTR_NSTR_11");
	printf("In state11: EWR_NSY_EWTR_NSTR_11\n");
	startOneTimeTimer(timer_id, times.NSY_car);
	MsgReceive(chid, NULL, NULL, NULL);
}








/*********************************************************************
 *	Timer that counts for the specified input time (Used for the StateTime functions)
 *********************************************************************/
void startOneTimeTimer(timer_t timerID, double time)
{
	double x = time;
	x += 0.5e-9;
	itime1.it_value.tv_sec = (long) x;
	itime1.it_value.tv_nsec = (x - itime1.it_value.tv_sec) * 1000000000L;
	timer_settime(timerID, 0, &itime1, NULL);
}







/*********************************************************************
 *	Initialization of timer
 *********************************************************************/
void initTimer()
{
	Initialise_LCD(1,1);
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



/*********************************************************************
 *				TIMER VALUES SETTING FUNCTION
 *********************************************************************/
void setTimerValues(struct Timervalues t)
{
//	printf("%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",times.NSG_car,times.NSB_ped,times.NSTG_car, times.NSY_car,times.NSTY_car,
//			times.NSR_clear,times.NSTR_clear,times.EWG_car,times.EWB_ped,times.EWTG_car,times.EWY_car,times.EWTY_car,times.EWR_clear,times.EWTR_clear);
	times.NSG_car 	= t.NSG_car;
	times.NSB_ped 	= t.NSB_ped;
	times.NSTG_car 	= t.NSTG_car;
	times.NSY_car 	= t.NSY_car;
	times.NSTY_car 	= t.NSTY_car;
	times.NSR_clear	= t.NSR_clear;
	times.NSTR_clear= t.NSTR_clear;

	times.EWG_car	= t.EWG_car;
	times.EWB_ped	= t.EWB_ped;
	times.EWTG_car	= t.EWTG_car;
	times.EWY_car	= t.EWY_car;
	times.EWTY_car	= t.EWTY_car;
	times.EWR_clear = t.EWR_clear;
	times.EWTR_clear= t.EWTR_clear;
//	printf("%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",times.NSG_car,times.NSB_ped,times.NSTG_car, times.NSY_car,times.NSTY_car,
//			times.NSR_clear,times.NSTR_clear,times.EWG_car,times.EWB_ped,times.EWTG_car,times.EWY_car,times.EWTY_car,times.EWR_clear,times.EWTR_clear);
}

// Writes to I2C
int  I2cWrite_(int fd, uint8_t Address, uint8_t mode, uint8_t *pBuffer, uint32_t NbData)
{
	i2c_send_t hdr;
	iov_t sv[2];
	int status, i;

	uint8_t LCDpacket[21] = {};  // limited to 21 characters  (1 control bit + 20 bytes)

	// set the mode for the write (control or data)
	LCDpacket[0] = mode;  // set the mode (data or control)

	// copy data to send to send buffer (after the mode bit)
	for (i=0;i<NbData+1;i++)
		LCDpacket[i+1] = *pBuffer++;

	hdr.slave.addr = Address;
	hdr.slave.fmt = I2C_ADDRFMT_7BIT;
	hdr.len = NbData + 1;  // 1 extra for control (mode) bit
	hdr.stop = 1;

	SETIOV(&sv[0], &hdr, sizeof(hdr));
	SETIOV(&sv[1], &LCDpacket[0], NbData + 1); // 1 extra for control (mode) bit
	// int devctlv(int filedes, int dcmd,     int sparts, int rparts, const iov_t *sv, const iov_t *rv, int *dev_info_ptr);
	status = devctlv(fd, 		  DCMD_I2C_SEND, 2,          0,          sv,              NULL,           NULL);

	if (status != EOK)
		//printf("status = %s\n", strerror ( status ));

	return status;
}

void SetCursor(int fd, uint8_t LCDi2cAdd, uint8_t row, uint8_t column)
{
	uint8_t position = 0x80; // SET_DDRAM_CMD (control bit)
	uint8_t rowValue = 0;
	uint8_t	LCDcontrol = 0;
	if (row == 1)
		rowValue = 0x40;     // memory location offset for row 1
	position = (uint8_t)(position + rowValue + column);
	LCDcontrol = position;
	I2cWrite_(fd, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C
}


void Initialise_LCD (int fd, _Uint32t LCDi2cAdd2)
{

	int error;
	volatile uint8_t LCDi2cAdd = 0x3C;
	_Uint32t speed = 10000; // nice and slow (will work with 200000)

	uint8_t	LCDdata[21] = {};

	// Open I2C resource and set it up
	if ((file = open("/dev/i2c1",O_RDWR)) < 0)	  // OPEN I2C1
		printf("Error while opening Device File.!!\n");
	else
		printf("I2C1 Opened Successfully\n");

	error = devctl(file,DCMD_I2C_SET_BUS_SPEED,&(speed),sizeof(speed),NULL);  // Set Bus speed
	if (error)
	{
		fprintf(stderr, "Error setting the bus speed: %d\n",strerror ( error ));
		//exit(EXIT_FAILURE);
	}
	else
		printf("Bus speed set = %d\n", speed);

	//Initialise_LCD(file, LCDi2cAdd);

	uint8_t	LCDcontrol = 0x00;

	//   Initialise the LCD display via the I2C bus
	LCDcontrol = 0x38;  // data byte for FUNC_SET_TBL1
	I2cWrite_(file, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x39; // data byte for FUNC_SET_TBL2
	I2cWrite_(file, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x14; // data byte for Internal OSC frequency
	I2cWrite_(file, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x79; // data byte for contrast setting
	I2cWrite_(file, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x50; // data byte for Power/ICON control Contrast set
	I2cWrite_(file, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x6C; // data byte for Follower control
	I2cWrite_(file, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x0C; // data byte for Display ON
	I2cWrite_(file, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C

	LCDcontrol = 0x01; // data byte for Clear display
	I2cWrite_(file, LCDi2cAdd, Co_Ctrl, &LCDcontrol, 1);		// write data to I2C
}

void print_Data_LCD(int line, char *str)
{
	volatile uint8_t LCDi2cAdd = 0x3C;
	uint8_t	LCDdata[21] = {};
	// write some Text to the LCD screen
	SetCursor(file, LCDi2cAdd,line,0); // set cursor on LCD to first position first line
	strcpy(LCDdata,str);
	I2cWrite_(file, LCDi2cAdd, DATA_SEND, &LCDdata[0], sizeof(LCDdata));		// write new data to I2C

}

