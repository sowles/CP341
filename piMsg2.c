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
#include "queue.h"
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

#define YOUR_PI_ADDRESS 0b1111 // 15  in decimal
#define TARGET_PI_ADDRESS 0b0111//7 indecimal //address of pi you want to send msg to
#define YOUR_PI_ADDRESS 0b0111 // 7 in decimal
#define TARGET_PI_ADDRESS 0b0011// 3 in decimal
#define YOUR_PI_ADDRESS 0b0011 // 3  in decimal
#define TARGET_PI_ADDRESS 0b0111//15 in decimal

#define PACKET_SIZE_BITS 8 //sz of packet header (send and receive addresses)
// #define NEXT_PI_ADDRESS 1 //IP/addy of next pi in the ring



int pi;                        
uint32_t startTick = 0;   // starter tick for timing calculations
int count = 0;      // count for bits received
char* bitBuffer;   // buffer for received bits
int bitCt = 0;     //bitCOUNT
bool hasRisen = false;   // flag to show a rising edge has occurred
clock_t lastFallTime;     //time of last falling edeg
bool queueLock = false; //for queue (sending)
void qsend(Queue * q, char * binary); //forqueue
Queue q; //for queue
//PROTOTYPEŠ
void* receiveHandshake(void* args);
void* send_mode(void* args);
void send_binary_packets(int pi,char *binary_packet);
char* packet_to_binary(Packet *packet);
char binaryToASCII(char* binary);
int send_info(int pi, int bit);
void charToBinary(unsigned char decimal, char *eight); 
Packet binary_to_packet(char *binary);
void send_packet_forward(Packet *packet);
bool withinBuffer(uint32_t tick, int edgeDirection);
void fallingFunc(int pi, unsigned user_gpio, unsigned level, uint32_t tick);
void risingFunc(int pi, unsigned user_gpio, unsigned level, uint32_t tick);

//packet structure
typedef struct {
	uint8_t sender_addy;
	uint8_t receiver_addy;
	char *data 

} Packet;

// sender and receiver address to 4 bits 0-15


int main(){

	pi = pigpio_start(NULL,NULL);

	//set modes? 

	pthread_t rec_thread;
	pthread_create(&rec_thread, NULL,&receiveHandshake, NULL);
	
	pthread_t send_thread;
	pthread_create(&qsend_thread, NULL, &send_mode,NULL);


	pthread_join(rec_thread, NULL);
    pthread_join(send_thread, NULL);

	
	// for(int i = 0; i <20; i++){
		//send_mode(pi);
	//	printf("Receiving:\n");
	//	receiveHandshake(pi); // wait until something has been sent --> switch back
	// }	
	
	// init_queue(&q);
	
	/**
	if (mode == SEND_MODE){
		send_mode(pi);
	}
	else{
		receiveHandshake(pi);
	}
	*/
	//free();
	pigpio_stop(pi);
	return 0;
}

void* send_mode(void* args){
	/*
	collects message and receiver address from user
	creates packet contains sender receiver and msg data 
	conver packet to binary string
	send packets
	binary to packet reconstructs msg from received bits--help handle messaging
	*/

	while(1){
	printf("Sending:\n");
/*
[1,1,1,1] = 15 indecimal
[0,1,1,1] = 7 decimal
[0,0,1,1] = 3 dec
*/
		uint8_t sender_addy = YOUR_PI_ADDRESS;
		uint8_t receiver_addy = TARGET_PI_ADDRESS;

		char *txt = NULL;
		// char *binary;
		// int binaryLength;
		sizet_t size =100;
    	printf("Enter a sentence (max 100 characters): ");
    	getline(&txt, &size, stdin);
		
		//remove new line from input
		txt[strcspn(txt,"\n")] = '\0';

		//create packets
		Packet packet; 
		packet.sender_addy = sender_addy & 0x0F;
		packet.receiver_addy = receiver_addy & 0x0F; // & 0x0F to mask and ensure 4 bits (decimal:0-15)
		packet.data = txt;
		
		//convert packet to a binary string
		char *binary_packet = packet_to_binary(&packet);
		
		//sendbinary packet
		send_binary_packets(pi,binary_packet);
		
		free(binary_packet);
		free(txt);	
		usleep(100000);
		}
		pthread_exit(NULL);
		
	}

		//scanf("%100s[^\n]", text);	
					//	fgets(text, sizeof(text), stdin);
// 		size_t size = 30;
// 		ssize_t read = getline(&text, &size, stdin);
		
// 		usleep(sizeof(text)*DELTA_T);

// 		//fclose(stdin);
// 		binaryLength = strlen(text) * 16; //16 bit ASCII chars
// 		binary =  calloc(binaryLength + 1, sizeof(char)); //+1 for null ptr

