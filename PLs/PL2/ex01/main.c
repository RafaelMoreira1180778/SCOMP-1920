#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{

    int p, fd[2], received;

    if (pipe(fd) == -1) //cria o pipe
    {
        perror("Pipe failed");
        return 1;
    }

    p = fork();

    if (p > 0) //estamos dentro do processo pai
    {
        close(fd[0]);
        int pid = getpid();
        printf("PID dentro do pai: %d\n", pid);
        write(fd[1], &pid, sizeof(pid));
        close(fd[1]);
    }
    else if (p == 0) //estamos dentro do processo filho
    {
        close(fd[1]);
        read(fd[0], &received, sizeof(received));
        printf("O filho leu: %d\n", received);
        close(fd[0]);
    }

    return 0;
}