#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

void ret_shell(int sock_fd, char *cmd)
{
    int fd[2];
    if ( pipe(fd) < 0 )
    {
        perror("pipe");
        return;
    }

    pid_t pid = fork();
    if ( pid < 0 )
    {
        perror("fork");
    }

    if ( pid == 0 )
    {
        dup2(STDOUT_FILENO, fd[1]);
        close(fd[0]);
        close(fd[1]);
        char *argv[] = {
            "/bin/sh",
            "-c",
            cmd,
            NULL
        };
    }
    wait(NULL);
    dup2(STDIN_FILENO, fd[0]);
    dup2(STDOUT_FILENO, sock_fd);
    close(fd[0]);
    close(fd[1]);
}

int main()
{
    int f = open("logs.txt", O_RDWR | O_CREAT);
    if ( f == -1 )
    {
        perror("open");
    }
    char *cmd = "ls";
    ret_shell(f, cmd);
    close(f);
}