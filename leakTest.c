#include <stdio.h>
#include <pigpiod_if2.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "checksum.h"
#include <pthread.h>
#include "queue2.h"
	//KNOWN PINS
	//Port 1 tx: 27
	//Port 1 rx: 26
	//Port 2 tx: 25
	//Port 2 rx: 24
	//Port 3 tx: 23
	//Port 3 rx: 22
	//Port 4 tx: 21
	//Port 4 rx: 20



#define DELTA_T 6000
#define MAX_BITS 6400 // maximum number of bits we can record in a message
//ring topology: each Pi knows the IP address of next pi in the ring. Hardcode these

//REMEMBER EACH PI ONLY GETS ON DEFINITION!!

/*
[1,1,1,1] = 15 indecimal
[0,1,1,1] = 7 decimal
[0,0,1,1] = 3 dec
*/

#define YOUR_PI_ADDRESS 0b1111 // 15  in decimal
#define TARGET_PI_ADDRESS 0b0111//7 indecimal //address of pi you want to send msg to
//#define YOUR_PI_ADDRESS 0b0111 // 7 in decimal
//#define TARGET_PI_ADDRESS 0b0011// 3 in decimal
//#define YOUR_PI_ADDRESS 0b0011 // 3  in decimal
//#define TARGET_PI_ADDRESS 0b1111//15 in decimal

#define PACKET_SIZE_BITS 8 //sz of packet header (send and receive addresses)
// #define NEXT_PI_ADDRESS 1 //IP/addy of next pi in the ring


//typedef struct {
//	uint8_t sender_addy;
//	uint8_t receiver_addy;
//	char *data; 

//} Packet;

int pi;                        
uint32_t startTick = 0;   // starter tick for timing calculations
int count = 0;      // count for bits received
char* bitBuffer;   // buffer for received bits
int bitCt = 0;     //bitCOUNT
bool hasRisen = false;   // flag to show a rising edge has occurred
clock_t lastFallTime;     //time of last falling edeg
bool queueLock = false; //for queue (sending)
void* qsend(void *args); //forqueue
Queue q; //for queue
//PROTOTYPEŠ
void* receiveHandshake(void* args);
void* qsend(void* args);
void send_binary_packets(int pi,char *binary_packet);
char* packet_to_binary(Packet *packet);
char binaryToASCII(char* binary);
void asciiToBinary(char *text, char *binary);
int send_info(int pi, int bit);
void charToBinary(unsigned char decimal, char *eight); 
Packet binary_to_packet(char *binary);
void send_packet_forward(Packet *packet);
bool withinBuffer(uint32_t tick, int edgeDirection);
void fallingFunc(int pi, unsigned user_gpio, unsigned level, uint32_t tick);
void risingFunc(int pi, unsigned user_gpio, unsigned level, uint32_t tick);

//packet structure

// sender and receiver address to 4 bits 0-15


int main(){
	char * etest = calloc(17, sizeof(char));
	unsigned char a = 'a';
	charToBinary(a,etest);

//	printf("chartobinary %s\n", etest);
	
	
	pi = pigpio_start(NULL, NULL);
	init_queue(&q);

	pthread_t rec_thread;
	pthread_create(&rec_thread, NULL, &receiveHandshake, NULL);

	pthread_t qsend_thread;
	pthread_create(&qsend_thread, NULL, &qsend, NULL);

while(1){
	//printf("Sending:\n");

		uint8_t sender_addy = YOUR_PI_ADDRESS;
		uint8_t receiver_addy;

		char *txt = NULL;
		size_t size =0; // init size to 0 not 100 change
		printf("What computer do you want to send to? 15, 7, 3: ");\
		scanf("%hhu",&receiver_addy);
		
		//clear out input 
		int c;
		while((c= getchar()) != '\n' && c!= EOF);

	
		//	getchar();
	//	getline(&txt,&size,stdin);


    		printf("Enter a sentence (max 100 characters): ");
    		getline(&txt, &size, stdin);
		
		//remove new line from input
		txt[strcspn(txt,"\n")] = '\0';

		//create packets
		Packet *packet = malloc(sizeof(Packet)); //TODO - likely won't work, change amount to text + uints
		if (packet == NULL){
		    fprintf(stderr,"Mem alloc failed");
		    free(txt);
		    exit(1);
		
		}
	
		// [1,1,1,1]
		packet ->sender_addy = sender_addy & 0x0F;
		packet ->receiver_addy = receiver_addy & 0x0F; // & 0x0F to mask and ensure 4 bits (decimal:0-15) [1,1,1,1]
		packet ->data =strdup(txt); // dupe string to allocate separate memory
		if (packet->data == NULL){
			fprintf(stderr,"Mem allocfaild packet->data, freeing");
			free(packet);
			free(txt);
			exit(1);
		}
	//	printf("%d",packet->sender_addy);	
		//queue packets
		enqueue(&q, packet);

		free(packet->data);
		free(packet);
		free(txt);

//		printf("%d", q.size);
	//	printf("%d",q.size);
	//	free(packet); // TODO will freeing here make it unusable?
//		free(txt);
	//	free()	
		usleep(100000);
}


	pigpio_stop(pi);
	return 0;
}

