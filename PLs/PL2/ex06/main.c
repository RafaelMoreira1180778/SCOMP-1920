#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define ARRAY_SIZE 1000
#define CHILDS 5

int main()
{
    int vec1[ARRAY_SIZE], vec2[ARRAY_SIZE];
    int fd[2];
    int pids[CHILDS];
    int i, status, j, tmp, input, sum;

    srand(time(NULL));

    for (i = 0; i < ARRAY_SIZE; i++)
    {
        vec1[i] = rand() % 100;
        vec2[i] = rand() % 100;
    }

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
            for (j = i * 200; j < ((i + 1) * 200); j++)
            {
                tmp += vec1[j] + vec2[j];
            }
            printf("Soma do processo %d: %d\n", i, tmp);
            close(fd[0]);
            write(fd[1], &tmp, sizeof(tmp));
            exit(i);
        }
    }
    for (i = 0; i < CHILDS; i++)
    {
        read(fd[0], &input, sizeof(input));
        sum += input;
        waitpid(pids[i], &status, 0);
    }

    printf("Soma Total: %d\n", sum);
    return 0;
}