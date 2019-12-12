// server program for udp connection
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include<netinet/in.h>
#include <math.h>
#include <stdlib.h>

#define PORT 5000
#define MAXLINE 1000
//1 - не 6 кординат
//2 не числа, не правильный формат введенных данных
// не треугольник
//

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

struct TriangleSidesCoordinates{
    int errorCode;
    double coordinates[6];
};

struct TriangleSidesCoordinates proceedLineToCoordinates(char * line){
    struct TriangleSidesCoordinates currentTriangleSidesCoordinates;
    int numberOfCoordinates = 0;
    printf("%s\n", line);
    int i = 0;
    while (line[i] != '\n'){
        if (numberOfCoordinates == 6){
            currentTriangleSidesCoordinates.errorCode = 1;
            break;
        }
        char *currentNumber = NULL;
        currentNumber = (char *) malloc(sizeof(char));

        int numberOfSymbolsInWord = 0;

        while (line[i] != ' ' ) {// доделать
            currentNumber = (char *) realloc(currentNumber, (numberOfSymbolsInWord + 1) * sizeof(char));
            currentNumber[numberOfSymbolsInWord] = line[i];

            ++numberOfSymbolsInWord;
            ++i;
        }

        currentNumber = (char *) realloc(currentNumber, (numberOfSymbolsInWord + 1) * sizeof(char));
        currentNumber[numberOfSymbolsInWord] = '\0';
        printf("%s\n", currentNumber);
        //logic of transforming string to number
        double number = strtod(currentNumber, '\0');
        currentTriangleSidesCoordinates.coordinates[numberOfCoordinates] = number;
        printf("%lf\n", number);
        ++numberOfCoordinates;
        ++i;
    }
    //обрабатать что мб не числа
    if (numberOfCoordinates != 6){
        currentTriangleSidesCoordinates.errorCode = 1;
    }
    return currentTriangleSidesCoordinates;
}


double getSquare(struct TriangleSidesCoordinates triangle){
    double S = 0;
    double x1 = triangle.coordinates[0];
    double y1 = triangle.coordinates[1];
    double x2 = triangle.coordinates[2];
    double y2 = triangle.coordinates[3];
    double x3 = triangle.coordinates[4];
    double y3 = triangle.coordinates[5];

    S = 0.5 * abs((x2 - x1)*(y3 - y1) - (x3 - x1)*(y2 - y1));

    return S;
}



// Driver code
int main()
{
    char buffer[400];
    char *message;
    message = (char *) malloc(15 * sizeof(char));
    int listenfd, len;
    struct sockaddr_in servaddr, cliaddr;
    bzero(&servaddr, sizeof(servaddr));

    // Create a UDP Socket
    listenfd = socket(AF_INET, SOCK_DGRAM, 0);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    servaddr.sin_family = AF_INET;

    // bind server address to socket descriptor
    bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    //receive the datagram
    len = sizeof(cliaddr);
    int n = recvfrom(listenfd, buffer, sizeof(buffer),
                     0, (struct sockaddr*)&cliaddr, &len); //receive message from server
    buffer[n] = '\0';
    printf("%s\n", buffer);


    double S = getSquare(proceedLineToCoordinates(buffer));
    printf("%lf", S);
    ftoa(S, message, 10);
    printf("%s\n", message);
    // send the response
    sendto(listenfd, message, MAXLINE, 0,
           (struct sockaddr*)&cliaddr, sizeof(cliaddr));
}
