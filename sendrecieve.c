#include <stdio.h>
#include <pigpiod_if2.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

//clock_t startTime = 0;
//clock_t totalTime = 0;
bool oneSent;
bool stopwatchStarted = false;
bool correctTime = false;
int DELTA_T = 1;

int main(){
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

	recieveHandshake(pi);

	pigpio_stop(pi);		
	//free(sending);
	//free(recieved);
	return 0;
}
/**
int sendHandshake(int pi){
	int primaryState = gpio_read(pi, 20);	
	int currentState = primaryState;

	printf("primary: %d\n", primaryState);

	//Wait for the reception
	clock_t starttime = clock();
	while(currentState == primaryState){
		int toggle = gpio_write(pi,27,0);
		sleep(1);
		toggle = gpio_write(pi,27,1);
		currentState = gpio_read(pi,20);
		clock_t endtime = clock();
		if((endtime-starttime)/CLOCKS_PER_SEC > 5){
			printf("Handshake failed");
			return 1;
		}
	}
	
	int test = gpio_read(pi, 20);
	printf("End: %d\n", test);
	printf("Handshake successfully sent");

}
*/
int testSend(pi){
	gpio_write(pi,27,1);
	gpio_write(pi,27,0);
}
void callBackfunc(int pi, unsigned user_gpio, unsigned level, uint32_t tick){
	printf("Success\n");
	gpio_write(pi, 27, 0);
	gpio_write(pi,27,1);
}

void fallingFunc(int pi, unsigned user_gpio, unsigned level, uint32_t tick){
	printf("1 sent\n");
	oneSent = true;
	if (correctTime){
		printf("1\n");
	}
	//printf("falling edge\n");
}

void risingFunc(int pi, unsigned user_gpio, unsigned level, uint32_t tick){
	printf("0 sent\n");
	if (oneSent){
		stopwatchStarted = true;
	}
	if (correctTime){
		printf("0\n");
	}
	//printf("rising edge\n");
}

int recieveHandshake(int pi){
	//int fallingEdge = callback(pi,20,1,callBackfunc);
	//int primaryState = gpio_read(pi, 20);
	//int currentState = primaryState;
	//printf("primary: %d\n", primaryState);
	
	int callbackFall = callback(pi,20,FALLING_EDGE,fallingFunc);
	int callbackRise = callback(pi,20,RISING_EDGE,risingFunc);
	
	
	while (!stopwatchStarted){
		
	}

	// start stopwatch
	clock_t starttime = clock();
	while(1){	
		clock_t currentTime = clock();
		
		if((currentTime-starttime)/CLOCKS_PER_SEC == DELTA_T){
			correctTime = true;
		}
		else{
			correctTime = false;	
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
	

	
}
int packageReader(int pi, int sent){
	int toggle = gpio_write(pi, 27, 0);
	toggle = gpio_write(pi,27,sent);
	int state = gpio_read(pi, 20);
	return state;

}
