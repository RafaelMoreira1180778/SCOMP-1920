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

#define ARRAY_SIZE 100

typedef struct
{
    char a[ARRAY_SIZE];
} shared_data_type;

//Retirado de: https://codereview.stackexchange.com/questions/29198/random-string-generator-in-c
char *generateRandomName(size_t length, int a)
{
    srand(time(NULL) + a);

    static char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char *randomString = NULL;

    if (length)
    {
        randomString = malloc(sizeof(char) * (length + 1));

        if (randomString)
        {
            for (int n = 0; n < length; n++)
            {
                int key = rand() % (int)(sizeof(charset) - 1);
                randomString[n] = charset[key];
            }

            randomString[length] = '\0';
        }
    }

    return randomString;
}

int writer()
{
    int i, fd, data_size = sizeof(shared_data_type);
    shared_data_type *shared_data;

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
        shared_data->a[i] = *(generateRandomName(1, i));
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

    printf("-----------------------------\n");
    printf("READER:\n");

    int sum = 0;
    char c;

    for (i = 0; i < ARRAY_SIZE; i++)
    {
        c = shared_data->a[i];
        printf("CHAR[%d]: %c | ASCII = %d\n", i, c, c);
        sum += c;
    }

    printf("SUM = %d\n", sum);
    printf("AVERAGE = %d\n", (sum / ARRAY_SIZE));

    return 0;
}

int main()
{
    int x, y, error = -1;

    shm_unlink("/shmtest"); //libertar a memoria partilhada

    x = writer();
    y = reader();

    if (x == error || y == error)
    {
        return error;
    }

    shm_unlink("/shmtest"); //libertar a memoria partilhada

    return 0;
}