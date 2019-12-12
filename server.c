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

double triangle(){
    double  x1, y1, x2, y2,  x3, y3, S = 99;
    printf("%lf\n", S);
    scanf("%lf%lf%lf%lf%lf%lf", &x1, &y1, &x2, &y2, &x3, &y3);
    printf("%lf\n", x1);
    printf("%lf\n", y1);
    printf("%lf\n", x2);
    printf("%lf\n", y2);
    printf("%lf\n", x3);
    printf("%lf\n", y3);
    S = abs((x2 - x1)*(y3 - y1) - (x3 - x1)*(y2 - y1));
    printf("%lf\n", S);
    S = 0.5*S;
    printf("%lf\n", S);
    return S;
}



// Driver code
int main()
{
    char buffer[100];
    char *message = "Hello Client";
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
                     0, (struct sockaddr*)&cliaddr,&len); //receive message from server
    buffer[n] = '\0';



    puts(buffer);

    // send the response
    sendto(listenfd, message, MAXLINE, 0,
           (struct sockaddr*)&cliaddr, sizeof(cliaddr));
}
