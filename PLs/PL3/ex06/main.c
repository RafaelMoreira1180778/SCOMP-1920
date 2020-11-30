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

#define ARRAY_SIZE 1000000
#define CHILDS 2

typedef struct
{
    int a[ARRAY_SIZE];
} shared_data_type;

int generateRandomNumber(int a)
{
    srand(time(NULL) + a);
    return rand() % 50;
}

int main()
{
    int fd[CHILDS];
    int i, t;
    pid_t p1, p2;
    clock_t p_inicio, p_fim, m_inicio, m_fim;

    shared_data_type *struct1, *struct2;
    struct1 = malloc(1 * sizeof(*struct1));
    struct2 = malloc(1 * sizeof(*struct2));

    for (i = 0; i < ARRAY_SIZE; i++)
    {
        struct1->a[i] = generateRandomNumber(i);
    }

    if (pipe(fd) == -1)
    {
        printf("Pipe Error");
        exit(-1);
    }

    int fd_m, data_size = sizeof(int);
    int *shared_data;

    shm_unlink("/shmtest");

    if ((fd_m = shm_open("/shmtest", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR)) == -1)
    {
        printf("SHM Error\n");
        exit(-2);
    }

    if (ftruncate(fd_m, data_size) == -1)
    {
        printf("Truncate Error\n");
        exit(-3);
    }
    if ((shared_data = (shared_data_type *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_m, 0)) == NULL)
    {
        printf("Map Error\n");
        exit(-4);
    }

    if ((p1 = fork()) == -1)
    {
        printf("p1 fork failed\n");
        return p1;
    }

    if (p1 == 0) //processo filho
    {
        close(fd[1]);
        for (i = 0; i < ARRAY_SIZE; i++)
        {
            if (read(fd[0], &t, sizeof(int)) == -1)
            {
                printf("reading error\n");
            }
            struct2->a[i] = t;
        }
        close(fd[0]);

        exit(0);
    }
    else if (p1 > 0) //processo pai
    {
        p_inicio = clock();

        close(fd[0]);
        for (i = 0; i < ARRAY_SIZE; i++)
        {
            if (write(fd[1], &struct1->a[i], sizeof(struct1->a[i])) == -1)
            {
                printf("writing error\n");
            }
        }
        close(fd[1]);

        p_fim = clock();
        printf("USING PIPES IT TOOK %f SECONDS TO COPY THE ARRAY\n", (double)(p_fim - p_inicio));

        wait(0);
    }

    p2 = fork();

    if (p2 == 0) //processo filho
    {
        for (i = 0; i < ARRAY_SIZE; i++)
        {
            struct2->a[i] = *(shared_data);
        }
        exit(0);
    }
    else if (p2 > 0) //processo pai
    {
        m_inicio = clock();

        for (i = 0; i < ARRAY_SIZE; i++)
        {
            *(shared_data) = struct1->a[i];
        }

        m_fim = clock();
        printf("USING SHARED MEMORY IT TOOK %f SECONDS TO COPY THE ARRAY\n", (double)(m_fim - m_inicio));
        wait(0);
    }

    munmap(shared_data, data_size);
    shm_unlink("/shmtest");
    return 0;
}