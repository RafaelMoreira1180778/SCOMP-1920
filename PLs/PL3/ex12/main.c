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

#define STR_SIZE 50
#define NR_DISC 10

struct aluno
{
    int numero;
    char nome[STR_SIZE];
    int disciplinas[NR_DISC];
    int check;
};

int main()
{
    int fd, data_size = sizeof(struct aluno);
    struct aluno *shared_data;

    if ((fd = shm_open("/shmHSP", O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
        perror("Failed to create shared memory");

    if (ftruncate(fd, data_size) < 0)
        perror("Failed to adjust memory size");

    if ((shared_data = mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == NULL)
    {
        perror("No map()");
        exit(1);
    }

    int p, i;

    p = fork();

    if (p == 0)
    {
        while (1)
        {
            if (shared_data->check == 1)
            {
                break;
            }
        }

        int t = shared_data->disciplinas[0], l = t, h = t, s = 0;

        for (i = 0; i < NR_DISC; i++)
        {
            t = shared_data->disciplinas[i];
            s += t;
            if (t > h)
            {
                h = t;
            }
            if (t < l)
            {
                l = t;
            }
        }

        printf("\nName: %s\nNumber: %d\n", shared_data->nome, shared_data->numero);
        printf("Highest grade: %d\nLowest grade: %d\nAverage: %d\n", h, l, (s / NR_DISC));

        exit(0);
    }

    printf("Enter name:\n");
    scanf("%s", shared_data->nome);
    printf("Enter number:\n");
    scanf("%d", &shared_data->numero);
    printf("Enter grades:\n");
    for (i = 0; i < NR_DISC; i++)
    {
        printf("%d:", i + 1);
        scanf("%d", &shared_data->disciplinas[i]);
    }
    shared_data->check = 1;

    wait(0);

    if (munmap(shared_data, data_size) < 0)
    {
        perror("No munmap()");
        exit(1);
    }

    if (shm_unlink("/shmHSP") < 0)
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