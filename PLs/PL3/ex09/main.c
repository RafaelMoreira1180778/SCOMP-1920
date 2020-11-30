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

int generateRandomNumber(int a)
{
    srand(time(NULL) + a);
    return rand() % 1000;
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

    int i, j, status, p[i];

    for (i = 0; i < ARRAY_SIZE; i++)
    {
        shared_data->global[i] = generateRandomNumber(i);
        printf("generated = %d\n", shared_data->global[i]);
    }

    for (i = 0; i < CHILDS; i++)
    {
        int local_maximum = 0;
        if ((p[i] = fork()) == -1)
        {
            printf("Error in pipe number %d", i);
            exit(i);
        }
        if (p[i] == 0) //processo filho
        {
            for (j = 100 * i; j < (100 * i) + 99; j++)
            {
                if (shared_data->global[j] > local_maximum)
                {
                    local_maximum = shared_data->global[j];
                }
            }
            shared_data->local[i] = local_maximum;
            exit(i);
        }
    }

    int global_max = 0;

    for (i = 0; i < CHILDS; i++)
    {
        waitpid(p[i], &status, 0);
        if (shared_data->local[i] > global_max)
        {
            global_max = shared_data->local[i];
        }
    }

    printf("GLOBAL MAXIMUM = %d\n", global_max);

    return 0;
}