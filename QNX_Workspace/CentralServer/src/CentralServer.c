#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <mqueue.h>
#include <errno.h>
#include <sys/iofunc.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>
#include <errno.h>
#include <time.h>

#define Q_FLAGS O_RDWR | O_CREAT | O_EXCL
#define Q_Mode S_IRUSR | S_IWUSR
#define BUF_SIZE 100

typedef struct
{
	struct _pulse hdr; // Our real data comes after this header
	int ClientID; // our data (unique id from client)
    char data ;     // our data
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

struct itimerspec       itime_a;
timer_t                 timer_id;

int pid;
int chid;
int client0Alive = 0;
int checkClient0Counter = 0;

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;



void *ex_timer(void * val);
void *ex_server(void * val);

char client0mode[10];

int buff[10]={};
int rcvid=0;
char msgnum;

int main(int argc, char *argv[])
{
	char val = 'e';
	client0mode[0] = 'e';
	my_data msg;
	printf("Central Controller message passing demo starts now...\n");
	pthread_t  th2_clientAliveChecking;
	pthread_t  th1_server;

    pthread_create (&th1_server, NULL, ex_server, &val);
    pthread_create (&th2_clientAliveChecking, NULL, ex_timer, &val);

	while(1)
	{
	sleep(1);
	scanf("%d",&buff);

	pthread_mutex_lock(&mutex);

	msgnum = buff[0];

	pthread_mutex_unlock(&mutex);
	printf("The mode command has been set to: %d\n", buff[0]);
	}

	return EXIT_SUCCESS;
}

void *ex_server(void * val)
{

	//file write code starts
	    FILE *fp;
	    struct IDs_data ids;
	    int i;

	    pid= getpid();
	    chid = ChannelCreate(_NTO_CHF_DISCONNECT);
	    fp = fopen( "ids_file", "w" );
	    if( fp != NULL ) {

	        ids.p_id = pid;
	        ids.ch_id = chid;
	        printf("PID is %d\n", ids.p_id);
	        printf("CHID is %d\n", ids.ch_id);

	        fwrite( &ids, sizeof( struct IDs_data ), 1, fp );
	        printf( "Server info file has created.\n", i );


	        fclose( fp );
		}

	//file write code ends


		if (chid == -1)  // _NTO_CHF_DISCONNECT flag used to allow detach
		{
		    printf("\nFailed to create communication channel on server\n");
			pthread_exit(EXIT_FAILURE);
		}



		my_data msg;

		int Stay_alive=0, living=0;	// server stays running (ignores _PULSE_CODE_DISCONNECT request)

		my_reply replymsg; 			// replymsg structure for sending back to client
		replymsg.hdr.type = 0x01;
		replymsg.hdr.subtype = 0x00;

		living =1;
		while (living)

		{

		   // Do your MsgReceive's here now with the chid
		   rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);

		   if (rcvid == -1)  // Error condition, exit
		   {
			   printf("\nFailed to MsgReceive\n");
			   break;
		   }

		   // did we receive a Pulse or message?
		   // for Pulses:
		   if (rcvid == 0)  //  Pulse received, work out what type
		   {
			   switch (msg.hdr.code)
			   {
				   case _PULSE_CODE_DISCONNECT:
						// A client disconnected all its connections by running
						// name_close() for each name_open()  or terminated
					   if( Stay_alive == 0)
					   {
						   ConnectDetach(msg.hdr.scoid);
						   printf("\nServer was told to Detach from ClientID:%d ...\n", msg.ClientID);
						   living = 0; // kill while loop
						   continue;
					   }
					   else
					   {
						   printf("\nServer received Detach pulse from ClientID:%d but rejected it ...\n", msg.ClientID);
					   }
					   break;

				   case _PULSE_CODE_UNBLOCK:
						// REPLY blocked client wants to unblock (was hit by a signal
						// or timed out).  It's up to you if you reply now or later.
					   printf("\nServer got _PULSE_CODE_UNBLOCK after %d, msgnum\n", msgnum);
					   break;

				   case _PULSE_CODE_COIDDEATH:  // from the kernel
					   printf("\nServer got _PULSE_CODE_COIDDEATH after %d, msgnum\n", msgnum);
					   break;

				   case _PULSE_CODE_THREADDEATH: // from the kernel
					   printf("\nServer got _PULSE_CODE_THREADDEATH after %d, msgnum\n", msgnum);
					   break;

				   default:
					   // Some other pulse sent by one of your processes or the kernel
					   printf("\nServer got some other pulse after %d, msgnum\n", msgnum);
					   break;

			   }
			   continue;// go back to top of while loop
		   }

		   // for messages:
		   if(rcvid > 0) // if true then A message was received
		   {
			   if(checkClient0Counter < 50)
			   {
				   pthread_mutex_lock(&mutex);
				   checkClient0Counter++;
				   pthread_mutex_unlock(&mutex);
			   }
			   else
			   {
				   pthread_mutex_lock(&mutex);
				   checkClient0Counter = 0;
				   pthread_mutex_unlock(&mutex);
			   }


			   // If the Global Name Service (gns) is running, name_open() sends a connect message. The server must EOK it.
			   if (msg.hdr.type == _IO_CONNECT )
			   {
				   MsgReply( rcvid, EOK, NULL, 0 );
				   printf("\n gns service is running....");
				   continue;	// go back to top of while loop
			   }

			   // Some other I/O message was received; reject it
			   if (msg.hdr.type > _IO_BASE && msg.hdr.type <= _IO_MAX )
			   {
				   MsgError( rcvid, ENOSYS );
				   printf("\n Server received and IO message and rejected it....");
				   continue;	// go back to top of while loop
			   }

			   // A message (presumably ours) received


			   // put your message handling code here and assemble a reply message
			   sprintf(replymsg.buf, "%d", msgnum);
			   printf("Server received the current state with value of '%c' from client (ID:%d), ", msg.data, msg.ClientID);
				   fflush(stdout);
				   client0mode[0] = msg.data;
				   sleep(1); // Delay the reply by a second (just for demonstration purposes)

			   printf("\n    -----> replying with: '%s'\n",replymsg.buf);
			   MsgReply(rcvid, EOK, &replymsg, sizeof(replymsg));
		   }
		   else
		   {
			   printf("\nERROR: Server received something, but could not handle it correctly\n");
		   }

		}
		printf("\nServer received Destroy command\n");
		// destroyed channel before exiting
		ChannelDestroy(chid);
		printf("Cummunication channel destoried");


		return EXIT_SUCCESS;
	}


