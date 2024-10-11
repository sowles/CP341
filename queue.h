#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_SIZE 20


typedef struct {
	uint8_t sender_addy;
	uint8_t receiver_addy;
	char *data; 

} Packet;

typedef struct {
	// arr holds pointers to our messages
	//each element in arr array is now a pointer to char(string)	
	Packet* arr[MAX_SIZE];
	int first;
	int last;
	
	int size;
	
} 

Queue;

void init_queue(Queue* q);
void enqueue(Queue* q, Packet* element);
int dequeue(Queue* q, Packet** element);

/**
int main(){
	
	Queue q;

	init_queue(&q);
	Packet packet1;
	packet1.sender_addy = 0b1111 & 0x0F;
	packet1.receiver_addy= 0b00111 & 0x0F;
	packet1.data = "apple";
	enqueue(&q, &packet1);

	Packet packet2;
	packet2.sender_addy = 0b1111 & 0x0F;
	packet2.receiver_addy= 0b00111 & 0x0F;
	packet2.data = "banana";
	enqueue(&q, &packet2);
	
	
	Packet *element;
	int test = dequeue(&q, &element);
	printf("Dequeue'd: %s\n",element->data);
	test = dequeue(&q, &element);
	printf("Dequeue'd: %s\n",element->data);

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
int dequeue(Queue* q, Packet** element){ // char* ret
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

void enqueue(Queue* q,Packet *element){

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
	size_t size = sizeof(uint8_t) *2  + strlen(element->data);
    q->arr[q->last] = (Packet *)malloc(size + 1);
        if(q->arr[q->last] == NULL){
		printf("Mem alloc failed!\n");
		exit(EXIT_FAILURE);
		}

		//copy packet into q

	q->arr[q->last] = element; //might not work
	// 
	//strcpy(q->arr[q->last],element);
}
