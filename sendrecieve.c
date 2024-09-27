#include <stdio.h>
#include <pigpiod_if2.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>


uint32_t startTick = 0;
uint32_t STARTTEST = 0;
int count = 0;
#define DELTA_T 10000

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

	// set mode here
	int mode = RECIEVE_MODE;
	if (mode == SEND_MODE){
		//header
		gpio_write(pi,27,1);
		usleep(DELTA_T);
		//data
		send_info(pi,1);
		send_info(pi,1);
		send_info(pi,0);
		send_info(pi,1);
		//trailer
		gpio_write(pi,27,0);
		usleep(DELTA_T);
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

bool withinBuffer(uint32_t tick, int edgeDirection){
	//5000 is the buffer we set
	int buff = 5000;
	uint32_t correctTick = (startTick + (2* DELTA_T));	
	int diff = correctTick - tick;	
//	printf("correctTick: %u, tick: %u, diff: %d\n", correctTick, tick, diff);
	if((tick < (correctTick + buff)) && (tick > (correctTick-buff))){
		printf("Within range: %d\n", edgeDirection);
		//printf("Tick within range. \n");
		
		startTick = tick;
		return true;
	}else{
		return false;
	}
}
void fallingFunc(int pi, unsigned user_gpio, unsigned level, uint32_t tick){
	//printf("Down - tick: %u\n", (tick-STARTTEST));
	//printf("Down\n");
	// disregard first (and eventually last) edge 
		if (withinBuffer(tick, 1)){
			startTick = tick;
		//	printf("Read falling: %u\n", level);
			
		//	printf("StartTick set: %u", tick);
			count += 1;
		}


	}
void risingFunc(int pi, unsigned user_gpio, unsigned level, uint32_t tick){
	//printf("Up\n");
	
	//printf("Up - tick: %u\n", (tick-STARTTEST));
	// disregard first (and eventually last) edge
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


int recieveHandshake(int pi){
	
	callback(pi,20,FALLING_EDGE,fallingFunc);
	callback(pi,20,RISING_EDGE,risingFunc);
	
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
