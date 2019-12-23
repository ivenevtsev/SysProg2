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
#include <math.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX_SEND_BUF 1000
#define MAX_RECV_BUF 1000
#define MAX_DATA 1000
#define  MAXLINE 1000


void reverse(char* str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x) {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }

    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}

// Converts a floating-point/double number to a string.
void ftoa(float n, char* res, int afterpoint)
{
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    // convert integer part to string
    int i = intToStr(ipart, res, 0);

    // check for display option after point
    if (afterpoint != 0) {
        res[i] = '.'; // add dot

        fpart = fpart * pow(10, afterpoint);

        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}

struct Triangle{
    int errorCode;
    double coordinates[6];
    double square;
};

struct Triangle proceedLineToCoordinates(char * line){
    struct Triangle currentTriangle;
    currentTriangle.errorCode = 0;
    currentTriangle.square = 0.0;
    for (int i = 0; i < 6; i++){
        currentTriangle.coordinates[i] = 0;
    }
    int numberOfCoordinates = 0;
    printf("%s\n", line);
    int i = 0, maxi = strlen(line);
    //printf("%d - maxi\n", maxi);
    while ( i < maxi){ //&& line[i] != '\n' && line[i] != '\0'){
        //printf("%d - tuc4\n", numberOfCoordinates);
        if (numberOfCoordinates == 6){
            currentTriangle.errorCode = 1;
            break;
        }
        char *currentNumber = NULL;
        currentNumber = (char *) malloc(sizeof(char));

        int numberOfSymbolsInWord = 0;
        //printf("%d - i\n", i);
        while (i < maxi && line[i] != ' ' && line[i] != '\n' && line[i] != '\0' ) {// доделать
            //printf("%d - tuc3\n", numberOfCoordinates);
            currentNumber = (char *) realloc(currentNumber, (numberOfSymbolsInWord + 1) * sizeof(char));
            currentNumber[numberOfSymbolsInWord] = line[i];

            ++numberOfSymbolsInWord;
            ++i;
            //printf("%d - tuc3\n", numberOfCoordinates);
        }

        currentNumber = (char *) realloc(currentNumber, (numberOfSymbolsInWord + 1) * sizeof(char));
        currentNumber[numberOfSymbolsInWord] = '\0';
        //printf("%s\n", currentNumber);
        //logic of transforming string to number
        //printf("%d - tuc1\n", numberOfCoordinates);
        double number = strtod(currentNumber, '\0');
        currentTriangle.coordinates[numberOfCoordinates] = number;
        //printf("%lf\n", number);
        ++numberOfCoordinates;
        //printf("%d - tuc2\n", numberOfCoordinates);
        ++i;
    }
    //обрабатать что мб не числа
    if (numberOfCoordinates != 6){
        printf("%d - tuuuuc\n", numberOfCoordinates);
        currentTriangle.errorCode = 1;
        printf("%d - tuc4567890\n", numberOfCoordinates);
    }
    //printf("%d - her error\n", currentTriangle.errorCode);
    return currentTriangle;
}

struct Triangle getSquare(struct Triangle triangle){
    //printf("im here get square");
    if (triangle.errorCode == 0) {
        //printf("im here ger s in if");
        double x1 = triangle.coordinates[0];
        double y1 = triangle.coordinates[1];
        double x2 = triangle.coordinates[2];
        double y2 = triangle.coordinates[3];
        double x3 = triangle.coordinates[4];
        double y3 = triangle.coordinates[5];
        //printf("im here ger s in if after");

        //printf("%lf %lf %lf %lf %lf %lf", x1, y1, x2, y2, x3, y3);
        double S = 0.5 * abs((x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1));

        triangle.square = S;

        if (triangle.square == 0.0)
            triangle.errorCode = 3;
    }

    return triangle;
}


extern int errno;

int errexit(const char *format,...);
int connectUDP(const char *service,int portnum);
int connectsock(const char *service,int portnum,const char *transport);
void handler(int);

/*------------------------------------------------------------------------------------
 * connectsock-Allocate and connect socket for UDP
 *------------------------------------------------------------------------------------
*/

int connectsock(const char *service,int portnum,const char *transport)
{
/*
Arguments:
*service   - service associated with desired port
*transport - name of the transport protocol to use
*/
    printf("connectsock\n");
    struct sockaddr_in server;                                                //an internet endpoint address

    int server_socket,type,b,l,accept_socket,num;                             //two socket descriptors for listening and accepting

    memset(&server,0,sizeof(server));

    server.sin_addr.s_addr=htons(INADDR_ANY);                                 //INADDR_ANY to match any IP address
    server.sin_family=AF_INET;                                                //family name
    server.sin_port=htons(portnum);                                              //port number
/*
 * to determine the type of socket
 */  type=SOCK_DGRAM;

    server_socket = socket(AF_INET,type,0);                                    //allocate a socket

    if(server_socket<0)
    {
        printf("Socket can't be created\n");
        exit(0);
    }

/* to set the socket options- to reuse the given port multiple times */
    int enable = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        printf("setsockopt(SO_REUSEADDR) failed");
        exit(0);
    }

/* bind the socket to known port */
    b=bind(server_socket,(struct sockaddr*)&server,sizeof(server));

    if(b<0)
    {
        printf("Error in binding\n");
        exit(0);
    }

    return server_socket;

}

