#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>//->for getnameinfo()



//socket headers
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void report(struct sockaddr_in *serverAddress);

void setHttpHeader(char httpHeader[])
{
    FILE *htmlData = fopen("index.html", "r");

    char line[100];
    char responseData[8000];
    while (fgets(line, 100, htmlData) != 0) {
        strcat(responseData, line);
    }
    strcat(httpHeader, responseData);
}

int main()
{
    char httpHeader[8000] = "HTTP/1.1 200 OK\r\n\n";
    
    int serverSocket = socket(AF_INET, SOCK_STREAM,0);
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8000);
    serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    
    listen(serverSocket, 1);
    report(&serverAddress);
    setHttpHeader(httpHeader);
    int clientSocket;

    while(1) {
        clientSocket = accept(serverSocket, NULL, NULL);
        send(clientSocket, httpHeader, sizeof(httpHeader), 0);
        close(clientSocket);
    }
    return 0;
}

void report(struct sockaddr_in *serverAddress)
{
    char hostBuffer[INET6_ADDRSTRLEN];
    char serviceBuffer[NI_MAXSERV];
    socklen_t addr_len = sizeof(*serverAddress);
    int err = getnameinfo(
        (struct sockaddr *) serverAddress,
        addr_len,
        hostBuffer,
        sizeof(hostBuffer),
        serviceBuffer,
        sizeof(serviceBuffer),
        NI_NUMERICHOST
    );
    printf("http://%s:%s\n", hostBuffer, serviceBuffer);
}