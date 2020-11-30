#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define CHILDS 10
#define BUFFER_SIZE 50

struct w
{
    int integer;
    char string[100];
};

int main()
{
    int fd[2];
    int pids[CHILDS];
    int i, status;
    struct w win_parent;
    struct w input;

    if (pipe(fd) == -1)
    {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }

    for (i = 0; i < CHILDS; i++)
    {
        pids[i] = fork();
        if (pids[i] == 0) //processo filho
        {
            read(fd[0], &input, sizeof(input));
            printf("%s round %d\n", input.string, input.integer);
            if (i == CHILDS - 1)
            {
                close(fd[0]);
                close(fd[1]);
            }
            exit(input.integer);
        }
        else if (pids[i] > 0)
        {
            sleep(2);
            win_parent.integer = i;
            strcpy(win_parent.string, "Win");
            write(fd[1], &win_parent, sizeof(win_parent));
        }
    }

    for (i = 0; i < CHILDS; i++)
    {
        waitpid(pids[i], &status, 0);
        printf("PID: %d won round %d\n", pids[i], WEXITSTATUS(status) + 1);
    }

    return 0;
}