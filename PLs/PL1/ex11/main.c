#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define ARRAY_SIZE 1000
#define PROCESSOS_FILHOS 5

int main()
{
    int pids[PROCESSOS_FILHOS];      //vetor que armazena os pids de cada um dos processos filho
    int maxValues[PROCESSOS_FILHOS]; //vetor que armazena os valores máximos que cada um dos processos encontrou
    int result[ARRAY_SIZE];          //vetor que armazena o resultado da operação da alínea c

    int numbers[ARRAY_SIZE]; /* array to lookup */
    int i, j, m = 0;
    int status; //status preciso para o exit status e waitpid

    /* intializes RNG (srand():stdlib.h; time(): time.h) */
    srand(time(NULL));

    /* initialize array with random numbers (rand(): stdlib.h) */
    for (i = 0; i < ARRAY_SIZE; i++)
    {
        numbers[i] = rand() % 255;
    }

    for (i = 0; i < PROCESSOS_FILHOS; i++)
    {
        pids[i] = fork();
        if (pids[i] == 0) //estamos dentro do processo filho
        {
            for (j = (i * 200); j < (i + 1) * 200; j++)
            {
                if (numbers[i] > m)
                {
                    m = numbers[i]; //se o valor do vetor numbers for maior que o m (maior) passa a ser m
                }
            }
            exit(m); //exit com o status que é o maior encontrado
        }
    }

    for (i = 0; i < PROCESSOS_FILHOS; i++)
    {
        waitpid(pids[i], &status, 0);       //espera por cada um dos processos filho
        maxValues[i] = WEXITSTATUS(status); //à medida que eles terminam preenche-se este vetor com os maiores encontrados
    }

    int max = 0;

    for (j = 0; j < PROCESSOS_FILHOS; j++)
    {
        max = maxValues[j];
        printf("Maior valor na %d/5 parte: %d\n", j + 1, max);
        if (max > m)
        {
            m = maxValues[j]; //no processo pai m = 0
        }
    }

    printf("\nMaior valor do vetor: %d\n\n", m);

    int pid = fork();

    if (pid == 0) //estamos dentro do processo filho
    {
        for (i = 0; i < ARRAY_SIZE / 2; i++)
        {
            //o processo filho vai computar a primeira metade do vetor de números
            result[i] = ((int)numbers[i] / m) * 100;
            printf("result[%d]=%d; numbers[%d]=%d; max=%d\n", i, result[i], i, numbers[i], m); //m é o maior valor do array final neste ponto
        }
        exit(0);
    }

    for (i = ARRAY_SIZE / 2; i < ARRAY_SIZE; i++)
    {
        //o processo filho vai computar a primeira metade do vetor de números
        result[i] = ((int)numbers[i] / m) * 100; //m é o maior valor do array final neste ponto
    }

    waitpid(pid, &status, 0);

    for (i = ARRAY_SIZE / 2; i < ARRAY_SIZE; i++)
    {
        printf("result[%d]=%d; numbers[%d]=%d; max=%d\n", i, result[i], i, numbers[i], m);
    }

    return 0;
}