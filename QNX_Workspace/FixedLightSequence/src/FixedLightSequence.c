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



int main(int argc, char *argv[])
{

	printf("Fixed Sequence Traffic Lights State Machine\n");

    //create client thread
	char val = 'e';
	pthread_t  th3_client;
    pthread_create (&th3_client, NULL, ex_client, &val);

	int Runtimes=30, counter = 0;


	while (counter < Runtimes)
	{
		sleep(1);
		counter++;
	}

	printf("Finishing demo...");
	return EXIT_SUCCESS;
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
/*
        //testing reply chnages
        if (reply.buf == '0')
        {
			printf("now the received reply should be 0, actual is %c", reply.buf);
        }
        if(reply.buf == '1')
        {
        	printf("now the received reply should be 1, actual is %c", reply.buf);
        }
        else
        {
        	printf("if you see this means the reply test failed");
        }
*/
        //sleep(5);	// wait a few seconds before sending the next data packet
    }

    // Close the connection
    printf("\n Sending message to server to tell it to close the connection\n");
    ConnectDetach(server_coid);

    return EXIT_SUCCESS;
}
