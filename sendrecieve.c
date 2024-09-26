#include <stdio.h>
#include <pigpiod_if2.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>


uint32_t startTime = 0;
uint32_t calc_t = 0;
uint32_t tickOfLastBit;
bool oneSent;
bool firstTwoBits = true;
#define DELTA_T 1000

int main(){
	int SEND_MODE = 0;
	int RECIEVE_MODE = 1;
	//KNOWN PINS
	//Port 1 tx: 27
	//Port 1 rx: 26
	//Port 2 tx: 25
	//Port 2 rx: 24
	//Port 3 tx: 23
	//Port 3 rx: 22
	//Port 4 tx: 21
	//Port 4 rx: 20
	int clockrate = 1000;
	int pi = pigpio_start(NULL,NULL);
	
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
	gpio_write(pi,27,0);	
	gpio_write(pi,25,0);	
	gpio_write(pi,23,0);	
	gpio_write(pi,21,0);	
	

	// set mode here
	int mode = RECIEVE_MODE;
	if (mode == SEND_MODE){
		send_info(pi,1);
	}
	else{
		recieveHandshake(pi);
	}

	pigpio_stop(pi);		
	//free(sending);
	//free(recieved);
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

void callBackfunc(int pi, unsigned user_gpio, unsigned level, uint32_t tick){
	printf("Success\n");
	gpio_write(pi, 27, 0);
	gpio_write(pi,27,1);
}

void fallingFunc(int pi, unsigned user_gpio, unsigned level, uint32_t tick){
//	printf("1 sent\n");
	tickOfLastBit = tick;
	oneSent = true;
	startTime = tick;
	
	if(withinBuffer(tick)){
		tickOfLastBit = tick;
		printf("correct 1\n");	
	}

	//printf("falling edge\n");
	//printf("falling edge\n");
}

void risingFunc(int pi, unsigned user_gpio, unsigned level, uint32_t tick){
//	printf("0 sent\n");
	if (oneSent && firstTwoBits){
		calc_t = (tick-startTime);
		firstTwoBits = false;
	}
	if(withinBuffer(tick)){
		tickOfLastBit = tick;
		printf("correct 0\n");	
	}
}

int withinBuffer(uint32_t tick){
	//30 is the buffer we set
	if((tick < tickOfLastBit+30) && (tick > tickOfLastBit-30)){
		return 1;
	}else{
		return 0;
	}
}

int recieveHandshake(int pi){
	//int fallingEdge = callback(pi,20,1,callBackfunc);
	//int primaryState = gpio_read(pi, 20);
	//int currentState = primaryState;
	//printf("primary: %d\n", primaryState);
	int callbackFall = callback(pi,20,FALLING_EDGE,fallingFunc);
	int callbackRise = callback(pi,20,RISING_EDGE,risingFunc);
	
	while(1){	

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
	

	
}
int packageReader(int pi, int sent){
	int toggle = gpio_write(pi, 27, 0);
	toggle = gpio_write(pi,27,sent);
	int state = gpio_read(pi, 20);
	return state;

}
