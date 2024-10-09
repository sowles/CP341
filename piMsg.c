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
	//KNOWN PINS
	//Port 1 tx: 27
	//Port 1 rx: 26
	//Port 2 tx: 25
	//Port 2 rx: 24
	//Port 3 tx: 23
	//Port 3 rx: 22
	//Port 4 tx: 21
	//Port 4 rx: 20


uint32_t startTick = 0;
uint32_t STARTTEST = 0;
int count = 0;
#define DELTA_T 6000
#define MAX_BITS 6400 // maximum number of bits we can record in a message
char* bitBuffer;
int bitCt = 0;
int lastEdgeDir =-1; //no prev edge
bool hasRisen = false;
clock_t lastFallTime;

void* receiveHandshake(void* args);

int send_info(int pi, int bit);
char binaryToASCII(char*);
void asciiToBinary(char *text, char *binary); 
void* send_mode(void* args);
//void *send_mode(int pi);
int pi;

int main(){

	pi = pigpio_start(NULL,NULL);

	pthread_t rec_thread;
	pthread_create(&rec_thread, NULL,&receiveHandshake, NULL);
	pthread_t send_thread;
	pthread_create(&send_thread, NULL,&send_mode, NULL);

	while(1){
	printf("Starting rec\n");
	pthread_join(rec_thread, NULL);
	printf("Starting send\n");
	pthread_join(send_thread, NULL);
	}

	for(int i = 0; i <20; i++){
		//send_mode(pi);
	//	printf("Receiving:\n");
	//	receiveHandshake(pi); // wait until something has been sent --> switch back
	}	

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
	printf("Sending:\n");

//	while(1){
		
		//read ASCII bits user sends (same size 101)
	//	char text[400];
		char * text = NULL;
		char *binary;
		int binaryLength;
    	printf("Enter a sentence (max 100 characters): ");
    	//	scanf("%100[^\n]", text); //read any char except newline char [^\n], max100
		//scanf("%100s[^\n]", text);
			
		
	//	fgets(text, sizeof(text), stdin);
		size_t size = 30;
		ssize_t read = getline(&text, &size, stdin);
		
		usleep(sizeof(text)*DELTA_T);

		//fclose(stdin);
		binaryLength = strlen(text) * 16; //16 bit ASCII chars
		binary =  calloc(binaryLength + 1, sizeof(char)); //+1 for null ptr

		if (binary == NULL) {
			fprintf(stderr, "Memory alloc failed\n");
			exit(1);

		}
		asciiToBinary(text, binary);
		//printf(text);
		//send header bit
		gpio_write(pi,27,1);
		usleep(DELTA_T);
		//send read binary data
		for (int i =0; binary[i]!= '\0';i++){
//			printf("%c\n",binary[i]);
			int bit = binary[i] - '0';
			send_info(pi,bit);

		}
		//trailer bit
		gpio_write(pi,27,0);
		usleep(DELTA_T);
		free(binary);
		free(text);
//}
	//fclose(stdin);	
	pthread_exit(NULL);
		//receiveHandshake(pi);

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
//	printf("Binary to ascii running\n");
	double decimal = 0.0;
	// take our 16 bits, convert to 8 bits using decode
 	
	int * return_arr = malloc(sizeof(int)*8);
	int* param_arr = malloc(sizeof(int)*16); //empty array to be filled
	for (int i = 0; i <16; i++){
		param_arr[i] = (int) binary[i];
		
	}

	ec_decode(param_arr, return_arr);
	// possibly need to change return_arr to char

	// takes 8 bits, converts to decimal
	for (int i=7; i >=0 ;i--){
	//	printf("%d\n", binary[i]);
		double base = 2.0;
		double exp = 7-i;	
		decimal += return_arr[i] * (pow(base,exp)); // TODO: switch to return_arr[i]
		
		//asciiChar <<= 1;
		//if(binary[i] == '1'){
		//	asciiChar |= 1; //inclusiv OR 


		}
		
	//printf("%f\n", decimal);	
	char asciiChar = (int) decimal;

	return asciiChar; 


	
}
//convert single character uinto its 8bit binary representation
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
	
//	printf("%d", edgeDirection);
	// set buffer of 5000
    int buff = 5000;
    uint32_t correctTick = startTick + (2 * DELTA_T);
   int diff = correctTick - tick;
	//printf("correctTick: %u, tick: %u, diff: %d\n", correctTick, tick, diff);

    if((tick < (correctTick + buff)) && (tick > (correctTick - buff))){
	
//	printf("Within buffer\n");
       // printf("Within range: %d\n", edgeDirection);
		//printf("Tick within range. \n");

        startTick = tick;
		// return true;

            //store bits in bitBuffer
	bitBuffer[bitCt++] = edgeDirection;
            // check to see if we have received 8 bits ( each char 8 bits)
            if (bitCt % 16 == 0){ //TODO change back to 16 instead of 8, as well as the next 4 lines
		    //printf("Byte sent\n");
                char byteStr[16];
                for(int i = 0; i < 16; i++){
                    byteStr[i] = bitBuffer[bitCt - 16+i];
		    
                }
                byteStr[16] = '\0';
                char asciiChar = binaryToASCII(byteStr); //converts bites receeived to ASCII chars
		
                printf("%c", asciiChar);
                fflush(stdout); 
            }
	    else{
	    }


        return true;
    } else {
        return false;
    }
}

void fallingFunc(int pi, unsigned user_gpio, unsigned level, uint32_t tick){
	//printf("Down - tick: %u\n", (tick-STARTTEST));
	//printf("Down\n");
	// disregard first (and eventually last) edge 
	int edgeDirection = 1;
	
	if (withinBuffer(tick, edgeDirection)){
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
	//usleep(5000000);
	//return 0;
	//while(1){
	//	usleep(1000);
		clock_t current = clock();
		int diff = current - (int) lastFallTime;	
		if ((lastFallTime != 0) && (diff > 40*DELTA_T)){
			printf("\nreceiving over\n");
			lastFallTime = 0;
			hasRisen = false;
			count = 0;
			free(bitBuffer);
		//	bitBuffer = malloc(MAX_BITS);
			pthread_exit(NULL);

		//	return 0;
		}	
			
	//}
//	return 0;

}
