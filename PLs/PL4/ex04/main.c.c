/*  Exemplo de semaforo binario.
 *
 *  Utilizacao de semaforo binario para controlar acessos a memoria partilhada
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <semaphore.h>

int main(int argc, char *argv[])
{
    int *shm_counter = NULL; /* apontador para a memória partilhada (um inteiro) */
    int shmfd;               /* descritor da memória partilhada */
    int i, r;
    sem_t *semaforo; /* ap para o semaforo */
    pid_t pid;

    /* criar o objeto de memoria partilhada */
    shmfd = shm_open("/hsp_shm", O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
    if (shmfd < 0)
    {
        perror("No shm_open()");
        exit(1);
    }

    /* ajustar o tamanho da mem. partilhada */
    ftruncate(shmfd, sizeof(int));

    /* mapear a mem. partilhada */
    shm_counter = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    if (shm_counter == NULL)
    {
        perror("No mmap()");
        exit(1);
    }

    /* criar o semáforo com valor = 1 */
    if ((semaforo = sem_open("semaforo", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED)
    {
        perror("No sem_open()");
        exit(1);
    }

    *shm_counter = 0; /* iniciar a zero (na realidade, o mmap() já o faz) */

    pid = fork(); /* fork */

    if (pid == 0)
    { /* filho */
        sem_wait(semaforo);
        for (i = 0; i < 500000; i++)
            (*shm_counter)++;
        sem_post(semaforo);
        exit(0);
    }

    /* pai */
    sem_wait(semaforo);
    for (i = 0; i < 500000; i++)
        (*shm_counter)++;
    sem_post(semaforo);

    wait(NULL); /* esperar que o filho termine */
    printf("Contador: %d\n", *shm_counter);

    /* fechar/apagar o semaforo */
    if (sem_close(semaforo) < 0)
    {
        perror("sem_close()");
        exit(EXIT_FAILURE);
    }

    if (sem_unlink("semaforo") < 0)
    {
        perror("sem_unlink()");
        exit(EXIT_FAILURE);
    }

    /* desfaz mapeamento */
    r = munmap(shm_counter, sizeof(int));
    if (r < 0)
    { /* verifica erro */
        perror("No munmap()");
        exit(1);
    }

    /* apaga a memoria partilhada do sistema */
    r = shm_unlink("//hsp_shm");
    if (r < 0)
    { /* verifica erro */
        perror("No unlink()");
        exit(1);
    }

    return 0;
}