#include <stdio.h>
#define _USE_BSD 1
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include<netinet/in.h>


#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff
#endif

#define MAX_RECV_BUF 1000
#define MAX_SEND_BUF 1000

char *serverIP = "127.0.0.1";
int serverPort = 8088;
//FILE *logFile;


extern int errno;
int errexit(const char *format,...);
int connectUDP(const char *service,const char *host,int portnum);
int connectsock(const char *service,const char *host,int portnum,const char *transport);

/*------------------------------------------------------------------------------------
 * connectsock-Allocate and connect socket for UDP
 *------------------------------------------------------------------------------------
*/

int connectsock(const char *service,const char *host,int portnum,const char *transport){
    printf("connectsock\n");
    /*
    Arguments:
    *service   - service associated with desired port
    *host      - name of the host to which connection is desired
    *transport - name of the transport protocol to use
*/ 

    struct sockaddr_in sin; 					 //an internet endpoint address
    int s,type;               					 //socket descriptor and socket type
    memset(&sin,0,sizeof(sin));
    sin.sin_family=AF_INET;   				         //family name
    sin.sin_port=htons(portnum);                                        //port number
    inet_pton(AF_INET,host,&(sin.sin_addr));                         //to convert host name into 32-bit IP address
 
/*
 * to determine the type of socket
 */
    if(strcmp(transport,"udp")==0)
        type=SOCK_DGRAM;
    else
        type=SOCK_STREAM;

/* Allocate a socket */
    s=socket(AF_INET,type,0);
    if(s<0)
        errexit("can't create socket : %s\n",strerror(errno));

    if((connect(s,(struct sockaddr *) &sin,sizeof(sin)))<0)        //connect the socket
        errexit("can't connect to %s.%s: %s\n",host,service,strerror(errno));

    return s;
}

/*
 * errexit- print and error message and exit
 */


int errexit(const char* format,...){
    printf("errexit\n");
    va_list args;

    va_start(args,format);
    vfprintf(stderr,format,args);
    va_end(args);
    exit(1);
}


/*------------------------------------------------------------------------
 * connectUDP-connect to a specified UDP service on specified host
 -------------------------------------------------------------------------*/

int connectUDP(const char *service,const char *host,int portnum)
{
    printf("connectUDP\n");
/*
 Arguments:
 *service-service associated with desired port
 *host-name of the host to which connection is desired
 */

    return connectsock(service,host,portnum,"udp");
}


/*
 main - UDP client file
 */ 

int main(int argc,char *argv[])
{

    char *file_name;
    ssize_t recv_bytes;
    char recv_buff[1000];

    char *service="time";                                               //default service port
    int sockfd;                                                           //socket descriptor

   /* switch(argc)
    {
        case 1:
            host="localhost";
            printf("case 1\n");
        break;

        case 2:
            host="localhost";
            printf("case 2\n");
        break;
        case 4:
            printf("case 1\n");
            file_name=argv[3];
            portnum=atoi(argv[2]);
            host=argv[1];
        break;

         default:
             printf("Error in taking arguments\n");
             exit(1); // */



    struct sockaddr_in fsin;

    sockfd = connectUDP(service, serverIP, serverPort);

    //char buffer[100]; //recv_buff

    char *inputText = NULL;
    inputText = (char*)malloc(sizeof(char));
    int numberOfSymbolsInText = 0;
    char currentSymbol = fgetc(stdin);

    while (currentSymbol != '\n')//чтение
    {
        inputText = (char *) realloc(inputText, (numberOfSymbolsInText + 1) * sizeof(char));
        inputText[numberOfSymbolsInText] = currentSymbol;
        currentSymbol = fgetc(stdin);
        numberOfSymbolsInText++;
    }
    inputText = (char *) realloc(inputText, (numberOfSymbolsInText + 1) * sizeof(char));
    inputText[numberOfSymbolsInText] = currentSymbol;
    numberOfSymbolsInText++;


    //struct sockaddr_in servaddr;


// request to send datagram
// no need to specify server address in sendto
// connect stores the peers IP and port
    sendto(sockfd, inputText, numberOfSymbolsInText, 0, (struct sockaddr*)NULL, sizeof(fsin));
    recvfrom(sockfd, recv_buff, MAX_RECV_BUF, 0,  (struct sockaddr*)NULL, NULL);
    printf("%s\n", recv_buff);
    puts(recv_buff);


// close the descriptor
    //close(sockfd);


    exit(0); //*/

}