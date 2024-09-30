#include <stdio.h>
#include <pigpiod_if2.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
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
#define DELTA_T 10000
#define MAX_BITS 800
char bitBuffer[MAX_BITS];
int bitCt = 0;
int lastEdgeDir =-1; //no prev edge

int main(){
	int SEND_MODE = 0;
	int RECEIVE_MODE = 1;

	int clockrate = 1000;
	int pi = pigpio_start(NULL,NULL);
	

	int mode = RECEIVE_MODE;
	if (mode == SEND_MODE){
		//read ASCII bits user sends (same size 101)
		char text[101];
		char *binary;
		int binaryLength;
    	printf("Enter a sentence (max 100 characters): ");
    	scanf("%100[^\n]", text); //read any char except newline char [^\n], max100

		binaryLength = strlen(text) * 8; //8 bit ASCII chars
		binary =  malloc(binaryLength + 1); //+1 for null ptr

		if (binary == NULL) {
			fprintf(stderr, "Memory alloc failed\n");
			exit(1);

		}
		asciiToBinary(text, binary);

		//send header bit
		gpio_write(pi,27,1);
		usleep(DELTA_T);

		//send read binary data
		for (int i =0; binary[i]!= '\0';i++){
			int bit = binary[i] - '0';
			send_info(pi,bit);

		}
		//trailer bit
		gpio_write(pi,27,0);
		usleep(DELTA_T);
		binaryInterpreter(text);
		free(binary);
	}
	else{
		receiveHandshake(pi);
	}
	pigpio_stop(pi);
	return 0;
}
int send_info(int pi, int bit){	
	int half_rate = DELTA_T/2;

	gpio_write(pi,27,bit);
	usleep(DELTA_T);	
	if(bit == 1){
		
		gpio_write(pi,27,0);
		usleep(DELTA_T);	
	}else{
	
		gpio_write(pi,27,1);
		usleep(DELTA_T);	
	}


}
char binaryToASCII(char *binary){
	char asciiChar =0;
	for (int i=0;i<8;i++){
		asciiChar <<= 1;
		if(binary[i] == '1'){
			asciiChar |= 1; //inclusiv OR 


		}
	}
	return asciiChar; 
	
}
void binaryInterpreter(char *inputString, char *outputString){
	for (int i=0;i<strlen(inputString);i++){
		outputString[i] = binaryToASCII(inputString[i]);
		printf(outputString[i]);
	}
}
//convert single character uinto its 8bit binary representation
void charToBinary(unsigned char decimal, char *eight) { //char =ascii char, eight==char arr for binary string to be stored (9 long to hold 8 bit and terminator)
    //bitwise to extract bits
	for(int i = 7; i >= 0; --i) {
        eight[7 - i] = ((decimal >> i) & 1) + '0';
    }
    eight[8] = '\0'; // Null-terminate the string
}
//converts string of ASCII chars to binary string (goes through each char and concatennates results)
void asciiToBinary(char *text, char *binary) { //exclude spaces (line25)
    char *eight = malloc(9); //alloc 8-bit mem to represent each character sent
    if (eight == NULL)
        exit(1);
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
    free(eight);
}
bool withinBuffer(uint32_t tick, int edgeDirection){
//implementing a char array to store received data
	char recArr[20];	
	// set buffer of 5000
    int buff = 5000;
    uint32_t correctTick = startTick + (2 * DELTA_T);
    int diff = correctTick - tick;
	//	printf("correctTick: %u, tick: %u, diff: %d\n", correctTick, tick, diff);

    if((tick < (correctTick + buff)) && (tick > (correctTick - buff))){
        printf("%d\n", edgeDirection);
	
		//printf("Tick within range. \n");

        startTick = tick;
		// return true;
        int bit;
        if (lastEdgeDir != -1){
            if (lastEdgeDir == 0 && edgeDirection == 1){
                bit = 1; // Low to high transition represents '1'
            } else if (lastEdgeDir == 1 && edgeDirection == 0){
                bit = 0; // High to low transition represents '0'
            } else {
                // Invalid transition
                lastEdgeDir = edgeDirection;
                return false;
            }

            //store bits in bitBuffer
            bitBuffer[bitCt++] = bit + '0';

            // check to see if we have received 8 bits ( each char 8 bits)
            if (bitCt % 8 == 0){
                char byteStr[9];
                for(int i = 0; i < 8; i++){
                    byteStr[i] = bitBuffer[bitCt - 8 + i];
                }
                byteStr[8] = '\0';

                char asciiChar = binaryToASCII(byteStr); //converts bites receeived to ASCII chars
                printf("%c", asciiChar);
                fflush(stdout);
            }
        }

        // Update LASTEDGE 
        lastEdgeDir = edgeDirection;
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
			
		}


	}
void risingFunc(int pi, unsigned user_gpio, unsigned level, uint32_t tick){
	//printf("Up\n");
	
	//printf("Up - tick: %u\n", (tick-STARTTEST));
	// disregard first (and eventually last) edge
	int edgeDirection = 1;

	if (count == 0){
		STARTTEST =tick;
		startTick = tick;
		printf("Disegarded.\n");
		//printf("Header read! Start tick: %u\n", tick);
		count += 1;
	}
	else if (withinBuffer(tick, 0)){
		//printf("Read rising: %u\n", level);
		//printf("StartTick set: %u", tick);
		count += 1;
		}

}


int receiveHandshake(int pi){
	
	// callback(pi,EITHER_EDGE,edgeCallback)
	callback(pi,20,FALLING_EDGE,fallingFunc);
	callback(pi,20,RISING_EDGE,risingFunc);
	
	while(1){
		usleep(650);

	}
}
/*
	while(currentState == primaryState){
		currentState = gpio_read(pi,20);
		int toggle = gpio_write(pi,27,0);
		toggle = gpio_write(pi,27,1);
		clock_t endtime = clock();
		if((endtime-starttime)/CLOCKS_PER_SEC > 10){
			return 1;
		
		}
	}
*/
	//int toggle = gpio_write(pi,27,0);
	//toggle = gpio_write(pi,27,1);
	

	

// int packageReader(int pi, int sent){
// 	int toggle = gpio_write(pi, 27, 0);
// 	toggle = gpio_write(pi,27,sent);
// 	int state = gpio_read(pi, 20);
// 	return state;

// }





// main comments:
	/**	
	int *sending;
	int *recieved;
	sending = (int*) malloc(sizeof(int)*4);
	sending[0] = 1;	
	sending[1] = 0;
	sending[2] = 0;
	sending[3] = 1;
	
	recieved = (int*) malloc(sizeof(int)*4);
	recieved[0] = 0;	
	recieved[1] = 0;	
	recieved[2] = 0;	
	recieved[3] = 0;	
	for(int i = 0; i < 4; i++){
		int read = packageReader(pi, sending[i]);
		recieved[i] = read;
		printf("%d", recieved[i]);
	}
	*/

	// set mode here


	//OLD MAIN--hardcoded bits in send code--convert to binary string obtained from ASCII
	// if (mode == SEND_MODE){
	// 	//header
	// 	gpio_write(pi,27,1);
	// 	usleep(DELTA_T);
	// 	//data
	// 	send_info(pi,1);
	// 	send_info(pi,1);
	// 	send_info(pi,0);
	// 	send_info(pi,1);
	// 	//trailer
	// 	gpio_write(pi,27,0);
	// 	usleep(DELTA_T);
	// }
	// else{
	// 	recieveHandshake(pi);
	// }

	// pigpio_stop(pi);		
	// //free(sending);
	// //free(recieved);
	// return 0;
