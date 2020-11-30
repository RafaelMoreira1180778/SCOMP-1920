#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CHILDS 5
#define PIPES CHILDS + 1

int biggestInt(int a, int b)
{
    return (a > b) ? a : b;
}

int generateRandomNumber(int a)
{
    srand(time(NULL) + a);
    return rand() % 499 + 1;
}

void initiatePipes(int fd[PIPES][2])
{
    int i;
    for (i = 0; i < PIPES; i++)
    {
        if (pipe(fd[i]) == -1)
        {
            perror("Broken Pipe");
        }
    }
}

int main()
{
    int c, i, m, n;
    int fd[PIPES][2];
    pid_t p;

    initiatePipes(fd);

    for (i = 0; i < CHILDS; i++)
    {
        p = fork();
        if (p == 0)
        {
            c = generateRandomNumber(getpid());
            printf("(CHILD%d) PID: %d ---- NUM: %d\n", i + 1, getpid(), c);

            // i = 0 le fd[0][0]
            // i = 1 le fd[1][0]
            read(fd[i][0], &n, sizeof(int));
            m = biggestInt(c, n);

            // i = 0 escreve fd[0+1][1]
            // i = 1 escreve fd[2][1]
            write(fd[i + 1][1], &m, sizeof(m));
            close(fd[i + 1][1]);

            exit(0);
        }
    }

    m = generateRandomNumber(getpid());
    printf("(PARENT) PID: %d ---- NUM: %d\n", getpid(), m);
    write(fd[0][1], &m, sizeof(m));
    close(fd[0][1]);

    read(fd[CHILDS][0], &c, sizeof(int));
    printf("BIGGEST NUMBER: %d\n", c);

    return 0;
}
