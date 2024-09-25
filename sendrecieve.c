#include <stdio.h>
#include <pigpiod_if2.h>
#include <unistd.h>
#include <time.h>

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

int recieveHandshake(int pi){
	int primaryState = gpio_read(pi, 20);
	int currentState = primaryState;
	printf("primary: %d\n", primaryState);

	clock_t starttime = clock();
	while(currentState == primaryState){
		currentState = gpio_read(pi,20);
		int toggle = gpio_write(pi,27,0);
		toggle = gpio_write(pi,27,1);
		clock_t endtime = clock();
		if((endtime-starttime)/CLOCKS_PER_SEC > 10){
			return 1;
		
		}
	}

	int toggle = gpio_write(pi,27,0);
	toggle = gpio_write(pi,27,1);
	
	
	printf("Handshake recieved");

	
}
int packageReader(int pi, int sent){
	int toggle = gpio_write(pi, 27, 0);
	toggle = gpio_write(pi,27,sent);
	int state = gpio_read(pi, 20);
	return state;

}
