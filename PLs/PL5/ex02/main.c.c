#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define ELEM 5

struct DataStruct {
    int number;
    char name[50];
    char address[50];
};

void* funcPrint(void *arg) {
    struct DataStruct* data = (struct DataStruct *) arg;
    printf("Number %d, ", data->number);
    printf("Name: %s, ", data->name);
    printf("Address: %s\n", data->address);
    pthread_exit(NULL);
}

int main() {
    int i;
    struct DataStruct data[ELEM];
    pthread_t thread_ids[ELEM];

    for (i = 0; i < ELEM; i++) {
        data[i].number = i;
        sprintf(data[i].name, "nome %d", (i*10));
        sprintf(data[i].address, "morada %d", (i*100));
    }
   
    for(i = 0; i < ELEM; i++) {
        if(pthread_create(&thread_ids[i], NULL, funcPrint, (void*)(data+i))) {
            perror("Error creating thread.");
        }
    }

    for(i = 0; i <ELEM; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    return 0;
}