void* qsend(void* args){
 
	Packet* packet = NULL; //TODO do we need to malloc here?
//	Packet *text = malloc(sizeof(char) * 1000);
	//char *binary;
	//int binaryLength;
	// while true waiting for queueLock to become false
	while (1)
	{
		usleep(DELTA_T);
		if (q.size > 0) 
		{
			// waits until q is unlocked to do this. 
			dequeue(&q, &packet);
		//	printf("dequeued: %s\n", packet->data);
			
			
		}
		else
		{
			usleep(DELTA_T);
			continue;
		}
		//check if packet and packet->data are valid
		if (packet == NULL || packet->data == NULL){
		    fprintf(stderr,"invalid packet dequeued from quuee");
		    continue;
		}
		//convert packet to binary string
	//	printf("Before pack bin\n");
		//binary_packet = malloc(sizeof(packet)*100);
		char * binary_packet = packet_to_binary(packet);
		
	//	printf("Binary Packet!!!%s\n", binary_packet);
//		printf("\nlen: %ld\n", strlen(binary_packet));

	//	packet_to_binary(packet);
	//	binaryLength = (sizeof(uint8_t)*8)+(strlen(text->data) * 16); // each char is 16-bit binary
	//	binary = calloc(binaryLength + 1, sizeof(char));

		if (binary_packet == NULL)
		{

			fprintf(stderr, "Failed to change packet to binary\n");
			
			free(packet->data);
			free(packet);
			continue;
		}
		
		//printf("Sending data now\n");	
		
		send_binary_packets(pi,binary_packet);
		
		free(packet->data);	
		free(packet);
	//	free(packet->data); //TODO might be necessary?
		//free(binary_packet);
	
		//usleep(2000); //TODO changes
		//printf("Sending done\n");
		}
	

	pthread_exit(NULL);
	
}
		
	
/*
Merge goal: 
Collects user input (receiver address and message).
Converts the message to binary.
Sends the binary data over GPIO.
Optionally, handles the queue if necessary.

Queue packets



Store strings as packets and send packets in queuue order: 
send mode stre strings as packets and put them into the queue
main takes them out of queue puts into binary and sends that 

*/


	/*
	collects message and receiver address from user
	creates packet contains sender receiver and msg data 
	conver packet to binary string
	send packets
	binary to packet reconstructs msg from received bits--help handle messaging
	*/

	// while(1){
	// printf("Sending:\n");

	// 	uint8_t sender_addy = YOUR_PI_ADDRESS;
	// 	uint8_t receiver_addy;

	// 	char *txt = NULL;
	// 	size_t size =100;
	// 	printf("What computer do you want to send to? 15, 7, 3: ")
	// 	scanf("%hhu",&receiver_addy);
	// 	getline(&txt,&size,stdin);


    // 		printf("Enter a sentence (max 100 characters): ");
    // 		getline(&txt, &size, stdin);
		
	// 	//remove new line from input
	// 	txt[strcspn(txt,"\n")] = '\0';

	// 	//create packets
	// 	Packet packet; 
	// 	// [1,1,1,1]
	// 	packet.sender_addy = sender_addy & 0x0F;
	// 	packet.receiver_addy = receiver_addy & 0x0F; // & 0x0F to mask and ensure 4 bits (decimal:0-15) [1,1,1,1]
	// 	packet.data = txt;
		
	// 	//queue packets
	// 	enqueue(&q, &packet);


		//dequeue?


		
		
