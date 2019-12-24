// server program for udp connection
#include <stdio.h>
#define _USE_BSD 1
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdarg.h>
#include<netinet/in.h>
#include <math.h>
#include <string.h>
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
char *serverIP = "127.0.0.1";
int serverPort = 8088;
int delay = 0;
int sock;
FILE *logFile;

char *currentTimestamp() { //Создание текущей временной отметки в заданном формате
    time_t timer;
    char *buffer = (char *) malloc(sizeof(char) * 26);
    struct tm *tm_info;
    time(&timer);
    tm_info = localtime(&timer);
    strftime(buffer, 26, "%d.%m.%Y %H:%M:%S", tm_info);
    return buffer;
}

void error(char *err) { //Вывод ошибок и запись в лог
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
    if (triangle.errorCode == 0) {
        double x1 = triangle.coordinates[0];
        double y1 = triangle.coordinates[1];
        double x2 = triangle.coordinates[2];
        double y2 = triangle.coordinates[3];
        double x3 = triangle.coordinates[4];
        double y3 = triangle.coordinates[5];
        double S = 0.5 * abs((x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1));

        triangle.square = S;

        if (triangle.square == 0.0)
            triangle.errorCode = 3;
    }

    return triangle;
}

void serverHandler(int delay) {
    printf("serverHandler(int delay) {\n");
    socklen_t addrLength;
    printf("1\n");
    ssize_t messLength;
    struct sockaddr_in serverAddress;
    int optval = 1;
    printf("1\n");
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) { // Создание сокета
        printf("error(\"ERROR 73: Socket failed\");\n");
        error("ERROR 73: Socket failed");
    }
    printf("1\n");
    fprintf(logFile, "%s\tSocket created\n", currentTimestamp()); // Запись в лог момента создания сокета
    printf("1\n");
    memset(&serverAddress, 0, sizeof(serverAddress));
    printf("1\n");
    serverAddress.sin_family = AF_INET; //IP4 internetwork: UDP, TCP, etc.
    serverAddress.sin_port = htons(serverPort); //Устанавливаем порт
    serverAddress.sin_addr.s_addr = inet_addr(serverIP); //Указанный IP
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void *) &optval, sizeof(int)); // Возможность переиспользовать указанный адрес
    printf("tyt\n");
    if (bind(sock, (struct sockaddr *) &serverAddress, // Привязка сокета к определенному порту и адресу
             sizeof(struct sockaddr)) == -1) {
        printf("error(\"ERROR 42: Binding failed\");\n");
        error("ERROR 42: Binding failed");
    }
    fprintf(logFile, "%s\tBinding done\n", currentTimestamp());

   // addrLength = sizeof(struct sockaddr);

    fprintf(logFile, "%s\tUDP-Server Waiting for client on port %d\n", currentTimestamp(), serverPort);
    fflush(logFile);

    while ("endless cycle") { // Бесконечный цикл прослушки сервера
        printf("while (\"endless cycle\") ");
        char data[MAX_DATA];
        int data_len;

      //  data_len = recvfrom(sock, data, MAX_DATA, 0, (struct sockaddr *) &serverAddress, &addrLength);

        if (data_len) {
            printf("connected to multiforked connectionless server\n");
            printf("File name recieved: %s\n", data);

            switch (fork()) {
                case 0: {
                    struct Triangle result = getSquare(proceedLineToCoordinates(data));

                    char *message;

                    if (result.errorCode == 0) {
                        message = (char *) malloc(20 * sizeof(char));
                        ftoa(result.square, message, 3);
                        //message = "Correct";
                    } else {
                        message = (char *) malloc(9 * sizeof(char));
                        strcpy(message, "ERROR ");
                        message[6] = (char) (result.errorCode + (int) '0');
                    }

                   // if (sendto(sock, message, MAXLINE, 0, (const struct sockaddr *) &serverAddress, addrLength) == -1) {
                     //   printf("err %s\n", message);
                   // }
                    printf("\nclient disconnected\n");
                }
                default:
                    break;
            }
        }
    }
}
      /*  struct arg_struct args;

        memset(&args, 0, sizeof(struct arg_struct));
        messLength = recvfrom(sock, NULL, 0, MSG_PEEK | MSG_TRUNC,
                              (struct sockaddr *) &args.clientAddress,
                              &addrLength); // Получаем длину сообщения от клиента
        args.data = (char *) malloc(messLength); // Выделение памяти под сообщение
        if (args.data == NULL) {
            error("ERROR 52: Malloc failed");
        }
        recvfrom(sock, args.data, messLength, 0,
                 (struct sockaddr *) &args.clientAddress, &addrLength); // Чтение самого сообщения
        args.data[messLength] = '\0';
        sleep(delay); //Если необходима задержка
        fprintf(logFile, "\n%s\tMessage %s received from(%s , %d)\n", currentTimestamp(), args.data,
                inet_ntoa(args.clientAddress.sin_addr),
                ntohs(args.clientAddress.sin_port)); // Запись в лог файл полученного сообщения
        fflush(logFile);
       //
    }
}




/*
 main - connectionless multiprocess server
 */
int main(int argc,char *argv[]){
    char *help = "\033[1mNAME\033[0m\n\tKaratist - TCP server, which checks if there are Central polygonal numbers in the string\n"
                 "\033[1mSYNOPSIS\033[0m\n\t Karatist [OPTIONS]\n"
                 "\033[1mDESCRIPTION\033[0m\n"
                 "\t-w=N\n\t\tset delay N for client \n"
                 "\t-d\n\t\tdaemon mode\n"
                 "\t-l=path/to/log\n\t\tPath to log-file\n"
                 "\t-a=IP\n\t\tset server listening IP\n"
                 "\t-p=PORT\n\t\tset server listening PORT\n"
                 "\t-v\n\t\tcheck program version\n"
                 "\t-h\n\t\tprint help\n";

    if (getenv("L2ADDR") != NULL) { // Это и далее - Чтение переменных окружения
        serverIP = getenv("L2ADDR");
    }
    if (getenv("L2PORT") != NULL) {
        serverPort = atoi(getenv("L2PORT"));
    }
    if (getenv("L2WAIT") != NULL) {
         delay = atoi(getenv("L2WAIT"));
    }

    int daemonFlag = 0;


    printf("Listening to client\n");

    //(void) signal(SIGCHLD, handler);
    int rez;
    while ((rez = getopt(argc, argv, "w:dl:a:p:vh")) != -1) { // Обработка ключей терминала
        printf(" while ((rez = getopt(argc, argv, \"w:dl:a:p:vh\")) != -1)\n");
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
                //logPath = strdup(optarg);
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
                printf("\033my version 34.4");
                return 0;
            default:
                printf("%s", help);
                return 0;
        }
    }
    printf("end of while\n");
    serverHandler(delay);

    printf("hmmmmmmmm\n");
    return 0;
}