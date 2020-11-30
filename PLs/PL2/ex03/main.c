#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 100

int main()
{

    int p, fd[2], status;

    char helloWorld[BUFFER_SIZE] = "Hello World";
    char goodbye[BUFFER_SIZE] = "Goodbye!";

    char helloWorldR[BUFFER_SIZE];
    char goodbyeR[BUFFER_SIZE];

    if (pipe(fd) == -1) //cria o pipe
    {
        perror("Pipe failed");
        return 1;
    }

    p = fork();

    if (p > 0) //estamos dentro do processo pai
    {
        close(fd[0]);

        write(fd[1], helloWorld, sizeof(helloWorld));
        write(fd[1], goodbye, sizeof(goodbye));

        close(fd[1]);

        waitpid(p, &status, 0);
        printf("PID: %d\n", getpid());
    }
    else if (p == 0) //estamos dentro do processo filho
    {
        close(fd[1]);

        read(fd[0], helloWorldR, sizeof(helloWorldR));
        read(fd[0], goodbyeR, sizeof(goodbyeR));

        printf("%s\n", helloWorldR);
        printf("%s\n", goodbyeR);

        close(fd[0]);

        exit(0);
    }

    return 0;
}