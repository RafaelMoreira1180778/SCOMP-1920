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

#define OPERATION_SIZE 1000000

typedef struct
{
    int a;
} shared_data_type;

int main()
{
    int i, fd, data_size = sizeof(shared_data_type);
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

    pid_t p;

    shared_data->a = 100;

    if ((p = fork()) == -1)
    {
        perror("PIPE ERROR\n");
        exit(0);
    }

    if (p == 0) //processo filho
    {
        for (i = 0; i < OPERATION_SIZE; i++)
        {
            ++shared_data->a;
        }
        for (i = 0; i < OPERATION_SIZE; i++)
        {
            --shared_data->a;
        }
        exit(0);
    }
    else if (p > 0) //processo pai
    {
        for (i = 0; i < OPERATION_SIZE; i++)
        {
            ++shared_data->a;
        }
        for (i = 0; i < OPERATION_SIZE; i++)
        {
            --shared_data->a;
        }
    }

    wait(0);
    printf("A = %d\n", shared_data->a);
    printf("These results are not always correct because the program runs and both processes are altering the same variable present in the shared memory at the same time.\n");
    return 0;
}