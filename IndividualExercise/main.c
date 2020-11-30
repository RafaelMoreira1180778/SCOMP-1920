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
#include <limits.h>

#define SHM_NAME "/shm_2de_1180778"
#define CHILDS 5
#define NUM_SEMAPHORES 2

typedef struct
{
    char nome_processo[3]; //nome de cada processo
    int duracao;           //duracao de cada processo, simula o tempo de algoritmo
} shared_data_type;

/*
* MÉTODO FORNECIDO PELOS DOCENTES NO ENUNCIADO DO EXERCÍCIO INDIVIDUAL
*/
int generate_random_number(int a)
{
    time_t t;                      /* to init. the random number generator (RNG)*/
    srand((unsigned)time(&t) + a); //adição de um random inteiro para que sejam gerados números diferentes a cada vez que se chama o método
    return rand() % 11;            /* inteiro de 0 a 999 */
}

int initiate_semaphores(sem_t *semaforos[NUM_SEMAPHORES], char nomes_semaforos[][25], int valores_iniciais_semaforos[])
{
    int i;
    for (i = 0; i < NUM_SEMAPHORES; i++)
    {
        if ((semaforos[i] = sem_open(nomes_semaforos[i], O_CREAT | O_EXCL, 0644, valores_iniciais_semaforos[i])) == SEM_FAILED)
        {
            perror("No sem_open()");
            return 1;
        }
    }
    return 0;
}

int main()
{
    sem_t *semaforos[NUM_SEMAPHORES];
    char nomes_semaforos[][25] = {"/sem_pai_2de_1180778", "/sem_filho_2de_1180778"}; //evita espera ativa
    int valores_iniciais_semaforos[] = {0, 1};

    int fd, data_size = sizeof(shared_data_type);
    shared_data_type *shared_data;

    if ((fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR)) == -1)
    {
        perror("shm_open() error\n");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, data_size) == -1)
    {
        perror("ftruncate() error\n");
        exit(EXIT_FAILURE);
    }

    if ((shared_data = (shared_data_type *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
    {
        perror("mmap() error\n");
        exit(EXIT_FAILURE);
    }

    if (initiate_semaphores(semaforos, nomes_semaforos, valores_iniciais_semaforos) == EXIT_FAILURE)
    {
        perror("sem_open() error\n");
        exit(EXIT_FAILURE);
    }

    pid_t p[CHILDS];
    int i, status;

    for (i = 0; i < CHILDS; i++)
    {
        p[i] = fork();
        if (p[i] == -1)
        {
            perror("fork() error\n");
            exit(EXIT_FAILURE);
        }
        if (p[i] == 0) //processo filho
        {
            sem_wait(semaforos[1]); //sem_filho

            int tempo = generate_random_number(i + 1);
            sleep(tempo); //simula o tempo do algoritmo parado
            shared_data->duracao = tempo;
            sprintf(shared_data->nome_processo, "A%d", i);
            printf("\nAlgoritmo Terminado - os dados foram inseridos na regiao de memoria partilhada...\n");

            sem_post(semaforos[0]); //sem_pai
            exit(0);
        }
    }

    //processo pai

    int menorDuracao = INT_MAX;
    char menorNome[3]; //evita o problema do sprintf em que uma string tem de ter no min 3 bytes

    for (i = 0; i < CHILDS; i++)
    {
        sem_wait(semaforos[0]); //sem_pai
        printf("O algoritmo %s demorou %d segundos a executar...\n", shared_data->nome_processo, shared_data->duracao);
        if (shared_data->duracao < menorDuracao)
        {
            menorDuracao = shared_data->duracao;
            sprintf(menorNome, "%s", shared_data->nome_processo);
        }
        sem_post(semaforos[1]); //sem_ filho
    }

    for (i = 0; i < CHILDS; i++)
    {
        waitpid(p[i], &status, 0); //espera que todos os filhos terminem de executar
    }

    printf("\nMelhor tempo %d para o algoritmo %s!\n", menorDuracao, menorNome); //imprime apenas quando todos os filhos terminam

    if (munmap(shared_data, data_size) < 0 || shm_unlink(SHM_NAME) < 0)
    {
        exit(1);
    }

    sem_close(semaforos[0]);
    sem_close(semaforos[1]);
    sem_unlink(nomes_semaforos[0]);
    sem_unlink(nomes_semaforos[1]);
}