/*void *ex_timer(void * val)
{

	#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL

	typedef union
	{
		struct _pulse   pulse;
		// your other message structures would go here too
	} my_message_t;

	struct sigevent         event;
	struct itimerspec       itime;
	timer_t                 timer_id;
	int                     chidTimer;
	int                     rcvidTimer;
	my_message_t            msg;
	printf("the timer channel has NOT created.");
	chidTimer = ChannelCreate(0); // Create a communications channel
	printf("the timer channel has created.");


	event.sigev_notify = SIGEV_PULSE;

	// create a connection back to ourselves for the timer to send the pulse on
	event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chidTimer, _NTO_SIDE_CHANNEL, 0);
	int oldCheckClient0Counter = 0;
	event.sigev_code = MY_PULSE_CODE;

	oldCheckClient0Counter = checkClient0Counter;

	if (timer_create(CLOCK_REALTIME, NULL, &timer_id) == -1)
	{
	   printf (stderr, "%s:  couldn't create a timer, errno %d\n",errno);
	   perror (NULL);
	   exit (EXIT_FAILURE);
	}
	//printf("Fixed Sequence Traffic Lights State Machine with Perodic Timer\n");

	itime_a.it_value.tv_sec = 3;
	itime_a.it_value.tv_nsec = 0;
	itime_a.it_interval.tv_sec =3;
	itime_a.it_interval.tv_nsec = 0;

	timer_settime(timer_id, 0, &itime_a, NULL);


	while(1)
	{
		// wait for message/pulse
		rcvidTimer = MsgReceive(chidTimer, &msg, sizeof(msg), NULL);
		if (rcvidTimer == 0)
		{
			//recevied a pulse, check with 'code' filed
			if (msg.pulse.code == MY_PULSE_CODE) // we got a pulse
			{
				if(oldCheckClient0Counter ==checkClient0Counter)
				{
					client0Alive = 1;
					printf("client alive info has been updated to true\n");
				}
				else
				{
					client0Alive = 0;
					printf("client alive info has been updated to false\n");

				}
			}
		}
	}

	return EXIT_SUCCESS;
}*/
void *ex_timer(void * val)
{
	int oldCheckClient0Counter = checkClient0Counter;

	int number_of_seconds = 30;
	//Converting time into milli_seconds
	int milli_seconds = 1000 * number_of_seconds;
    // Start the timer and the loop
	//while (1){
		clock_t start_time = clock();
		while(clock() <= start_time + milli_seconds){;}
		if(clock() >= start_time + milli_seconds){
			if(oldCheckClient0Counter != checkClient0Counter)
			{
				client0Alive = 1;
				printf("client alive info has been updated to true\n");
			}
			else
			{
				client0Alive = 0;
				printf("client alive info has been updated to false\n");
			}
		}
	//}
	return EXIT_SUCCESS;
}

