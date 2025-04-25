#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

void ret_shell(void *sock_fd, char *cmd)
{
    int f = *(int*)sock_fd;
    free(sock_fd);

    pid_t pid = fork();
    if ( pid < 0 )
    {
        perror("fork");
    }

    if ( pid == 0 )
    {
	dup2(f, STDOUT_FILENO);
	dup2(f, STDERR_FILENO);
	execlp("/bin/sh", "sh", "-c", cmd, (char*)NULL);
    	perror("execlp");
    }
    wait(NULL);
}
