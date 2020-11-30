#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CHILDS 1

int generateRandomNumber(int a)
{
    srand(time(NULL) + a);
    return rand() % 5 + 1;
}

int main()
{
    pid_t pid;
    int credits = 20;
    int g, c, m;

    while (credits >= 5)
    {
        int fd1[2], fd2[2];
        if (pipe(fd1) == -1)
        {
            fprintf(stderr, "Pipe1 Failed");
            return 1;
        }

        if (pipe(fd2) == -1)
        {
            fprintf(stderr, "Pipe2 Failed");
            return 1;
        }

        pid = fork();
        if (pid == -1)
        {
            exit(-1);
        }

        if (pid == 0)
        {
            close(fd1[1]);
            close(fd2[0]);

            int creditsChild, b;

            read(fd1[0], &g, sizeof(int));

            if (g == 1)
            {
                b = generateRandomNumber(getpid());
                write(fd2[1], &b, sizeof(b));
            }

            read(fd1[0], &creditsChild, sizeof(int));
            printf("Updated Credits: %d.\n", creditsChild);

            close(fd1[0]);
            close(fd2[1]);
            exit(0);
        }
        else if (pid > 0)
        {
            close(fd1[0]);
            close(fd2[1]);

            g = generateRandomNumber(getpid());

            if (credits > 0)
            {
                m = 1;
            }
            else
            {
                m = 0;
            }

            write(fd1[1], &m, sizeof(m));

            read(fd2[0], &c, sizeof(int));

            if (c == g)
            {
                printf("The bet was correct: %d == %d.\n", c, g);
                credits += 10;
            }
            else
            {
                printf("The bet was not correct: %d != %d.\n", c, g);
                credits -= 5;
            }

            write(fd1[1], &credits, sizeof(credits));
            wait(NULL);

            close(fd1[1]);
            close(fd2[0]);
        }
    }

    printf("Out of credits.\n");

    return 0;
}