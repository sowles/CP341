#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 20


typedef struct {
	// arr holds pointers to our messages
	
	//each element in arr array is now a pointer to char(string)	
	char* arr[MAX_SIZE];
	int first;
	int last;
	
	int size;
	
} Queue;

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
int dequeue(Queue* q){
	if(isEmpty(q)){
	    printf("queue is empty\n");
	    return NULL; // null = error

	}
	char *element = q->arr[q->first];
	if (q->first == q->last){
	//one elmnt present
	q->first = -1;
	q->rear = -1;


	} else {q->first = (q->first+1) % MAX_SIZE;}
	return element; // return ptr

	
	// 'return' the first element
	// move everything up


}
// enqueue arr
// function to get element at the front of our queue

int enqueue(Queue* q,char *element){
    if(isFull(q)){
    printf("Queue is full\n");
    return;
    }
    if(isEmpty(q)){
        q->first =0;
    }
    q->last = (q->last + 1 % MAX_SIZE;
    q->arr[q->rear] = item; ///store ptr directly

    }
    //can also use malloc toalloc memory and strcpy to copy string and free at end if this implmeentation doesnt work
    

    // add the arr to last open position

    q->first++;


}
