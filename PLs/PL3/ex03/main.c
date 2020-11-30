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

#define ARRAY_SIZE 100000
#define CHILDS 2

typedef struct
{
    char a[20];
    int b;
} data;

int generateRandomNumber(int a)
{
    srand(time(NULL) + a);
    return rand() % 50;
}

int main()
{
    int p[2], fd, i;
    data array[ARRAY_SIZE];
    data *arrayP;
    clock_t start, end;

    for (i = 0; i < ARRAY_SIZE; i++)
    {
        strcpy(array[i].a, "ISEP - SCOMP 2020");
        array[i].b = generateRandomNumber(i);
    }

    if ((fd = shm_open("/shmHSP", O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
        perror("Failed to create shared memory");

    if (ftruncate(fd, sizeof(array)) < 0)
        perror("Failed to adjust memory size");

    if ((arrayP = mmap(NULL, sizeof(array), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == NULL)
    {
        perror("No map()");
        exit(1);
    }

    for (i = 0; i < ARRAY_SIZE; i++)
    {
        *(arrayP + i) = array[i];
    }

    start = clock();

    if (fork() == 0)
    {
        for (i = 0; i < ARRAY_SIZE; i++)
        {
            array[i] = *(arrayP + i);
        }
        exit(0);
    }

    wait(0);
    end = clock();
    printf("Using shared memory took %f seconds.\n", ((double)(end - start)) / CLOCKS_PER_SEC);

    if (munmap(arrayP, ARRAY_SIZE * sizeof(data)) < 0)
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

    if (pipe(p) == -1)
    {
        perror("Pipe failed");
        return -1;
    }

    if (fork() == 0)
    {
        close(p[0]);
        write(p[1], array, sizeof(array));
        close(p[1]);
        exit(0);
    }

    data tmp[ARRAY_SIZE];
    read(p[0], tmp, sizeof(tmp));

    end = clock();
    printf("Using pipes took %f secnds.\n", ((double)(end - start)) / CLOCKS_PER_SEC);

    return 0;
}