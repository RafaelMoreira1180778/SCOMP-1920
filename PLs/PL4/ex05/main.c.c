#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <semaphore.h>

int main() {
	/**
	 * Criar o semaforo com o valor 0.
	 */
	sem_t *semaforo;
	if ((semaforo = sem_open("sem_pl4_ex5", O_CREAT | O_CREAT, 0644, 0))== SEM_FAILED) {
		perror("No sem_open()");
		exit(0);
	}

	/**
	 * Sendo filho, imprime e adiciona 1 ao semáforo.
	 * O pai espera que o filho termine através do
	 * semáforo.
	 */
	if (fork() == 0) {
		printf("I'm the child.\n");
		sem_post(semaforo);
		exit(0);
	}
	else{
		sem_wait(semaforo);
		printf("I'm the father.\n");
		sem_post(semaforo);
	}

	if (sem_close(semaforo) < 0) {
		perror("No unlink()");
		exit(0);
	}

	if (sem_unlink("sem_pl4_ex5") < 0) {
		perror("No unlink()");
		exit(0);
	}
}
