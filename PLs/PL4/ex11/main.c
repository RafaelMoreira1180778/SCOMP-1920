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

#define shm_name "/shm_ex11pl04"
#define NUM_SEMAPHORES 4
#define CHILDS 10
#define NUM_PASSENGERS 200

typedef struct
{
    int capacity;
    int passangers;
} train;

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

void initiate_semaphores(sem_t *sem[NUM_SEMAPHORES], char sems_names[][25], int sems_initial_values[])
{
    int i;
    for (i = 0; i < NUM_SEMAPHORES; i++)
    {
        if ((sem[i] = sem_open(sems_names[i], O_CREAT | O_EXCL, 0644, sems_initial_values[i])) == SEM_FAILED)
        {
            perror("No sem_open()");
            exit(1);
        }
    }
}

void get_in(train *train)
{
    train->passangers++;
    printf("Got in the train :) | PID: %d)\n", getpid());
}

void get_out(train *train)
{
    train->passangers--;
    printf("Left the train :( | PID: %d)\n", getpid());
}

int main()
{
    int i, bool;
    sem_t *sem[NUM_SEMAPHORES];

    char sems_names[][25] = {"/shm_ex11pl04", "/ex11_d1", "/ex11_d2", "/ex11_d3"};
    int sems_initial_values[] = {1, 1, 1, 1};

    enum index
    {
        SHM,
        DOOR1,
        DOOR2,
        DOOR3
    };

    int fd, data_size = sizeof(train);
    train *shared_data;

    // Inicializar memória partilhada
    fd = shm_open(shm_name, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("Erro ao abrir a memoria!\n");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, data_size) == -1)
    {
        perror("Erro a definir o tamanho!\n");
        exit(EXIT_FAILURE);
    }

    shared_data = (train *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_data == MAP_FAILED)
    {
        perror("Erro no mmap!\n");
        exit(EXIT_FAILURE);
    }

    shared_data->capacity = NUM_PASSENGERS;
    shared_data->passangers = 0;

    initiate_semaphores(sem, sems_names, sems_initial_values);
    int id = initiate_childs(CHILDS);

    //Verificar se o comboio está cheio
    bool = 0;
    while (!bool)
    {
        sem_wait(sem[SHM]);
        if (shared_data->passangers == shared_data->capacity)
        {
            sem_post(sem[SHM]);
            printf("The train is full...");
            sleep(2);
        }
        else
        {
            bool = 1;
        }
    }

    //Entrar no comboio
    bool = 0;
    while (!bool)
    {
        if (sem_trywait(sem[DOOR1]) && bool == 0)
        {
            bool = 1;
            get_in(shared_data);
            sem_post(sem[DOOR1]);
            printf("Actual capacity (door1): %d\n", shared_data->passangers);
        }
        if (sem_trywait(sem[DOOR2]) && bool == 0)
        {
            bool = 1;
            get_in(shared_data);
            sem_post(sem[DOOR2]);
            printf("Actual capacity (door2): %d\n", shared_data->passangers);
        }
        if (sem_trywait(sem[DOOR3]) && bool == 0)
        {
            bool = 1;
            get_in(shared_data);
            sem_post(sem[DOOR3]);
            printf("Actual capacity (door3): %d\n", shared_data->passangers);
        }
    }
    sem_post(sem[SHM]);

    //Sair do comboio
    sem_wait(sem[SHM]);
    bool = 0;
    while (!bool)
    {
        if (sem_trywait(sem[DOOR1]) && bool == 0)
        {
            bool = 1;
            get_out(shared_data);
            sem_post(sem[DOOR1]);
        }
        if (sem_trywait(sem[DOOR2]) && bool == 0)
        {
            bool = 1;
            get_out(shared_data);
            sem_post(sem[DOOR2]);
        }
        if (sem_trywait(sem[DOOR3]) && bool == 0)
        {
            bool = 1;
            get_out(shared_data);
            sem_post(sem[DOOR3]);
        }
    }
    sem_post(sem[SHM]);

    if (id > 0)
    {
        printf("Going home... | PID: %d)\n", getpid());
        exit(1);
    }

    for (i = 0; i < CHILDS; i++)
    {
        wait(NULL); //espera por qualquer um
    }

    if (munmap(shared_data, 100) < 0)
    {
        exit(1);
    }
    if (shm_unlink(shm_name) < 0)
    {
        exit(1);
    }

    sem_close(sem[SHM]);
    sem_close(sem[DOOR1]);
    sem_close(sem[DOOR2]);
    sem_close(sem[DOOR3]);

    sem_unlink(sems_names[SHM]);
    sem_unlink(sems_names[DOOR1]);
    sem_unlink(sems_names[DOOR2]);
    sem_unlink(sems_names[DOOR3]);

    return 0;
}