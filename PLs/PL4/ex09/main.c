#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>

#define SHM_NAME "/shm_pl04ex09"
#define SEMS 3
#define CHILDS 1

typedef struct
{
    int chips;
    int beer;
} shared_data_type;

int initiate_childs()
{
    pid_t p[CHILDS];
    int i;

    for (i = 0; i < CHILDS; i++)
    {
        p[i] = fork();
        if (p[i] == -1)
        {
            perror("Erro no fork!\n");
            exit(EXIT_FAILURE);
        }

        if (p[i] == 0)
        {
            return i + 1;
        }
    }
    return 0;
}

void initiate_semaphores(sem_t *sem[SEMS], char sems_names[][25], int sems_initial_values[])
{
    int i;
    for (i = 0; i < SEMS; i++)
    {
        if ((sem[i] = sem_open(sems_names[i], O_CREAT | O_EXCL, 0644, sems_initial_values[i])) == SEM_FAILED)
        {
            perror("No sem_open()");
            exit(1);
        }
    }
}

void buy_chips(shared_data_type *shared_data)
{
    shared_data->chips++;
    shared_data->chips++;
    printf("Bought 2 chips. | PID: %d\n", getpid());
}

void buy_beer(shared_data_type *shared_data)
{
    shared_data->beer++;
    shared_data->beer++;
    printf("Bought 2 beers. | PID: %d\n", getpid());
}

void eat_and_drink(shared_data_type *shared_data)
{
    shared_data->beer--;
    shared_data->chips--;
    printf("Ate a potato and drank a beer. | PID: %d\n", getpid());
}

int main()
{
    sem_t *sem[SEMS];
    char sems_names[][25] = {"/sms_ex09", "/sms_chips", "/sms_beer"};
    int sems_initial_values[] = {1, 0, 0};

    enum index
    {
        SHM,
        CHIPS,
        BEER
    };

    int fd, data_size = sizeof(shared_data_type);
    shared_data_type *shared_data;

    fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("Error opnening shared memory area.\n");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, data_size) == -1)
    {
        perror("Error allocating space ftruncate.\n");
        exit(EXIT_FAILURE);
    }

    shared_data = (shared_data_type *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_data == MAP_FAILED)
    {
        perror("Map error.\n");
        exit(EXIT_FAILURE);
    }

    initiate_semaphores(sem, sems_names, sems_initial_values);
    int id = initiate_childs(CHILDS);

    if (id == 0) //processo pai
    {
        sem_wait(sem[SHM]);
        buy_chips(shared_data);
        sem_post(sem[SHM]);
        sem_post(sem[CHIPS]);

        sem_wait(sem[BEER]);
        sem_wait(sem[CHIPS]);
        sem_wait(sem[SHM]);
        eat_and_drink(shared_data);

        sem_post(sem[SHM]);
        sem_post(sem[CHIPS]);
        sem_post(sem[BEER]);
    }

    if (id > 0) //processo filho
    {
        sem_wait(sem[SHM]);
        buy_beer(shared_data);
        sem_post(sem[SHM]);
        sem_post(sem[BEER]);

        sem_wait(sem[BEER]);
        sem_wait(sem[CHIPS]);
        sem_wait(sem[SHM]);
        eat_and_drink(shared_data);

        sem_post(sem[SHM]);
        sem_post(sem[CHIPS]);
        sem_post(sem[BEER]);
    }

    // --------------------------------------------------------------------------------------------------------------------

    if (munmap(shared_data, 100) < 0)
    {
        exit(1);
    }

    if (shm_unlink(SHM_NAME) < 0)
    {
        exit(1);
    }

    sem_close(sem[SHM]);
    sem_close(sem[CHIPS]);
    sem_close(sem[BEER]);

    sem_unlink(sems_names[SHM]);
    sem_unlink(sems_names[CHIPS]);
    sem_unlink(sems_names[BEER]);

    return 0;
}