#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>

#define OPERATION_TIME 1000000

typedef struct
{
    int a, b;
} shared_data_type;

int main()
{
    int i, fd, data_size = sizeof(shared_data_type);
    shared_data_type *shared_data;
    pid_t p;

    if ((fd = shm_open("/shmtest", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR)) == -1)
    {
        return fd;
    }

    ftruncate(fd, data_size);

    if ((shared_data = (shared_data_type *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == NULL)
    {
        return -1;
    }

    shared_data->a = 8000;
    shared_data->b = 200;

    if ((p = fork()) == 0) //processo filho
    {
        for (i = 0; i < OPERATION_TIME; i++)
        {
            --shared_data->a;
            ++shared_data->b;
        }
        exit(0);
    }
    else if (p > 0) // processo pai
    {
        for (i = 0; i < OPERATION_TIME; i++)
        {
            ++shared_data->a;
            --shared_data->b;
        }
    }

    wait(0);

    printf("A = %d | B = %d\n", shared_data->a, shared_data->b);

    printf("These results are not always correct because the program runs and both processes are altering 2 variables present in the shared memory at the same time.\n");

    if (close(fd) < 0)
    { /* desfaz mapeamento */
        perror("No close()");
        exit(1);
    }
    if (munmap(shared_data, sizeof(shared_data_type)) < 0)
    { /* desfaz mapeamento */
        perror("No munmap()");
        exit(1);
    }

    if (shm_unlink("/shmtest") < 0)
    { /* apaga a memoria partilhada do sistema */
        perror("No unlink()");
        exit(1);
    }

    return 0;
}