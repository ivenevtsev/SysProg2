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
int sock;
//FILE *logFile;

int errexit(const char* format,...){
    printf("errexit\n");
    va_list args;

    va_start(args,format);
    vfprintf(stderr, format, args);
    va_end(args);
    exit(1);
}


int main(int argc,char *argv[])
{
    char *help = "\033[1mNAME\033[0m\n\tParadist - Client which is connected to TCP Server (Karatist) to send him string with numbers and receive if there are Central polygonal numbers in there\n"
                 "\033[1mSYNOPSIS\033[0m\n\t Paradist [OPTIONS]\n"
                 "\033[1mDESCRIPTION\033[0m\n"
                 "\t-a=IP\n\t\tset server listening IP\n"
                 "\t-p=PORT\n\t\tset server listening PORT\n"
                 "\t-v\n\t\tcheck program version\n"
                 "\t-h\n\t\tprint help\n";

    int rez;

    if (getenv("L2ADDR") != NULL) {
        serverIP = getenv("L2ADDR");
    }
    if (getenv("L2PORT") != NULL) {
        serverPort = atoi(getenv("L2PORT"));
    }

    while ((rez = getopt(argc, argv, "a:p:vh")) != -1) {
        switch (rez) {
            case 'a':
                if (strncmp(optarg, "", 1) == 0) {
                    printf("%s", help);
                    return 0;
                }
                serverIP = optarg;
                break;
            case 'p':
                if (strncmp(optarg, "", 1) == 0) {
                    printf("%s", help);
                    return 0;
                }
                serverPort = atoi(optarg);
                break;
            case 'v':
                printf("version 34.0\n");
                return 0;
            default:
                printf("%s", help);
                return 0;
        }
    }

    char recv_buff[1000];
    socklen_t addrLength;
    //sock = connectSock();
    struct sockaddr_in serverAddress; 					 //an internet endpoint address
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;   				         //family name
    serverAddress.sin_port = htons(serverPort);                                        //port number
    inet_pton(AF_INET, serverIP, &(serverAddress.sin_addr));                         //to convert host name into 32-bit IP address
    addrLength = sizeof(struct sockaddr);
/* Allocate a socket */
    sock = socket(AF_INET, SOCK_DGRAM,0);

    if(sock < 0)
        errexit("can't create socket\n");

    char *inputText = NULL;
    inputText = (char*)malloc(sizeof(char));
    int numberOfSymbolsInText = 0;

    printf("waiting for data\n");

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
// request to send datagram
// no need to specify server address in sendto
// connect stores the peers IP and port
    sendto(sock, inputText, numberOfSymbolsInText, 0, (const struct sockaddr *) &serverAddress, addrLength);

    recvfrom(sock, recv_buff, MAX_RECV_BUF, 0,  (struct sockaddr *) &serverAddress, &addrLength);
    puts(recv_buff);

    close(sock);

    exit(0); //*/

}