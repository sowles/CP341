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

void init_queue(Queue* q);
void enqueue(Queue* q, char *element);
int dequeue(Queue* q, char** element);
/**
int main(){
	
	Queue q;

	init_queue(&q);
	enqueue(&q, "banana");
	enqueue(&q, "apple");
	enqueue(&q, "dog");
	enqueue(&q, "cherry");
	enqueue(&q, "mango");
	
	char *element;
	int test = dequeue(&q, &element);
	printf("Dequeue'd: %s\n",element);

	return 0;	
}
*/




//create q
void init_queue(Queue* q){
	q->size = 0;
	q->first = -1;
	q->last = -1;

}

// is queue empty or full ?
int isEmpty(Queue* q){
	return (q->first == -1);
}

int isFull(Queue* q){
	return (q->last == MAX_SIZE);
}


	
// ret_arr is an empty array being passed in which will be filled
//Function to remove an emnt from the queue (dequeue)
//ret is changed
int dequeue(Queue* q, char** element){ // char* ret
	if(isEmpty(q)){
	    printf("queue is empty\n");
	    return 1; // null = error

	}
	*element =q->arr[q->first];
	
	if (q->first == q->last){
	//one elmnt present
		q->first = -1;
		q->last = -1;
	
	} else {
		q->first = (q->first+1) % MAX_SIZE;
	}

	return 0;
}
// enqueue arr
// function to get element at the front of our queue

void enqueue(Queue* q,char *element){

//char * item;
	if(isFull(q)){
    	printf("Queue is full\n");
    		return;
  	}	

	if(isEmpty(q)){
        	q->first =0;
   	 }
    	
	q->last = (q->last+1) % MAX_SIZE;
    //allocate mem for new item
    q->arr[q->last] = (char *)malloc(strlen(element) + 1);
        if(q->arr[q->last] == NULL){
		printf("Mem alloc failed!\n");
		exit(EXIT_FAILURE);
		}
		//copyitem into q using strcpy
	strcpy(q->arr[q->last],element);
}