char * packet_to_binary(Packet *packet){
	//check if packet oir packet data is NLUL
	if (packet ==NULL || packet->data == NULL){
	    fprintf(stderr,"Invalid packet or packet data\n");
	    return NULL;
	}

	//convert 4 bit addresses into binary strings
	//convert msg data to binary and sppend to packet
	//size_t lenData = malloc(sizeof(size_t)*16);
	size_t lenData = strlen(packet->data)*16;
	size_t lenTotal = sizeof(uint8_t)*2 + lenData; 
//	printf("calloc'd: %ld\n",lenTotal+1);
	char *binary = calloc(lenTotal + 1,sizeof(char));//null pointer + 1
	if (binary==NULL){
		fprintf(stderr,"Mem alloc fail\n");
			exit(1);
			}
		//sender and receeive addys--> binary
		for(int i=3; i>=0;--i){
			binary[3-i] = ((packet->sender_addy >> i)&1) +'0'; //TODO possibly need to type cast this somehow?
 			binary[7-i] = ((packet->receiver_addy >> i)&1) +'0';
			}
		//adding to null term string
		binary[8] = '\0';
		//data to binary + append
		char *data_binary = malloc(lenData+1*sizeof(char));
		if (lenData > 0){
		    if(data_binary == NULL){
			fprintf(stderr,"Mem alloc failed\n");
			free(binary);
			exit(1);

		}

//		printf("dequeued: %s\n", packet->data);
		asciiToBinary(packet->data,data_binary);
		
		//printf("data_binary: %s\n", data_binary);
		//printf("Two tests: %s\n%s\n", packet->data, data_binary);
		strcat(binary,data_binary);
//		printf("Two tests: %s\n%s\n", packet->data, data_binary);
		//free(binary);
		//free(data_binary);
		}
		return binary;
//		free(binary);
		}
void send_binary_packets(int pi,char *binary_packet){
		//send header!
		gpio_write(pi,27,1);
		usleep(DELTA_T);
		
		//send bit data
		for (int i = 0; binary_packet[i] != '\0';i++){
			int bit = binary_packet[i] - '0';
			send_info(pi,bit);
		}
		gpio_write(pi,27,0);
		usleep(DELTA_T);
		//send trailerrrrr bit! 
		}																			



int send_info(int pi, int bit){	
	gpio_write(pi,27,bit);
//	printf("%d", bit);
	usleep(DELTA_T);	
	if(bit == 1){
//		printf("0");
		gpio_write(pi,27,0);
		usleep(DELTA_T);
		return 0;	
	}else{
			
//		printf("1");
		gpio_write(pi,27,1);
		usleep(DELTA_T);	
		return 0;
	}
	return 1;

}
char binaryToASCII(char *binary){
	// take our 16 bits, convert to 8 bits using decode
	int * return_arr = malloc(sizeof(int)*8);
	int* param_arr = malloc(sizeof(int)*16); //empty array to be filled
	for (int i = 0; i <16; i++){
		param_arr[i] = binary[i] - '0'; // Convert '0'/'1' to 0/1
	}	

	ec_decode(param_arr, return_arr);
	int decimal = 0;
	// takes 8 bits, converts to decimal
	for (int i=0; i < 8; i++){
		decimal = (decimal << 1) | return_arr[i];

	}
	free(return_arr);
	free(param_arr);

	return (char)decimal;
}
void charToBinary(unsigned char decimal, char *eight) { 
	
	//bitwise to extract bits
	for(int i = 7; i >= 0; --i) {
        eight[7 - i] = ((decimal >> i) & 1) + '0';
    }
	//printf("decimal:%d", decimal);
	eight[8] = '\0';	
	int* encode_param = calloc(8, sizeof(int));
	int* encode_ret = calloc(16, sizeof(int));
	/*
	for(int i=0; eight[i]!= '\0'; i++){
		if(eight[i] == 0){
			printf("0");
		}else{ printf("1");
		}
		//printf("character at: %c \n", eight[i] );

	}*/
	// convert eight to int
	for (int i= 0; i < 8; i++){
		encode_param[i] = (eight[i] - '0');	
	
	}
	
	
	ec_encode(encode_param, encode_ret);

// changed this back to changing eight	
	for(int i = 0; i<16; i++){
	if (encode_ret[i] ==0){
		eight[i] = '0';	
	}	
	else {
		eight[i] = '1';
	}

		}


	eight[16] = '\0'; // null term string	
	free(encode_param);
	free(encode_ret);
	


}
//converts string of ASCII chars to binary string (goes through each char and concatennates results)
void asciiToBinary(char *text, char *binary) { 
//	printf("Starting ascii to bin\n"); 
	//check if text or binary is empty
  	if (text == NULL || binary == NULL){
		fprintf(stderr,"binary or txt null in asciitobinary(invalid)\n");
		return;
	}
	binary[0] = '\0';
	char *eight = calloc(8,sizeof(char)); //alloc 17-bit mem to represent each character sent
   	 if (eight == NULL){
	    exit(1);
    	}
   	 while(*text) { // loop thjrough each char
		//printf("calling char to bin");
       	 	charToBinary(*text, eight);
        	strcat(binary,eight); // char *eight_ptr = eight;
		++text;
       // while(*eight_ptr) //add binary representation to output string
     //       	*binary++ = *eight_ptr++;
   //     	++text; //move to next character in input txt
    }
 //   *binary = '\0'; 

    free(eight);
}
bool withinBuffer(uint32_t tick, int edgeDirection){
/*
making correction: edgeDirection stores bits as integers and later tries to use
them as characters in functions like binaryToAscii
Instead:store bits as characters 0,1 in BitBuffer!
*/
	//variables
    	int buff = 5000; 	// set buffer of 5000
	uint32_t correctTick = startTick + (2 * DELTA_T);
//	int diff = correctTick - tick;

	//printf("correctTick: %u, tick: %u, diff: %d\n", correctTick, tick, diff);

    if((tick < (correctTick + buff)) && (tick > (correctTick - buff))){
		startTick = tick;
		bitBuffer[bitCt++] = edgeDirection + '0'; //conmvert to '0'or'1'
		return true;}else{
			return false;
		}
}
void fallingFunc(int pi, unsigned user_gpio, unsigned level, uint32_t tick){

	// disregard first (and eventually last) edge 
	int edgeDirection = 1;
	
	if (withinBuffer(tick, edgeDirection)){
			startTick = tick;
			count += 1;
			if(hasRisen){
				lastFallTime = clock();	
			}	
		}


	}
