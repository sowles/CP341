#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_SIZE 20


typedef struct {
	// arr holds pointers to our messages
	
	//each element in arr array is now a pointer to char(string)	
	char* arr[MAX_SIZE];
	int first;
	int last;
	
	int size;
	
} 

Queue;

//create q
void init_queue(Queue* q){
	q->size = 0;

}

// is queue empty or full ?
int isEmpty(Queue* q){
	return (q->size == 0);
}

int isFull(Queue* q){
	return (q->size == MAX_SIZE);
}


	
// ret_arr is an empty array being passed in which will be filled
//Function to remove an emnt from the queue (dequeue)
char *  dequeue(Queue* q, char * ret){
	if(isEmpty(q)){
	    printf("queue is empty\n");
	    return NULL; // null = error

	}
	char *element = q->arr[q->first];
	if (q->first == q->last){
	//one elmnt present
	q->first = -1;
	q->last = -1;


	} else {q->first = (q->first+1) % MAX_SIZE;}
	
	ret = malloc(strlen(element));
	for (int i = 0; i < strlen(element); i++){
		ret[i] = element[i];
	}
		

	
	// 'return' the first element
	// move everything up


}
// enqueue arr
// function to get element at the front of our queue

int enqueue(Queue* q,char *element){
    char *  item;
if(isFull(q)){
    printf("Queue is full\n");
    	return 1;
  }
    if(isEmpty(q)){
        q->first =0;
    }
    q->last = (q->last + 1 % MAX_SIZE);
    q->arr[q->last] = item; ///store ptr directly

    //can also use malloc toalloc memory and strcpy to copy string and free at end if this implmeentation doesnt work
    

    // add the arr to last open position

    q->first++;
	q->size++;
	return 0;
}
