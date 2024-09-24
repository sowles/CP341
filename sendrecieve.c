#include <stdio.h>
#include <pigpiod_if2.h>
#include <unistd.h>

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
	int pi = pigpio_start(NULL,NULL);
	//for (int i = 0; i < 54; i++){
	//	toggle = gpio_write(pi,27,0);
	//	int state1 = gpio_read(pi, i);

	//	toggle = gpio_write(pi,27,1);
	//	int state2 = gpio_read(pi, i);
	//	printf("PIN %d: %d - %d\n", i, state1, state2);
	//	toggle = gpio_write(pi,27,0);
	//}
	
	int *sending;
	int *recieved;
	sending = (int*) malloc(sizeof(int)*4);
	sending[0] = 1;	
	sending[1] = 1;
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
	
	

		
	//int pi = pigpio_start(NULL,NULL);
		
	//int setmode = set_mode(pi,24,PI_OUTPUT);
	//int firsttest = gpio_read(pi,24);
	//int toggle = gpio_write(pi,24,1);
	//int secondtest = gpio_read(pi,24);
	//printf("%d\n",firsttest);
	//printf("%d\n",secondtest);
	pigpio_stop(pi);

	free(sending);
	free(recieved);
	return 0;

}

int packageReader(int pi, int sent){
	int toggle = gpio_write(pi, 27, 0);
	toggle = gpio_write(pi,27,sent);
	int state = gpio_read(pi, 20);
	return state;

}
