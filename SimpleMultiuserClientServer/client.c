#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>

#define PORT 8080
#define BUFFSIZE 4096
#define SOCKET_ERROR -1
#define CMD_LEN 100

int main(int argc, char* argv[])
{
    int client_sock = 0;
    int server_sock = 0;
    char buffer[BUFFSIZE] = {0};
    char text[CMD_LEN] = {0};
    struct sockaddr_in server_addr;
    
    if ( (client_sock = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR )
    {
        perror("Failed to create a socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;


    if ( inet_pton(AF_INET, "192.168.100.7", &server_addr.sin_addr) == SOCKET_ERROR )
    {
        perror("Invalid Address/Address not supported");
        close(client_sock);
        exit(EXIT_FAILURE);
    }

    if ( connect(client_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR )
    {
        perror("Connection failed");
        close(client_sock);
        exit(EXIT_FAILURE);
    }
    printf("You are connected successfully\n");

    while ( 1 )
    {
        printf("Send: ");
        fgets(text, BUFFSIZE, stdin);

        if ( (send(client_sock, text, strlen(text), 0)) == SOCKET_ERROR )
        {
            printf("Failed to send info\n");
            fflush(stdin);
            exit(EXIT_FAILURE);
        }

        if ( !strcmp(text, "q\n") )
        {
            printf("Bye\n");
            fflush(stdin);
            close(client_sock);
            return 0;
        }

        int status = 0;

        if ( (status = read(client_sock, buffer, BUFFSIZE - 1)) <= 0 )
        {
            printf("Failed to recieve info\n");
            exit(EXIT_FAILURE);
        }
        buffer[status] = '\0';
        printf("Received: %s\n", buffer);
        fflush(stdin);
        fflush(stdout);
        memset(buffer, 0, BUFFSIZE);
        memset(text, 0, CMD_LEN);
    }
    close(client_sock);

    return 0;
}