/*------------------------------------------------------------------------
 * connectUDP-connect to a specified UDP service on specified host
 -------------------------------------------------------------------------*/
int connectUDP(const char *service,int portnum)
{
    printf("connectUDP\n");
/*
 Arguments:
 *service-service associated with desired port
 */
    return connectsock(service, portnum, "udp");

}


void handler(int sig)
{
    printf("handler\n");
    int status;
    while(wait3(&status,WNOHANG,(struct rusage *)0)>=0);
}


int errexit(const char* format,...)
{
    va_list args;

    va_start(args,format);
    vfprintf(stderr,format,args);
    va_end(args);
    exit(1);
}

/*
 main - connectionless multiprocess server 
 */
int main(int argc,char *argv[]){
    char *service = "echo";

    int portnum = atoi(argv[1]);

    int msock;
    char data[1000];

    /* call connectTCP to create a socket, bind it and place it in passive mode
       once the call returns call accept on listening socket to accept the incoming requests
     */

    msock = connectUDP(service, portnum);
    printf("Listening to client\n");

    (void) signal(SIGCHLD, handler);


    while (1) {
        struct sockaddr_in fsin;
        printf("while%s\n");
        int alen = sizeof(fsin);

        int data_len;

        data_len = recvfrom(msock, data, MAX_DATA, 0, (struct sockaddr *) &fsin, &alen);

        if (data_len) {
            printf("connected to multiforked connectionless server\n");
            printf("File name recieved: %s\n", data);

            switch (fork()) {
                case 0:
                {/* child */
                    /*if ((file = open(data, O_RDONLY)) < 0) {

                        printf("File not found\n");
                        printf("Client disconnected\n");
                    } else {// */

                        printf("File opened successfully\n");

                        struct Triangle result = getSquare(proceedLineToCoordinates(data));
                        //printf("imhhh here\n");

                        char *message;

                        if (result.errorCode == 0){
                            message = (char *) malloc(20 *sizeof(char));
                            ftoa(result.square, message, 3);
                            //message = "Correct";
                        } else {
                            //printf("im here else\n");
                            message = (char *) malloc(9 * sizeof(char));
                            strcpy(message, "ERROR ");
                            message[6] = (char)(result.errorCode + (int)'0');

                            //printf("%s\n", message);

                        }
                        printf("%s\n", message);
                        send(msock, message, MAXLINE, 0);
                        printf("%s\n", message);
                        //sendto(listenfd, message, MAXLINE, 0,  (struct sockaddr *) &fsin, sizeof(fsin));
                    //sendto(listenfd, message, MAXLINE, 0,  (struct sockaddr *) NULL, sizeof(fsin));
                    //sendto(sockfd, inputText, numberOfSymbolsInText, 0, (struct sockaddr*)NULL, sizeof(fsin));
                        /*
                        ssize_t read_bytes;
                        ssize_t sent_bytes;

                        char send_buf[MAX_SEND_BUF];


                        while ((read_bytes = read(file, send_buf, MAX_RECV_BUF)) > 0) {

                            printf("%s", send_buf);


                            if ((sent_bytes = sendto(msock, send_buf, read_bytes, 0, (struct sockaddr *) &fsin,
                                                     sizeof(fsin)) < read_bytes)) {
                                printf("send error\n");
                                return -1;
                            }
                        }
                        close(file); // */
                        printf("\nclient disconnected\n");
                }

                default: /* parent */

                    break;
                case -1:
                    printf("error in forking\n");
            }

        }


    }


    return 0;
}