// 		if (binary == NULL) {
// 			fprintf(stderr, "Memory alloc failed\n");
// 			exit(1);

// 		}
// 		asciiToBinary(text, binary);
// 		//printf(text);
// 		//send header bit
// 		gpio_write(pi,27,1);
// 		usleep(DELTA_T);
// 		//send read binary data
// 		for (int i =0; binary[i]!= '\0';i++){
// //			printf("%c\n",binary[i]);
// 			int bit = binary[i] - '0';
// 			send_info(pi,bit);

// 		}


// 	//	printf("%d",queueLock);
		
// 		queueLock = true;
// 		enqueue(&q, binary);
		
// 		queueLock = false;
// 		//printf("%d",queueLock);
// 		//trailer bit
// 		gpio_write(pi,27,0);
// 		usleep(DELTA_T);
// 		free(binary);
// 		free(text);
// //}
// 	//fclose(stdin);	
// 	return 0;
// 		//receiveHandshake(pi);

		
		
char* packet_to_binary(Packet *packet){
	//convert 4 bit addresses into binary strings
	//convert msg data to binary and sppend to packet

	size_t lenData = strlen(packet->data)*16;
	size_t lenTotal = 8 + lenData;

	char *binary = calloc(PACKET_SIZE_BITS + 1, sizeof(char)); //np = +1
	if (binary==NULL){
		fprintf(stderr,"Mem alloc fail\n");
			exit(1);
			}
		//sender and receeive addys--> binary
		for(int i=3; i>=0;--i){
			binary[3-i] = ((packet->sender_addy >> i)&1) +'0';
			binary[7-i] = ((packet->receiver_addy >> i)&1) +'0';
			}
		//data to binary + append
		char *data_binary = calloc(strlen(packet->data)*16+1,sizeof(char));
		if(data_binary == NULL){
			fprintf(stderr,"Mem alloc failed\n");
			exit(1);

		}
		asciiToBinary(packet->data,data_binary);
		strcat(binary,data_binary);

		free(data_binary);
		return binary;

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
		}																											//	scanf("%100[^\n]", text); //read any char except newline char [^\n], max100
					
