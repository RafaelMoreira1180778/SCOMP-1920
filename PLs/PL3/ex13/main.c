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

#define shm_name "/shmEX13"
#define CHILDS 10

typedef struct
{
    int numOcurrences[10];
    char wordToFind[10][255];
    char pathToFile[10][100];
} shared_memory;

int main()
{
    int i, fd, data_size = sizeof(shared_memory);
    pid_t p;
    FILE *f;
    char line[1024];
    shared_memory *shared_data;

    if ((fd = shm_open(shm_name, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
        perror("Failed to create shared memory");

    if (ftruncate(fd, data_size) < 0)
        perror("Failed to adjust memory size");

    if ((shared_data = mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == NULL)
    {
        perror("No map()");
        exit(1);
    }

    strcpy(shared_data->wordToFind[0], "um");
    strcpy(shared_data->wordToFind[1], "dois");
    strcpy(shared_data->wordToFind[2], "tres");
    strcpy(shared_data->wordToFind[3], "quatro");
    strcpy(shared_data->wordToFind[4], "cinco");
    strcpy(shared_data->wordToFind[5], "seis");
    strcpy(shared_data->wordToFind[6], "sete");
    strcpy(shared_data->wordToFind[7], "oito");
    strcpy(shared_data->wordToFind[8], "nove");
    strcpy(shared_data->wordToFind[9], "dez");

    for (i = 0; i < CHILDS; i++)
    {
        sprintf(shared_data->pathToFile[i], "child (%d).txt", i);

        p = fork();
        if (p == -1)
        {
            perror("Fork error\n");
            exit(EXIT_FAILURE);
        }
        if (p == 0)
        {
            f = fopen(shared_data->pathToFile[i], "r");
            while (fgets(line, sizeof(line), f) != NULL)
            {
                if (strstr(line, shared_data->wordToFind[i]) != NULL)
                {
                    shared_data->numOcurrences[i]++;
                }
            }
            fclose(f);
            exit(0);
        }
    }

    wait(NULL);

    for (i = 0; i < 10; i++)
    {
        printf("CHILD %d FOUND THE WORD '%s' %d TIMES IN THE FILE %s\n", i, shared_data->wordToFind[i], shared_data->numOcurrences[i], shared_data->pathToFile[i]);
    }

    if (munmap(shared_data, data_size) < 0)
    {
        perror("No munmap()");
        exit(1);
    }

    if (shm_unlink(shm_name) < 0)
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