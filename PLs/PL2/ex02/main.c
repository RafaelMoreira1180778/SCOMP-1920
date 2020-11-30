#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

struct s
{
    int integer;
    char string[100];
};

int main()
{

    int p, fd[2], received, inputInt;

    struct s structInput;
    struct s structReceived;

    char inputStr[100];
    char receivedStr[100];

    if (pipe(fd) == -1) //cria o pipe
    {
        perror("Pipe failed");
        return 1;
    }

    p = fork();

    if (p > 0) //estamos dentro do processo pai
    {
        close(fd[0]);

        scanf("%s %d", inputStr, &inputInt);

        write(fd[1], &inputInt, sizeof(inputInt));
        write(fd[1], inputStr, sizeof(inputStr));

        structInput.integer = inputInt;
        strcpy(structInput.string, inputStr);

        write(fd[1], &structInput, sizeof(structInput));

        close(fd[1]);
    }
    else if (p == 0) //estamos dentro do processo filho
    {
        close(fd[1]);

        read(fd[0], &received, sizeof(received));
        read(fd[0], receivedStr, sizeof(receivedStr));

        read(fd[0], &structReceived, sizeof(structReceived));

        printf("\nInt: %d\n", received);
        printf("String: %s\n", receivedStr);

        printf("\nSTRUCT: string: %s; int: %d\n", structReceived.string, structReceived.integer);
        close(fd[0]);

        exit(0);
    }

    return 0;
}