void qsend(Queue *q, char * binary){
    //while true waiting for queueLock to become false
    while(1){
    //lock queue
    
    //dequeue
    //unlock
    //send message
    if (queueLock == false){
	queueLock = true;
	dequeue(&q, binary);
    	queueLock = false;
    }




	}
}
int send_info(int pi, int bit){	
//printf("in send info\n");
	gpio_write(pi,27,bit);
	usleep(DELTA_T);	
	if(bit == 1){
		
		gpio_write(pi,27,0);
		usleep(DELTA_T);
		return 0;	
	}else{
	
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
	// possibly need to change return_arr to char
	int decimal = 0;
	// takes 8 bits, converts to decimal
	for (int i=0; i < 8; i++){
		//printf("%d,", binary[i]);
		decimal = (decimal << 1) | return_arr[i];

	}
	free(return_arr);
	free(param_arr);

	return (char)decimal;
}
void charToBinary(unsigned char decimal, char *eight) { //char =ascii char, eight==char arr for binary string to be stored (9 long to hold 8 bit and terminator)
  //  printf("in char to bin\n");
	
	//bitwise to extract bits
	for(int i = 7; i >= 0; --i) {
        eight[7 - i] = ((decimal >> i) & 1) + '0';
    }
	
	int* encode_param = calloc(8, sizeof(int));
	int* encode_ret = calloc(16, sizeof(int));


	// convert eight to int
//	printf("eight: \n");
	for (int i= 0; i < 8; i++){
		int dec = (int) eight[i];
		if (dec == 48){
			encode_param[i] = 0;
		}
		else{
			encode_param[i] = 1;
		}
//		printf("%c",eight[i]);
	}
//	printf("\ndone\n");
	// call encode function
//	printf("After switching to ints\n");
//	for (int i = 0; i < 8; i++){
//		printf("%d", encode_param[i]);
//	}
//	printf("\ndone\n");
	ec_encode(encode_param, encode_ret);

	for(int i = 0; i<16; i++){
//		printf("%d", encode_ret[i]);
	
		if (encode_ret[i]  == 0){
			eight[i] = '0';
			
			
		}
		else{
			eight[i] =  '1';
		}

//		char temp = (char) encode_ret[i];
//		eight[i] = temp;
//		printf("%c", eight[i]);
		
	}
	// switch the sixteen return arr to char 
		
	free(encode_param);
	free(encode_ret);


    	eight[16] = '\0'; // Null-terminate the string
}
//converts string of ASCII chars to binary string (goes through each char and concatennates results)
void asciiToBinary(char *text, char *binary) { //exclude spaces (line25)
//	printf("in ascii to bin\n");
       	char *eight = calloc(17,sizeof(char)); //alloc 8-bit mem to represent each character sent
    if (eight == NULL){
	    exit(1);
    }
    while(*text) // loop through each char
    {
        charToBinary(*text, eight); //convert char to binary
        char *eight_ptr = eight;

        while(*eight_ptr) //add binary repr to output string
            *binary++ = *eight_ptr++;
        // *binary++ = ' ';
        ++text; //move to next character in input txt
    }
    *binary = '\0'; //nullterm

    // binary -= binaryLength;
   // printf("before free eight\n");
    free(eight);
   // printf("After free eight");
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
	int diff = correctTick - tick;

	//printf("correctTick: %u, tick: %u, diff: %d\n", correctTick, tick, diff);

    if((tick < (correctTick + buff)) && (tick > (correctTick - buff))){
		startTick = tick;
		bitBuffer[bitCt++] = edgeDirection + '0'; //conmvert to '0'or'1'
		return true;}else{
			return false;
		}
}
void fallingFunc(int pi, unsigned user_gpio, unsigned level, uint32_t tick){
	//printf("Down - tick: %u\n", (tick-STARTTEST));
	//printf("Down\n");
	// disregard first (and eventually last) edge 
	int edgeDirection = 1;
	
	if (withinBuffer(tick, edgeDirection)){
		//	printf("%d,",1);
			startTick = tick;
			count += 1;
		//	printf("Read falling: %u\n", level);
		//	printf("StartTick set: %u", tick);
			if(hasRisen){
				lastFallTime = clock();	
			}	
		}


	}
void risingFunc(int pi, unsigned user_gpio, unsigned level, uint32_t tick){
	//printf("Up\n");
	//printf("Up - tick: %u\n", (tick-STARTTEST));
	// disregard first (and eventually last) edge
	//int edgeDirection = 1;
	hasRisen = true;
	if (count == 0){
		startTick = tick;
	//	printf("Header bit disegarded.\n");
		//printf("Header read! Start tick: %u\n", tick);
		count += 1;
	}
	else if (withinBuffer(tick, 0)){
	//	printf("%d,",0);
		//printf("Read rising: %u\n", level);
		//printf("StartTick set: %u", tick);
		count += 1;
		}

}


void* receiveHandshake(void* args){
	
	// callback(pi,EITHER_EDGE,edgeCallback)
	callback(pi,20,FALLING_EDGE,fallingFunc);
	callback(pi,20,RISING_EDGE,risingFunc);
	
	bitBuffer = malloc(MAX_BITS);
	if (bitBuffer == NULL){
		fprintf(stderr,"memalloc failed\n")
		exit(1);
	}
	bitCt = 0;
	//usleep(5000000);
	//return 0;
	while(1){
		usleep(1000);

		clock_t current = clock();
		int diff = current - (int) lastFallTime;	
		if ((lastFallTime != 0) && (diff > 40*DELTA_T)){
		//	printf("\nreceiving over\n");
			lastFallTime = 0;
			hasRisen = false;
			count = 0;
			//free(bitBuffer);
			bitBuffer[bitCt] = '\0'// malloc(MAX_BITS);
		//	pthread_exit(NULL);
			Packet packet = binary_to_packet(bitBuffer)

			if(packet.data != NULL){
				if (packet.receiver_addy == YOUR_PI_ADDRESS){
					printf("Received msg from %d: %s\n",packet.sender_addy,packet.data);
				
				} else {
					send_packet_forward(&packet);

				}
				free(packet.data);

			} else {
				fprintf(stderr,"Failed to parse packets\n");

			}
		//reset bitBuffer for next msg
		free(bitBuffer);
		bitBuffer = malloc(MAX_BITS);
		if(bitBuffer == NULL){
			//print error msg agai!
			fprintf(stderr,"Mem alloc failed\n");
			exit(1);
		}
		bitCt = 0;
		}
	}
	pthread_exit(NULL);
}

Packet binary_to_packet(char *binary){
	Packet packet;
	char addy_str[5] = {0}; //4bits and null terminator
	//acquire snder addy
	strncpy(addy_str,binary,4);
	packet.sender_addy = strtol(addy_str,NULL,2);

	//get receiver addy
	strncpy(addy_str,binary+4,4);
	packet.receiver_addy = strtol(addy_str,NULL,2);

	//get data
	char *data_binary =binary + 8; //skip first 8 bits (address)
	//get length ofdqta
	size_t data_length = strlen(data_binary);
    if (data_length % 16 != 0){
		fprintf(stderr,"Invalid binary length(not16!)\n")
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
	//covert packet back to binary
	char *binary_paclet = packet_to_binary(packet);
	//send to next pi in ring
	send_binary_packets(pi,binary_packet);
	//cleanup
	free(binary_packet);
}