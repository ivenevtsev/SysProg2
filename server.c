// server program for udp connection
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include<netinet/in.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define PORT 5000
#define MAXLINE 1000
//1 - не 6 кординат
//2 не числа, не правильный формат введенных данных
// 3 не треугольник
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
    printf("%d - maxi\n", maxi);
    while (printf("%d - tuc4\n", numberOfCoordinates) && i < maxi){ //&& line[i] != '\n' && line[i] != '\0'){
        printf("%d - tuc4\n", numberOfCoordinates);
        if (numberOfCoordinates == 6){
            currentTriangle.errorCode = 1;
            break;
        }
        char *currentNumber = NULL;
        currentNumber = (char *) malloc(sizeof(char));

        int numberOfSymbolsInWord = 0;
        printf("%d - i\n", i);
        while (printf("%d - tuc3\n", numberOfCoordinates) && i < maxi && line[i] != ' ' && line[i] != '\n' && line[i] != '\0' ) {// доделать
            printf("%d - tuc3\n", numberOfCoordinates);
            currentNumber = (char *) realloc(currentNumber, (numberOfSymbolsInWord + 1) * sizeof(char));
            currentNumber[numberOfSymbolsInWord] = line[i];

            ++numberOfSymbolsInWord;
            ++i;
            printf("%d - tuc3\n", numberOfCoordinates);
        }

        currentNumber = (char *) realloc(currentNumber, (numberOfSymbolsInWord + 1) * sizeof(char));
        currentNumber[numberOfSymbolsInWord] = '\0';
        printf("%s\n", currentNumber);
        //logic of transforming string to number
        printf("%d - tuc1\n", numberOfCoordinates);
        double number = strtod(currentNumber, '\0');
        currentTriangle.coordinates[numberOfCoordinates] = number;
        printf("%lf\n", number);
        ++numberOfCoordinates;
        printf("%d - tuc2\n", numberOfCoordinates);
        ++i;
    }
    //обрабатать что мб не числа
    if (numberOfCoordinates != 6){
        printf("%d - tuuuuc\n", numberOfCoordinates);
        currentTriangle.errorCode = 1;
        printf("%d - tuc4567890\n", numberOfCoordinates);
    }
    printf("%d - her error\n", currentTriangle.errorCode);
    return currentTriangle;
}

struct Triangle getSquare(struct Triangle triangle){
    printf("im here get square");
    if (triangle.errorCode == 0) {
        printf("im here ger s in if");
        double x1 = triangle.coordinates[0];
        double y1 = triangle.coordinates[1];
        double x2 = triangle.coordinates[2];
        double y2 = triangle.coordinates[3];
        double x3 = triangle.coordinates[4];
        double y3 = triangle.coordinates[5];
        printf("im here ger s in if after");

        printf("%lf %lf %lf %lf %lf %lf", x1, y1, x2, y2, x3, y3);
        double S = 0.5 * abs((x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1));

        triangle.square = S;

        if (triangle.square == 0.0)
            triangle.errorCode = 3;
    }

    return triangle;
}


// Driver code
int main()
{
    char buffer[400];

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

    struct Triangle result = getSquare(proceedLineToCoordinates(buffer));
    printf("imhhh here\n");

    char *message;

    if (result.errorCode == 0){
        message = (char *) malloc(9 *sizeof(char));
        message = "Correct";
    } else {
        printf("im here else\n");
        message = (char *) malloc(9 * sizeof(char));
        strcpy(message, "ERROR ");
        message[6] = (char)(result.errorCode + (int)'0');

        printf("%s\n", message);

    }

    sendto(listenfd, message, MAXLINE, 0,
           (struct sockaddr*)&cliaddr, sizeof(cliaddr));

}
