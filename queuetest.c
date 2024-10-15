#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include "queue.h"


int main() {
    Queue * test;
    init_queue(test);

    printf("%d\n",isEmpty(test));

    char* elt = malloc(sizeof(char)*4);
    elt[0] = 't';
    elt[1] = 'e';
    elt[2] = 's';
    elt[3] = 't';

    char* retarr;


    enqueue(test, elt);

    printf("%d\n",isEmpty(test));

    dequeue(test,retarr);
    for (int i = 0; i < strlen(retarr); i++){
        printf("%c", retarr[i]);
    }

} 