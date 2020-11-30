#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#define CHILDS 5

int biggestInt(int a, int b)
{
    return (a > b) ? a : b;
}

int generateRandomNumber(int a)
{
    srand(time(NULL) + a);
    return rand() % 499 + 1;
}

void initiatePipes()
{
}

int main()
{
    int p1[2], p2[2], pids[CHILDS];
    int i, b, c, m, status;

    if (pipe(p1) == -1)
    {
        fprintf(stderr, "Pipe1 Failed");
        return -1;
    }

    for (i = 0; i < CHILDS; i++)
    {

        if (pipe(p2) == -1)
        {
            fprintf(stderr, "Pipe1 Failed");
            return -1;
        }

        pids[i] = fork();
        if (pids[i] == -1)
        {
            printf("Fork Failed\n");
            exit(-1);
        }
        else if (pids[i] == 0)
        {
            close(p1[1]);
            close(p2[0]);

            b = generateRandomNumber(i);

            read(p1[0], &c, sizeof(int));
            m = biggestInt(b, c);

            printf("PID: %d ---- i = %d ---- B = %d ---- C = %d\n", getpid(), i, b, c);

            write(p2[1], &m, sizeof(m));

            close(p1[0]);
            close(p2[1]);
            exit(m);
        }
        close(p1[0]);
        close(p1[1]);
        p1[0] = p2[0];
        p1[1] = p2[1];
    }

    b = generateRandomNumber(50);
    printf("PID: %d ---- NUM: %d ---- PARENT\n", getpid(), b);
    write(p1[1], &b, sizeof(b));
    close(p1[1]);
    close(p2[1]);
    read(p2[0], &m, sizeof(int));
    close(p2[0]);
    printf("BIGGEST NUMBER: %d\n", m);

    return 0;
}
