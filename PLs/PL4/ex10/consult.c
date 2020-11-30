#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#define SHM_NAME "/shm_pl04_ex10"
#define SEM_NAME "/sem_pl04_ex10"

typedef struct
{
    int number;
    char name[100];
    char address[150];
} User;

typedef struct
{
    User records[100];
    int next;
} shared_data_type;

int main()
{
    int i, num, fd, data_size = sizeof(shared_data_type);
    shared_data_type *shared_data;
    sem_t *sems[100];
    char *nameSems[100];

    fd = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("Error opening shared memory\n");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, data_size) == -1)
    {
        perror("Error allocating size ftruncate \n");
        exit(EXIT_FAILURE);
    }

    shared_data = (shared_data_type *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_data == MAP_FAILED)
    {
        perror("Mmap error \n");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < 100; i++)
    {
        nameSems[i] = (char *)malloc(100);
        sprintf(nameSems[i], "%s%d", SEM_NAME, i);
        sems[i] = sem_open(nameSems[i], O_RDWR, 0644, 1);
    }

    printf("Number to search:\n");
    scanf("%d%*c", &num);

    for (i = 0; i < shared_data->next; i++)
    {
        if (shared_data->records[i].number == num)
        {
            sem_wait(sems[i]);
            printf("\nNumber: %d\n", shared_data->records[i].number);
            printf("Name: %s\n", shared_data->records[i].name);
            printf("Address: %s\n", shared_data->records[i].address);
            sem_post(sems[i]);
        }
        else if (i == shared_data->next - 1)
        {
            printf("User not found!\n");
        }
    }

    if (munmap(shared_data, data_size) == -1)
    {
        perror("Munmap error\n");
        exit(EXIT_FAILURE);
    }

    close(fd);

    return 0;
}
