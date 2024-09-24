#include <stdio.h>
#include <pigpiod_if2.h>
#include <time.h>

//KNOWN PINS
//Port 1 tx: 27
//Port 2 tx: 25
//Port 3 tx: 23
//Port 4 tx: 21

int main(){
    int i;
    int toggle;
    int pi = pigpio_start(NULL,NULL);
//     for(i=0,i<54,i++){
//         int setmode = set_mode(pi,i,PI_INPUT);
//         int firsttest = gpio_read(pi,i);
//         int toggle = gpio_write(pi,i,1);
//         printf("%d\n",i);
//         sleep(1);
//     }
//     pigpio_stop(pi)
//     return toggle; 
// }
double bigTotal = 0;
for(i=0;i<10;i++){

        int toggle = gpio_write(pi,27,1);
	clock_t start = clock();
       // sleep(1);
        gpio_write(pi,27,0);
	clock_t finish = clock();
	double total = (double)(finish-start)/CLOCKS_PER_SEC;
       // sleep(1);
       bigTotal += total;
}
printf("%f\n", bigTotal/10);
pigpio_stop(pi);
return toggle; 
}
//# get rid of `1 in mounting directions
