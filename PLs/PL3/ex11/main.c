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

#define ARRAY_SIZE 1000
#define CHILDS 10
#define PIPES CHILDS + 1

int generateRandomNumber(int a)
{
    srand(time(NULL) + a);
    return rand() % 1000;
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

typedef struct
{
    int global[ARRAY_SIZE];
    int local[CHILDS];
} shared_data_type;

int main()
{
    int fd, data_size = sizeof(shared_data_type);
    shared_data_type *shared_data;

    shm_unlink("/shmtest"); //libertar a memoria partilhada
    if ((fd = shm_open("/shmtest", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR)) == -1)
    {
        return fd;
    }

    ftruncate(fd, data_size);

    if ((shared_data = (shared_data_type *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == NULL)
    {
        return -1;
    }

    int i, j, pid, p[PIPES][2], local_maximum = 0;

    initiatePipes(p);

    for (i = 0; i < CHILDS; i++)
    {
        pid = fork();
        if (pid == 0) //processo filho
        {
            int tmp = 0;

            read(p[i][0], &local_maximum, sizeof(int));
            close(p[i][0]);

            for (j = 100 * i; j < (100 * i) + 99; j++)
            {
                if (shared_data->global[j] > tmp)
                {
                    tmp = shared_data->global[j];
                }
            }

            if (tmp > local_maximum)
            {
                write(p[i + 1][1], &tmp, sizeof(tmp));
            }

            write(p[i + 1][1], &local_maximum, sizeof(local_maximum));
            close(p[i + 1][1]);

            exit(i);
        }
    }

    for (i = 0; i < ARRAY_SIZE; i++)
    {
        shared_data->global[i] = generateRandomNumber(i);
    }

    int global_maximum = 0;

    write(p[0][1], &global_maximum, sizeof(global_maximum));
    close(p[0][1]);

    read(p[CHILDS - 1][0], &global_maximum, sizeof(global_maximum));
    close(p[CHILDS - 1][0]);
    printf("GLOBAL MAXIMUM = %d\n", global_maximum);

    shm_unlink("/shmtest"); //libertar a memoria partilhada

    return 0;
}