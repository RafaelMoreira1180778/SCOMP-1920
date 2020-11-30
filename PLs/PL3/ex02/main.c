#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct
{
    char description[20];
    int p_code;
    int price;
} shared_data_type;

int writer()
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

    printf("WRITER:\n");

    printf("PRODUCT CODE: ");
    scanf("%d", &shared_data->p_code);

    printf("DESCRIPTION: ");
    scanf("%s", shared_data->description);

    printf("PRICE: ");
    scanf("%d", &shared_data->price);

    return 0;
}

int reader()
{
    int fd, data_size = sizeof(shared_data_type);
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
    printf("PRODUCT CODE: %d\n", shared_data->p_code);
    printf("DESCRIPTION: %s\n", shared_data->description);
    printf("PRICE: %d\n", shared_data->price);

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