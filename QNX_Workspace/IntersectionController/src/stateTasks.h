/*
 * stateTasks.h
 *
 *  Created on: 1 Oct 2019
 *      Author: bruno
 */

#ifndef SRC_STATETASKS_H_
#define SRC_STATETASKS_H_

#include <sys/iofunc.h>

#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL

struct Timervalues{
	double NSG_car;
	double NSB_ped;
	double NSTG_car;
	double NSY_car;
	double NSTY_car;
	double NSR_clear;
	double NSTR_clear;

	double EWG_car;
	double EWB_ped;
	double EWTG_car;
	double EWY_car;
	double EWTY_car;
	double EWR_clear;
	double EWTR_clear;
};

struct Timervalues times;

timer_t                 timer_id;
int                     chid;


// timer variables
struct itimerspec itime1;

void initTimer();
void startOneTimeTimer(timer_t timerID, double time);
void setTimerValues();

void StateTime0();
void StateTime1();
void StateTime2();
void StateTime3();
void StateTime4();
void StateTime4_1();
void StateTime5();
void StateTime6();
void StateTime7();
void StateTime8();
void StateTime9();
void StateTime10();
void StateTime10_1();
void StateTime11();

//BBB LCD
int file;
#define DATA_SEND 0x40  // sets the Rs value high
#define Co_Ctrl   0x00  // mode to tell LCD we are sending a single command

int  I2cWrite_(int fd, uint8_t Address, uint8_t mode, uint8_t *pBuffer, uint32_t NbData);
void SetCursor(int fd, uint8_t LCDi2cAdd, uint8_t row, uint8_t column);
void Initialise_LCD (int fd, _Uint32t LCDi2cAdd);
void print_Data_LCD(int line, char *str);

#endif /* SRC_STATETASKS_H_ */
