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

typedef struct
{
    int buffer[BUFFER_SIZE]; //buffer para guardar
    int index;               //indice dentro do buffer
    int f1, f2;              //flags
} estrutura;

int main()
{
    int i, j, fd, data_size = sizeof(estrutura);
    estrutura *shared_data;

    if ((fd = shm_open("/shmEX14", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR)) == -1)
    {
        perror("Erro ao criar memoria partilhada.");
        return 0;
    }

    if (ftruncate(fd, sizeof(int)) == -1)
    {
        perror("Erro ao truncar a memória.");
        return 0;
    }

    shared_data = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_data == MAP_FAILED)
    {
        perror("Erro ao fazer map à memória.");
        return 0;
    }
    if (shared_data == 0)
    {
        perror("Erro ao criar a memória partilhada.");
        return EXIT_FAILURE;
    }

    shared_data->index = -1;
    shared_data->f1 = 0;
    shared_data->f2 = 0;

    for (i = 0; i < BUFFER_SIZE; i++)
    {
        shared_data->buffer[i] = i;
    }

    pid_t p = fork();

    if (p == 0)
    {
        for (i = 0; i < NUM_OPERATIONS; i++)
        {
            while (shared_data->f1 == 0)
            {
                if (shared_data->f1 == 1)
                {
                    break;
                }
            }

            int a = shared_data->index;
            int b = shared_data->buffer[shared_data->index];
            printf("BUFFER[%d] = %d\n", a + 1, b);

            shared_data->f1 = 0;
            shared_data->f2 = 1;
        }
        exit(0);
    }
    else if (p > 0)
    {
        for (j = 0; j < NUM_OPERATIONS; j++)
        {
            if ((shared_data->index) == 9)
            {
                shared_data->index = 0;
            }
            else
            {
                (shared_data->index)++;
            }

            (shared_data->buffer[shared_data->index])++;
            shared_data->f1 = 1;

            while (shared_data->f2 == 0)
            {
                if (shared_data->f2 == 1)
                {
                    break;
                }
            }
            shared_data->f2 = 0;
        }
        waitpid(p, NULL, 0);
    }

    if (munmap(shared_data, data_size) < 0)
    {
        perror("No munmap()");
        exit(1);
    }

    if (shm_unlink("/shmEX14") < 0)
    {
        perror("No unlink()");
        exit(1);
    }

    if (close(fd) < 0)
    {
        perror("No close()");
        exit(1);
    }

    return 0;
}