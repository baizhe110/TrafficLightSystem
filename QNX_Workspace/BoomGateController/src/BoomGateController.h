/*
 * BoomGateSequence.h
 *
 */

#ifndef SRC_BOOMGATESEQUENCE_H_
#define SRC_BOOMGATESEQUENCE_H_

enum states BoomGateSequence(void *CurrentState);

void *keyboard(void *notused);

#define BUF_SIZE 100
//#define QNET_ATTACH_POINT  "/net/GerardHost/dev/name/local/CentralServer"  // hostname using full path, change myname to the name used for server
//#define QNET_ATTACH_POINT "/net/VM_x86_Target02/dev/name/local/CentralServer"
#define QNET_ATTACH_POINT "/net/cycloneV_user/dev/name/local/CentralServer"
#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL
#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL

// enums ********************************************
enum states {
	NoTrain_0,
	TrainApproaching_1,
	TrainApproaching_2,
	TrainCrossing_3,
	TrainLeaving_4,
	TrainLeaving_5,
};

enum INTERSECTION_TYPE {
	Intersection1,
	Intersection2,
	BoomGate,
};

//enum substates {
//	INTERSECTION_NORMAL,
//	EWB_NSR_EWTR_NSTR_4_1,
//	EWR_NSB_EWTR_NSTR_10_1
//};
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
enum BoomGate_mode {
	NORMAL,
	SPECIAL
};

// global variables ****************************************
enum states CurState, CurSubstate;

#endif /* SRC_SENSORLIGHTSEQUENCE_H_ */
