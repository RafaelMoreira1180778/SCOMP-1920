#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 100

void client(char *in) //client recebe a mensagem a alterar
{
    printf("Inserir Mensagem:\n");
    scanf("%s", in);
}

void server(char *string) //processa a mensagem a processar
{
    while (*string)
    {
        if (*string >= 'A' && *string <= 'Z')
        {
            *string = *string + 32;
        }
        else if (*string >= 'a' && *string <= 'z')
        {
            *string = *string - 32;
        }
        string++;
    }
}

int main()
{

    int fd1[2]; // pipe1
    int fd2[2]; // pipe2
    int status;

    char stdin[BUFFER_SIZE];
    char input[BUFFER_SIZE];
    pid_t p;

    if (pipe(fd1) == -1)
    {
        fprintf(stderr, "Pipe1 Failed");
        return 1;
    }
    if (pipe(fd2) == -1)
    {
        fprintf(stderr, "Pipe2 Failed");
        return 1;
    }

    p = fork();

    if (p > 0) //processo pai SERVER
    {
        close(fd1[1]);
        read(fd1[0], input, BUFFER_SIZE);
        close(fd1[0]);

        server(input);
        close(fd2[0]);
        write(fd2[1], input, strlen(input) + 1);
        close(fd2[1]);

        waitpid(p, &status, 0);
    }
    else if (p == 0) //processo filho CLIENT
    {
        close(fd1[0]);
        client(stdin);
        write(fd1[1], stdin, strlen(stdin) + 1);
        close(fd1[1]);

        close(fd2[1]);
        read(fd2[0], stdin, BUFFER_SIZE);
        close(fd2[0]);

        printf("Servered: %s\n", stdin);
        exit(0);
    }

    return 0;
}