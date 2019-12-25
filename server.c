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

int sock, count, good, bad;
char *serverIP = "127.0.0.1";
int serverPort = 8088;
int delay = 0;
clock_t begin;

void check(int ret) {
    if (ret < 0) {
        fprintf(stderr, "ERROR: failed to set proc mask: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

char *currentTimestamp() {
    time_t timer;
    char *buffer = (char *) malloc(sizeof(char) * 26);
    struct tm *tm_info;
    time(&timer);
    tm_info = localtime(&timer);
    strftime(buffer, 26, "%d.%m.%Y %H:%M:%S", tm_info);
    return buffer;
}
FILE *logFile;

void error(char *err) {
    if (logFile == NULL) {
        printf("%s\t%s\n", currentTimestamp(), err);
        fflush(stdin);
    } else {
        fprintf(logFile, "%s\t%s\n", currentTimestamp(), err);
        fflush(logFile);
        fclose(logFile);
    }
    perror(err);
    exit(1);
}

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
    for (int j = 0; j < 6; j++){
        currentTriangle.coordinates[j] = 0;
    }
    int numberOfCoordinates = 0;
    printf("%s\n", line);
    int i = 0, maxi = strlen(line);
    while ( i < maxi){
        if (numberOfCoordinates == 6){
            //printf("here error %d %d %d\n", numberOfCoordinates, i, maxi);
            currentTriangle.errorCode = 1;
            error("ERROR 1 format");
        }
        char *currentNumber = NULL;
        currentNumber = (char *) malloc(sizeof(char));

        int numberOfSymbolsInWord = 0;
        while (i < maxi && line[i] != ' ' && line[i] != '\n' && line[i] != '\0' && line[i] != '\t' ) {// доделать
            currentNumber = (char *) realloc(currentNumber, (numberOfSymbolsInWord + 1) * sizeof(char));
            currentNumber[numberOfSymbolsInWord] = line[i];
            if (line[i] > '9' || line[i] < '0'){
                currentTriangle.errorCode = 4;
                error("ERROR 4 not a digit");
            }
            ++numberOfSymbolsInWord;
            ++i;
        }
        currentNumber = (char *) realloc(currentNumber, (numberOfSymbolsInWord + 1) * sizeof(char));
        currentNumber[numberOfSymbolsInWord] = '\0';

        double number = strtod(currentNumber, '\0');
        printf("cord %lf #%d\n", number, numberOfCoordinates);
        currentTriangle.coordinates[numberOfCoordinates] = number;
        ++numberOfCoordinates;
        do{
            ++i;
            printf("i %d\n", i);
        }while (i < maxi && (line[i] == '\n' || line[i] == ' ' || line[i] == '\t' || line[i] == '\0'));
    }
    //обрабатать что мб не числа
    if (numberOfCoordinates != 6){
        currentTriangle.errorCode = 1;
        error("ERROR 1 format");
    }
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
            error("ERROR 3 not a triangle");
    }

    return triangle;
}


void quit() {
    printf("\n%s\tЗавершение работы сервера\n", currentTimestamp());
    fprintf(logFile, "%s\tЗавершение работы сервера\n", currentTimestamp());
    fflush(logFile);
    fflush(stdin);
    close(sock);
    fclose(logFile);
    exit(0);
}

void quitWithLog() {
    clock_t end = clock();
    double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    fprintf(logFile, "Завершение работы сервера.\n"
    "Сервер работал: %f\n"
    "Обслуженные запросы: %d\n"
    "Текущее время: %s\n"
    "Количество обработанных запросов: %d\n"
    "Количество ошибочных запросов запросов:%d\n", time_spent, count, currentTimestamp(), good, bad);
    printf("Завершение работы сервера.\n"
           "Сервер работал: %f\n"
           "Обслуженные запросы: %d\n"
           "Текущее время: %s\n"
           "Количество обработанных запросов: %d\n"
           "Количество ошибочных запросов запросов:%d\n", time_spent, count, currentTimestamp(), good, bad);
    fflush(logFile);
    fflush(stdin);
    fflush(stdin);
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
    char *help = "\033[1mNAME\033[0m\n\t - UDP server, that calculate square\n"
                 "\033[1mSYNOPSIS\033[0m\n\t[OPTIONS]\n"
                 "\033[1mDESCRIPTION\033[0m\n"
                 "\t-w=N\n\t\tset delay N for client \n"
                 "\t-d\n\t\tdaemon mode\n"
                 "\t-l=path/to/log\n\t\tPath to log-file\n"
                 "\t-a=IP\n\t\tset server listening IP\n"
                 "\t-p=PORT\n\t\tset server listening PORT\n"
                 "\t-v\n\t\tcheck program version\n"
                 "\t-h\n\t\tprint help\n";
    int rez;
    int daemonFlag = 0;
    char *logPath = "/tmp/lab2.log";
    begin = clock();
    struct sigaction sa = {0};
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);
    sigaddset(&sa.sa_mask, SIGQUIT);
    sigaddset(&sa.sa_mask, SIGTERM);
    sa.sa_sigaction = quit;
    sa.sa_flags |= (SA_SIGINFO | SA_RESTART); // mind SA_RESTART here!
    check(sigaction(SIGINT, &sa, NULL));
    check(sigaction(SIGQUIT, &sa, NULL));
    check(sigaction(SIGTERM, &sa, NULL));
    struct sigaction sa1 = {0};
    sigemptyset(&sa1.sa_mask);
    sigaddset(&sa1.sa_mask, SIGUSR1);
    sa1.sa_sigaction = quitWithLog;
    sa1.sa_flags |= (SA_SIGINFO | SA_RESTART);
    check(sigaction(SIGUSR1, &sa1, NULL));
    if (getenv("L2ADDR") != NULL) {
        serverIP = getenv("L2ADDR");
    }
    if (getenv("L2PORT") != NULL) {
        serverPort = atoi(getenv("L2PORT"));
    }
    if (getenv("L2WAIT") != NULL) {
        delay = atoi(getenv("L2WAIT"));
    }
    while ((rez = getopt(argc, argv, "w:dl:a:p:vh")) != -1) {
        switch (rez) {
            case 'w':
                delay = atoi(optarg);
                break;
            case 'd':
                daemonFlag = 1;
                break;
            case 'l':
                if (strncmp(optarg, "", 1) == 0) {
                    printf("%s", help);
                    return 0;
                }
                logPath = strdup(optarg);
                break;
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
                printf("\033version 36\n");
                return 0;
            default:
                printf("%s", help);
                return 0;
        }
    }

    if (daemonFlag == 1) {
        pid_t pid, sid;
        pid = fork();
        if (pid < 0) {
            error("ERROR 99 Fork failed ");
        }
        if (pid > 0) {
            exit(0);
        }

        sid = setsid();
        if (sid < 0) {
            error("ERROR 98 SID failed ");
        }
        signal(SIGCHLD, SIG_IGN);
        signal(SIGHUP, SIG_IGN);
        if ((pid = fork()) < 0) {
            error("ERROR 99 Fork failed ");
        }
        if (pid > 0) {
            exit(0);
        }
        umask(0);
        chdir("/");
        int fd;
        for (fd = sysconf(_SC_OPEN_MAX); fd > 0; --fd) {
            close(fd);
        }
        stdin = fopen("/dev/null", "r");
        stdout = fopen("/dev/null", "w+");
        stderr = fopen("/dev/null", "w+");
        if ((logFile = fopen(logPath, "rb+")) == NULL) {
            error("ERROR 24: Open file");
        }

    }

    printf("connectsock\n");
    struct sockaddr_in server;                                                //an internet endpoint address

    memset(&server, 0, sizeof(server));

    server.sin_addr.s_addr= htons(INADDR_ANY);                                 //INADDR_ANY to match any IP address
    server.sin_family = AF_INET;                                                //family name
    server.sin_port = htons(serverPort);                                              //port number

    sock = socket(AF_INET, SOCK_DGRAM, 0);                                    //allocate a socket

    if(sock<0)
    {
        printf("Socket can't be created\n");
        exit(0);
    }

/* to set the socket options- to reuse the given port multiple times */
    int enable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        printf("setsockopt(SO_REUSEADDR) failed");
        exit(0);
    }

