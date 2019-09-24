#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <mqueue.h>
#include <errno.h>
#include <sys/iofunc.h>
#include <sys/netmgr.h>

#define Q_FLAGS O_RDWR | O_CREAT | O_EXCL
#define Q_Mode S_IRUSR | S_IWUSR
#define BUF_SIZE 100
#define  MESSAGESIZE 2

#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL

char *progname = "timer_per1.c";

//for massage passing
int pid;
int chid;
char key[10];

typedef union
{
	struct _pulse   pulse;
    // your other message structures would go here too
} my_message_t;

typedef struct
{
	struct _pulse hdr; // Our real data comes after this header
	int ClientID; // our data (unique id from client)
    char data;     // our data
} my_data;

typedef struct
{
	struct _pulse hdr;  // Our real data comes after this header
    char buf[BUF_SIZE]; // Message we send back to clients to tell them the messages was processed correctly.
} my_reply;
struct IDs_data {
    int p_id;
    int ch_id;
};

size_t read_data( FILE *fp, struct IDs_data *p )
{
    return( fread( p, sizeof( struct IDs_data ), 1, fp ) );
}

struct sigevent         event;
struct itimerspec       itime1, itime2;
timer_t                 timer_id;
int                     chid;
int                     rcvid;
my_message_t            msg;

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
void *ex_client(void * val);

enum states { EWR_NSR_EWTR_NSTR_0,
	EWR_NSR_EWTG_NSTR_1,
	EWR_NSR_EWTY_NSTR_2,
	EWR_NSR_EWTR_NSTR_3,
	EWG_NSR_EWTR_NSTR_4,
	EWY_NSR_EWTR_NSTR_5,
	EWR_NSR_EWTR_NSTR_6,
	EWR_NSR_EWTR_NSTG_7,
	EWR_NSR_EWTR_NSTY_8,
	EWR_NSR_EWTR_NSTR_9,
	EWR_NSG_EWTR_NSTR_10,
	EWR_NSY_EWTR_NSTR_11};
enum states CurState;
//--- --- --- ---

