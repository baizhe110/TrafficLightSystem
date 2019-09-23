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
enum states{
	state0, state1, state2, state3, state4, state5, state6
};
enum states currentState;
int pid;
int chid;

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;



//void *ex_test1(void * val);
void *ex_server(void * val);
//void *ex_client(void * val);

char test1();

char key[10];



void Initialise0();
void DoSomething1();
void DoSomething2();
void DoSomething3();
void DoSomething4();
void DoSomething5();
void DoSomething6();


int main(int argc, char *argv[])
{
	char val = 'e';
	key[0] = 'e';
	my_data msg;
	printf("Fixed Sequence Traffic Lights State Machine Example\n");
	pthread_t  th1;
	pthread_t  th2_server;
	//pthread_t  th3_client;

	// create the producer and consumer threads
    //pthread_create (&th1, NULL, ex_test1, &val);
    //create server thread
    pthread_create (&th2_server, NULL, ex_server, &val);
    //create client thread
    //pthread_create (&th3_client, NULL, ex_client, &val);

	int Runtimes=50, counter = 0;

	currentState = state0;

	while (counter < Runtimes)
	{

		traffic_light(currentState);
		counter++;
	}

	printf("Finishing demo...");
	return EXIT_SUCCESS;
}

void traffic_light()
{
	switch(currentState)
			{
			case state0:
				Initialise0();
				currentState = state1;
				break;
			case state1:
				DoSomething1();
					currentState = state2;
				break;
			case state2:
				DoSomething2();
				if(key[0] == 'e')
					currentState = state3;
				else
					currentState = state5;
				break;
			case state3:
				DoSomething3();
					currentState = state4;
				break;
			case state4:
				DoSomething4();
					currentState = state5;
				break;
			case state5:
				DoSomething5();
				if(key[0]=='n')
					currentState = state6;
				else
					currentState = state2;
				break;
			case state6:
				DoSomething6();
					currentState = state1;

				break;
			}
}

void Initialise0(){
	printf("the current state is: state0, EWR-NSR\n");
	sleep(1);
}
void DoSomething1()
{
	printf("the current state is: state1, EWR-NSR\n");
	sleep(1);
}
void DoSomething2()
{
	printf("the current state is: state2, EWG-NSR\n");
	sleep(2);
}
void DoSomething3(){
	printf("the current state is: state3, EWY-NSR\n");
	sleep(1);
}
void DoSomething4(){
	printf("the current state is: state4, EWR-NSR\n");
	sleep(1);

}
void DoSomething5(){
	printf("the current state is: state5, EWR-NSG\n");
	sleep(2);
}
void DoSomething6(){
	printf("the current state is: state6, EWR-NSY\n");
	sleep(1);
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
		int rcvid=0, msgnum=0;  	// no message received yet
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
			   msgnum++;

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
			   sprintf(replymsg.buf, "Message %d received", msgnum);
			   printf("Server received data packet with value of '%c' from client (ID:%d), ", msg.data, msg.ClientID);
				   fflush(stdout);
				   key[0] = msg.data;
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


		return EXIT_SUCCESS;
	}
/*
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
*/