/* bind the socket to known port */
    if(bind(sock, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        printf("Error in binding\n");
        exit(0);
    }

    printf("Listening to client\n");

    while (1) {
        socklen_t addrLength = sizeof(struct sockaddr);

        char data[MAX_DATA];
        memset(data, 0, MAX_DATA);
        int messLength = 0;
        messLength = recvfrom(sock, (char *)data, sizeof(data), 0,  (struct sockaddr *) &server, &addrLength);

        printf("len %d\n", messLength);

        if (data == NULL) {
            error("ERROR 52: Malloc failed");
        }

        printf("recieved: %s\n", data);
        sleep(delay); //Если необходима задержка

        //  fprintf(logFile, "\n%s\tMessage %s received from(%s , %d)\n", currentTimestamp(), data,
        //        inet_ntoa(args.clientAddress.sin_addr),
        //      ntohs(args.clientAddress.sin_port)); // Запись в лог файл полученного сообщения

        switch (fork()) {
            case 0: {/* child */
                struct Triangle result = getSquare(proceedLineToCoordinates(data));

                char *message;

                if (result.errorCode == 0) {
                    message = (char *) malloc(20 * sizeof(char));
                    ftoa(result.square, message, 3);
                    //message = "Correct";
                } else {
                    //printf("im here else\n");
                    message = (char *) malloc(9 * sizeof(char));
                    strcpy(message, "ERROR ");
                    message[6] = (char) (result.errorCode + (int) '0');

                    //printf("%s\n", message);

                }
                sendto(sock, message, MAXLINE, 0, (struct sockaddr *) &server, addrLength);
                printf("result %s\n", message);

                printf("\nclient disconnected\n");
            }

            default: /* parent */
                break;
            case -1:
                printf("error in forking\n");
        }
    }

    return 0;
}