void Initialise()
{
	printf("Start of fixed light sequence\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething0()
{
	printf("In state0: EWR_NSR_EWTR_NSTR_0\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething1()
{
	printf("In state1: EWR_NSR_EWTG_NSTR_1\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething2()
{
	printf("In state2: EWR_NSR_EWTY_NSTR_2\n");
	timer_settime(timer_id, 0, &itime2, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);

}
void DoSomething3()
{
	printf("In state3: EWR_NSR_EWTR_NSTR_3\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething4()
{
	printf("In state4: EWG_NSR_EWTR_NSTR_4\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething5()
{
	printf("In state5: EWY_NSR_EWTR_NSTR_5\n");
	timer_settime(timer_id, 0, &itime2, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething6()
{
	printf("In state6: EWR_NSR_EWTR_NSTR_6\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething7()
{
	printf("In state7: EWR_NSR_EWTR_NSTG_7\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething8()
{
	printf("In state8: EWR_NSR_EWTR_NSTY_8\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething9()
{
	printf("In state9: EWR_NSR_EWTR_NSTR_9\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething10()
{
	printf("In state10: EWR_NSG_EWTR_NSTR_10\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}
void DoSomething11()
{
	printf("In state11: EWR_NSY_EWTR_NSTR_11\n");
	timer_settime(timer_id, 0, &itime1, NULL);
	MsgReceive(chid, &msg, sizeof(msg), NULL);
}

enum states SingleStep_TrafficLight_SM(void *CurrentState)
{

	enum states CurState = *(enum states*)CurrentState;

	//CurState = CurrentState;


	switch (CurState){

	case EWR_NSR_EWTR_NSTR_0:
		DoSomething0();
		CurState = EWR_NSR_EWTG_NSTR_1;
		break;
	case EWR_NSR_EWTG_NSTR_1:
		DoSomething1();
		CurState = EWR_NSR_EWTY_NSTR_2;
		break;
	case EWR_NSR_EWTY_NSTR_2:
		DoSomething2();
		CurState = EWR_NSR_EWTR_NSTR_3;
		break;
	case EWR_NSR_EWTR_NSTR_3:
		DoSomething3();
		CurState = EWG_NSR_EWTR_NSTR_4;
		break;
	case EWG_NSR_EWTR_NSTR_4:
		DoSomething4();
		CurState = EWY_NSR_EWTR_NSTR_5;
		break;
	case EWY_NSR_EWTR_NSTR_5:
		DoSomething5();
		CurState = EWR_NSR_EWTR_NSTR_6;
		break;
	case EWR_NSR_EWTR_NSTR_6:
		DoSomething6();
		CurState = EWR_NSR_EWTR_NSTG_7;
		break;
	case EWR_NSR_EWTR_NSTG_7:
		DoSomething7();
		CurState = EWR_NSR_EWTR_NSTY_8;
		break;
	case EWR_NSR_EWTR_NSTY_8:
		DoSomething8();
		CurState = EWR_NSR_EWTR_NSTR_9;
		break;
	case EWR_NSR_EWTR_NSTR_9:
		DoSomething9();
		CurState = EWR_NSG_EWTR_NSTR_10;
		break;
	case EWR_NSG_EWTR_NSTR_10:
		DoSomething10();
		CurState = EWR_NSY_EWTR_NSTR_11;
		break;
	case EWR_NSY_EWTR_NSTR_11:
		DoSomething11();
		CurState = EWR_NSR_EWTR_NSTR_0;
		break;
	}
	return CurState;
}


int main(int argc, char *argv[])
{

	printf("Fixed Sequence Traffic Lights State Machine\n");

    //create client thread
	char val = 'e';
	pthread_t  th3_client;
    pthread_create (&th3_client, NULL, ex_client, &val);

	int Runtimes=30, counter = 0;
	enum states CurrentState=0; // Declaring the enum within the main
	// means we will need to pass it by address

		chid = ChannelCreate(0); // Create a communications channel

		event.sigev_notify = SIGEV_PULSE;

		// create a connection back to ourselves for the timer to send the pulse on
		event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL, 0);
		if (event.sigev_coid == -1)
		{
		   printf(stderr, "%s:  couldn't ConnectAttach to self!\n", progname);
		   perror(NULL);
		   exit(EXIT_FAILURE);
		}
		//event.sigev_priority = getprio(0);  // this function is depreciated in QNX 700
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

		// setup the timer (1.5s initial delay value, 1.5s reload interval)
		itime1.it_value.tv_sec = 1;			  // 1 second
		itime1.it_value.tv_nsec = 0;    // 500 million nsecs = .5 secs
		itime1.it_interval.tv_sec = 0;          // 1 second
		itime1.it_interval.tv_nsec = 0; // 500 million nsecs = .5 secs

		// setup the timer (1.5s initial delay value, 1.5s reload interval)
		itime2.it_value.tv_sec = 2;			  // 1 second
		itime2.it_value.tv_nsec = 0;    // 500 million nsecs = .5 secs
		itime2.it_interval.tv_sec = 0;          // 1 second
		itime2.it_interval.tv_nsec = 0; // 500 million nsecs = .5 secs

		Initialise();

	while (counter < Runtimes)
	{
		CurrentState = SingleStep_TrafficLight_SM( &CurrentState ); // pass address
		counter++;
	}
}

void *ex_client(void * val)

{
	//read file code starts

	    FILE *fp;
	    struct IDs_data ids;
	    int i;

	    fp = fopen( "ids_file", "r" );
	    if( fp != NULL ) {
	        while( read_data( fp, &ids ) != 0 ) {
	           // printf( "pid=%d\n", ids.p_id );
				//printf( "chid=%d\n", ids.ch_id);
	        	chid = ids.ch_id;
	        	pid =  ids.p_id;
	        }

	        fclose( fp );

	    }
	//read file code ends
    my_data msg;
    my_reply reply;

    msg.ClientID = 500;

    int server_coid;
    int index = 0;

	printf("   --> Trying to connect (server) process which has a PID: %d\n",   pid);
	printf("   --> on channel: %d\n\n", chid);
	char buff[10] = {};
	char buf[MESSAGESIZE] = {};
	// set up message passing channel
    server_coid = ConnectAttach(ND_LOCAL_NODE, pid, chid, _NTO_SIDE_CHANNEL, 0);
	if (server_coid == -1)
	{
        printf("\n    ERROR, could not connect to server!\n\n");
        pthread_exit(EXIT_FAILURE);
	}


    printf("Connection established to process with PID:%d, Ch:%d\n", pid, chid);

    // We would have pre-defined data to stuff here
    msg.hdr.type = 0x00;
    msg.hdr.subtype = 0x00;

    // Do whatever work you wanted with server connection
    while (1) // send data packets
    {
    	gets(buff);

    		pthread_mutex_lock(&mutex);

    		key[0] = buff[0];

    		pthread_mutex_unlock(&mutex);
    		for (i=1; i <= 2; ++i)
    				{
    					sprintf(buf, "%c", key[0]);			//put the message in a char[] so it can be sent
    					//printf("queue: '%s'\n", buf); 			//print the message to this processes terminal
    					msg.data=buf;
    				}
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

        //sleep(5);	// wait a few seconds before sending the next data packet
    }

    // Close the connection
    printf("\n Sending message to server to tell it to close the connection\n");
    ConnectDetach(server_coid);

    return EXIT_SUCCESS;
}
