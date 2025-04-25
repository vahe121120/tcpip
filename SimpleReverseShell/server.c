#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFSIZE 4096

void create_socket(int* s);
void attach(int* socket, struct sockaddr_in* server_addr);
void setListener(int sock);

int main(int argc, char *argv[])
{
    int server_sock = 0, client_sock = 0;
    int reuse = 1;
    char buffer[BUFFSIZE];
    char* text = "ls";

    socket(AF_INET, SOCK_STREAM, 0);
    create_socket(&server_sock);
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &reuse, sizeof(reuse));
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    attach(&server_sock, &server_addr);
    setListener(server_sock);

    if ( (client_sock = accept(server_sock, NULL, NULL)) == -1 )
    {
        printf("accept failed\n");
        exit(EXIT_FAILURE);
    }

    int status = 0;
    if ( (status = read(client_sock, buffer, BUFFSIZE - 1)) <= 0 )
    {
        printf("Failed to receive info\n");
        exit(EXIT_FAILURE);
    }
    buffer[status + 1] = '\0';
    printf("%s\n", buffer);
    if ( (send(client_sock, text, strlen(text), 0)) == -1 )
    {
        printf("Failed to send info\n");
        exit(EXIT_FAILURE);
    }

    close(server_sock);

    return 0;
}

void create_socket(int* s)
{
    *s = socket(AF_INET, SOCK_STREAM, 0);
    if ( *s == -1 )
    {
        printf("Failed to create a socket\n");
        exit(EXIT_FAILURE);
    }
}

void attach(int* socket, struct sockaddr_in* server_addr)
{
    if ( bind(*socket, (struct sockaddr*)server_addr, sizeof(*server_addr)) == -1 )
    {
        printf("Failed to bind\n");
        exit(EXIT_FAILURE);
    }
}

void setListener(int sock)
{
    if ( listen(sock, 3) == -1 )
    {
        printf("Failed to listen to the port %d\n", PORT);
        exit(EXIT_FAILURE);
    }
}