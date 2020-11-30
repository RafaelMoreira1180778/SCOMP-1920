#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

#define ARRAY_SIZE 1000
#define CHILDS 5

int main() {

	time_t t;			/* needed to initialize random number generator (RNG) */
	int vec1[ARRAY_SIZE];
	int vec2[ARRAY_SIZE];
	int result[ARRAY_SIZE];
	int fd[CHILDS][2];  // double array para criação de 5 pipes
	int i, status, j, posicao;
	int pids[CHILDS];
	int transferir_res_pos[2];	
	
	/* intializes RNG (srand():stdlib.h; time(): time.h) */
	srand ((unsigned) time (&t));

	/* initialize array with random numbers (rand(): stdlib.h) */
	for (i = 0; i < ARRAY_SIZE; i++){
		vec1[i] = rand () % 100;
		vec2[i] = rand () % 100;
	}
	
	//para testar
	vec1[999] = 70;
	vec2[999] = 80; // pos 1000 = 150
	vec1[499] = 100;
	vec2[499] = 100; // pos 500 = 200
	vec1[33] = 15;
	vec2[33] = 35; // pos 34 = 50
	
	for(i = 0; i < CHILDS; i++) {
		if (pipe(fd[i]) == -1) {
			fprintf(stderr, "Pipe Failed");
			return 1;
		}
	}
	
	for(i=0; i < CHILDS; i++) {
		pids[i] = fork();
		if(pids[i] == 0) {	// estamos no filho
			close(fd[i][0]); //fecha descritor de leitura do pipe correspondente a esse filho
			for(j = i*200; j < (i+1)*200; j++) {
				int sum; // variavel para guardar o somatorio da posição j de vec1+vec2
				sum = vec1[j] + vec2[j];
				transferir_res_pos[0] = sum; //array de 2 posiçoes para guardar resultado da soma e posição que corresponde
				transferir_res_pos[1] = j; 
				write(fd[i][1], transferir_res_pos, sizeof(transferir_res_pos)); // envia para processo pai o array
			}
			close(fd[i][1]); // fecha descritor escrita
			exit(0);
		}
		else {
			close(fd[i][1]);
			for(j = 0; j < 200; j++) {
				read(fd[i][0], transferir_res_pos, sizeof(transferir_res_pos));
				posicao = transferir_res_pos[1];
				result[posicao] = transferir_res_pos[0]; // guarda no array "result" a soma de vec1 + vec2 na posição correspondente
			}
			close(fd[i][0]);
		}
	}
    
    for (i = 0; i < CHILDS; i++) {
        waitpid(pids[i], &status, 0);
    }
    
    for(i = 0; i < ARRAY_SIZE; i++) {
		printf("pos %d: %d\n", i+1, result[i]);
	}
    
	return 0;
}
