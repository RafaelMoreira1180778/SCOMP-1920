#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <limits.h>

#define BUFFER_SIZE 10
#define NUM_OPERATIONS 30
#define PIPES 3
static int p_read = 0;
static int p_write = 1;

void initiatePipes(int fd[PIPES][2])
{
    int i;
    for (i = 0; i < PIPES; i++)
    {
        if (pipe(fd[i]) == -1)
        {
            perror("Pipe Error\n");
            exit(EXIT_FAILURE);
        }
    }
}

int generateRandomNumber(int a)
{
    srand(time(NULL) + a);
    return rand() % 10;
}

int main()
{
    int fd[PIPES][2];
    int buffer[BUFFER_SIZE];
    pid_t p;
    int i, j;

    initiatePipes(fd);

    p = fork();

    if (p == 0) //processo filho consumer
    {
        for (j = 0; j < PIPES; j++)
        {
            close(fd[j][p_write]);
            read(fd[j][p_read], &buffer, sizeof(buffer));
            close(fd[j][p_read]);
            for (i = 0; i < BUFFER_SIZE; i++)
            {
                printf("Consumer: %d\n", buffer[i]);
            }
        }
    }
    if (p > 0) //processo pai producer
    {
        for (j = 0; j < PIPES; j++)
        {
            for (i = 0; i < BUFFER_SIZE; i++)
            {
                buffer[i] = generateRandomNumber(i);
                printf("Producer: %d\n", buffer[i]);
            }

            close(fd[j][p_read]);
            write(fd[j][p_write], buffer, sizeof(buffer));
            close(fd[j][p_write]);
        }
    }

    wait(NULL);

    return 0;
}