void risingFunc(int pi, unsigned user_gpio, unsigned level, uint32_t tick){
	// disregard first (and eventually last) edge
	hasRisen = true;
	if (count == 0){
		startTick = tick;
		
		count += 1;
	}
	else if (withinBuffer(tick, 0)){

		count += 1;
		}

}


void* receiveHandshake(void* args){
	Packet packet;
	callback(pi,20,FALLING_EDGE,fallingFunc);
	callback(pi,20,RISING_EDGE,risingFunc);
	
	bitBuffer = malloc(MAX_BITS);
	if (bitBuffer == NULL){
		fprintf(stderr,"memalloc failed\n");
		exit(1);
	}
	bitCt = 0;

	while(1){
		usleep(1000);

		clock_t current = clock();
		int diff = current - (int) lastFallTime;	
		if ((lastFallTime != 0) && (diff > 40*DELTA_T)){
			lastFallTime = 0;
			hasRisen = false;
			count = 0;
			bitBuffer[bitCt] = '\0';
			packet = binary_to_packet(bitBuffer);

			if(packet.data != NULL){
				if (packet.receiver_addy == YOUR_PI_ADDRESS){
					printf("Received msg from %d: %s\n",packet.sender_addy,packet.data);
				
				} else {
					send_packet_forward(&packet);

				}
				free(packet.data);

			}
		
		//reset bitBuffer for next msg
		free(bitBuffer);
		bitBuffer = malloc(MAX_BITS);
		if(bitBuffer == NULL){
			//print error msg again!
			fprintf(stderr,"Mem alloc failed\n");
			exit(1);
		}
		bitCt = 0;
		}
	}
	pthread_exit(NULL);
}

Packet binary_to_packet(char *binary){ //TODO returning packet may be dicey, ask George
	Packet packet;

	//4bits and null terminator
	char addy_str[5] = {0}; 

	//acquire snder addy
	strncpy(addy_str,binary,4);
	packet.sender_addy = strtol(addy_str,NULL,2);

	//get receiver addy
	strncpy(addy_str,binary+4,4);
	packet.receiver_addy = strtol(addy_str,NULL,2);

	//get data
	char *data_binary =binary + 8; //skip first 8 bits (address)
	//get length ofdata
	size_t data_length = strlen(data_binary);
    if (data_length % 16 != 0){
		fprintf(stderr,"Invalid binary length(not16!)\n");
		packet.data = NULL;
		return packet;
	} //else
	size_t num_chars = data_length / 16; // each char is 16 bits (8data8parit)

	//alloc memory for the msg
	packet.data = calloc(num_chars + 1 , sizeof(char));
	if(packet.data==NULL){
		fprintf(stderr,"Allocation of Memory failed");
		exit(1);

	}
	char byteStr[17];//17for nullterm
	byteStr[16] = '\0';//terminate the string

	for (size_t i = 0;i< num_chars;i++){

		//process 2 bytes for one char
		strncpy(byteStr,data_binary+ (i*16),16);
		//use binaryToAscii to convert bits to chars
		char asciiChar = binaryToASCII(byteStr);
		//store char in msg data
		packet.data[i] = asciiChar;


	}
	packet.data[num_chars] = '\0'; // null term i nate the str i ng

	return packet;
}
void send_packet_forward(Packet *packet){
	if(packet->sender_addy == YOUR_PI_ADDRESS){
	    printf("checked that packet returned to sender");\
	    return;
	
	}

	//covert packet back to binary
	char* binary_packet = packet_to_binary(packet);
	//send to next pi in ring
	send_binary_packets(pi,binary_packet);
	//cleanup
	free(binary_packet);
}
