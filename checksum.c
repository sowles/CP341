#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sender_calculate_parity(char arr[], int totalCount);
void receiver_calculate_parity(char arr[], char recarr[], int totalCount);
void print_array(char arr[], int size);
int main() {
    int tcount = 0;
    char *sendArr = malloc(12); 
    //first bit ignored to set
    sendArr[0] = 0;
    sendArr[1] = 0;
    sendArr[2] = 1;
    sendArr[3] = 0;
    sendArr[4] = 1;
    sendArr[5] = 1;
    sendArr[6] = 1;
    sendArr[7] = 1;
    sendArr[8] = 0;
    sendArr[9] = 1;
    sendArr[10] = 1;

    char *recArr = malloc(12); 
    //first bit ignored to set
    recArr[0] = 0;
    recArr[1] = 0;
    recArr[2] = 1;
    recArr[3] = 0;
    recArr[4] = 1;
    recArr[5] = 0;
    recArr[6] = 1;
    recArr[7] = 1;
    recArr[8] = 0;
    recArr[9] = 1;
    recArr[10] = 1;
    sender_calculate_parity(sendArr, tcount);
    receiver_calculate_parity(sendArr, recArr, tcount);
    free(sendArr);
    free(recArr);
    return 0;
}

void sender_calculate_parity(char arr[], int totalCount) {
    for (int i = 1; i < 8; i++) {
        if (arr[i] == 1) {
            totalCount += 1;
        }
    }
    if (totalCount % 2 == 0) {
        arr[9] = 0;
    } else {

        arr[9] = 1;
    }
}

void receiver_calculate_parity(char arr[], char recarr[], int totalCount) {
    totalCount = 0;
    for (int i = 1; i < 8; i++) {
        if (arr[i] == 1) {
            totalCount += 1;
        }
    }
    if (totalCount % 2 == 0) {
        arr[9] = 0;
    } else {

        arr[9] = 1;
    }

    if(){
        printf("No Errors!");
    }else{
        printf("Error: parity bit does not match!");
    }
}