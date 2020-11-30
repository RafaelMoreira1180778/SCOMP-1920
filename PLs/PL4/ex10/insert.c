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
    int i, fd, data_size = sizeof(shared_data_type);
    shared_data_type *shared_data;
    sem_t *sems[100];
    char *nameSems[100];
    User user;

    fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
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
        sems[i] = sem_open(nameSems[i], O_CREAT | O_RDWR, 0644, 1);
    }

    sem_wait(sems[shared_data->next]);

    write(1, "Name:\n", strlen("Name:\n"));
    scanf("%s", user.name);
    write(1, "Number:\n", strlen("Number:\n"));
    scanf("%d", &user.number);
    write(1, "Address:\n", strlen("Address:\n"));
    scanf("%s", user.address);

    if (shared_data->next == 100)
    {
        write(1, "Out of space for that record...\n", strlen("Out of space for that record...\n"));
    }

    shared_data->records[shared_data->next] = user;
    sem_post(sems[shared_data->next]);

    shared_data->next++;
    sem_post(sems[shared_data->next]);

    if (munmap(shared_data, data_size) == -1)
    {
        perror("Munmap error\n");
        exit(EXIT_FAILURE);
    }

    close(fd);

    return 0;
}
