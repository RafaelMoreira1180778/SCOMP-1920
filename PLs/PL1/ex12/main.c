#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int spawn_childs(int n)
{
    int pids[n]; //array que armazena os pids dos processos filho criados
    int i, status, index;

    for (i = 0; i < n; i++)
    {
        pids[i] = fork(); //a=pid processo filho
        if (pids[i] == 0) //estamos dentro do processo filho
        {
            index = i + 1; //index value do processo filho
            return index * 2;
        }
    }

    for (i = 0; i < n; i++)
    {
        waitpid(pids[i], &status, 0); //o pai espera que cada um dos filhos termine
    }

    return 0; //apenas chegamos a este return de 0 quando TODOS os processos filho terminam
}

int main()
{

    printf("%d\n", spawn_childs(6));

    return 0;
}