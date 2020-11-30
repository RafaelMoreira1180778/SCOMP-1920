#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ARRAY_SIZE 10

typedef struct
{
    int a[ARRAY_SIZE];
} shared_data_type;

int generateRandomNumber(int a)
{
    srand(time(NULL) + a);
    return rand() % 20 + 1;
}

int writer()
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

    printf("WRITER:\n");

    for (i = 0; i < ARRAY_SIZE; i++)
    {
        shared_data->a[i] = generateRandomNumber(i);
        printf("GENERATED: %d\n", shared_data->a[i]);
    }

    return 0;
}

int reader()
{
    int i, fd, data_size = sizeof(shared_data_type);
    shared_data_type *shared_data;

    if ((fd = shm_open("/shmtest", O_RDWR, S_IRUSR | S_IWUSR)) == -1)
    {
        return fd;
    }

    ftruncate(fd, data_size); //Não necessário porque já foi alocada no writer

    if ((shared_data = (shared_data_type *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == NULL)
    {
        return -1;
    }

    int sum = 0;

    printf("-----------------------------\n");
    printf("READER:\n");

    for (i = 0; i < ARRAY_SIZE; i++)
    {
        sum += shared_data->a[i];
    }

    printf("AVERAGE = %.1f\n", (double)((double)sum / (double)ARRAY_SIZE));

    return 0;
}

int main()
{
    int x, y, error = -1;

    x = writer();
    y = reader();

    if (x == error || y == error)
    {
        return error;
    }

    return 0;
}