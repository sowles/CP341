#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sender_calculate_parity(char arr[], int totalCount);
void receiver_calculate_parity(char arr[], char recarr[], int totalCount);
void print_array(char arr[], int size);
void ec_encode(int * sendArr, int * retArr);
void ec_decode(int * decArr, int * decRetArr);
void bitFlip(int * arr, int i);
int main() {
    int tcount = 0;
    int *sendArr = malloc(sizeof(int)*8); 
    //first bit ignored to set
    
    sendArr[0] = 0;
    sendArr[1] = 1;
    sendArr[2] = 1;
    sendArr[3] = 0;
    sendArr[4] = 0;
    sendArr[5] = 0;
    sendArr[6] = 1;
    sendArr[7] = 0;

    int *retArr = malloc(sizeof(int)*16); 
    //first bit ignored to set
    retArr[0] = 0;
    retArr[1] = 0;
    retArr[2] = 0;
    retArr[3] = 0;
    retArr[4] = 0;
    retArr[5] = 0;
    retArr[6] = 0;
    retArr[7] = 0;
    retArr[8] = 0;
    retArr[9] = 0;
    retArr[10] = 0;
    retArr[11] = 0;
    retArr[12] = 0;
    retArr[13] = 0;
    retArr[14] = 0;
    retArr[15] = 0;

    int *decArr = malloc(sizeof(int)*16); 
    //first bit ignored to set    0110110000101011
    decArr[0] = 1;
    decArr[1] = 1;
    decArr[2] = 1;
    decArr[3] = 0;
    decArr[4] = 1;
    decArr[5] = 1;
    decArr[6] = 0;
    decArr[7] = 0;
    decArr[8] = 0;
    decArr[9] = 0;
    decArr[10] = 1;
    decArr[11] = 0;
    decArr[12] = 1;
    decArr[13] = 0;
    decArr[14] = 1;
    decArr[15] = 1;


    int *deRetArr = malloc(sizeof(int)*8); 
    //first bit ignored to set
    
    deRetArr[0] = 0;
    deRetArr[1] = 0;
    deRetArr[2] = 0;
    deRetArr[3] = 0;
    deRetArr[4] = 0;
    deRetArr[5] = 0;
    deRetArr[6] = 0;
    deRetArr[7] = 0;

   //ec_encode(sendArr, retArr);
   ec_decode(decArr, deRetArr);
    // for (int i = 0; i < 16; i++)
    // {
    //     printf("%d", deRetArr[i]);
    
    // }   
    // printf("\n");

    free(sendArr);
    free(deRetArr);
    free(retArr);
    free(decArr);
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

    // if(){
    //     printf("No Errors!");
    // }else{
    //     printf("Error: parity bit does not match!");
    // }
}

void ec_encode(int * sendArr, int * retArr){

   
    retArr[4] = (sendArr[0] + sendArr[2] + sendArr[3]) %2;
    retArr[5] = (sendArr[0] + sendArr[1] + sendArr[3]) %2;
    retArr[6] = (sendArr[1] + sendArr[2] + sendArr[3]) %2;
    retArr[7] = (sendArr[0] + sendArr[1] + sendArr[2] + sendArr[3] + retArr[8] + retArr[9] + retArr[10]) %2;

    retArr[12] = (sendArr[4] + sendArr[6] + sendArr[7]) %2;
    retArr[13] = (sendArr[4] + sendArr[5] + sendArr[7]) %2;
    retArr[14] = (sendArr[5] + sendArr[6] + sendArr[7]) %2;
    retArr[15] = (sendArr[4] + sendArr[5] + sendArr[6] + sendArr[7] + retArr[12] + retArr[13] + retArr[14]) %2;

    for (int i = 0; i <4; i++){
        retArr[i] = sendArr[i];
    }
    
    for (int i = 0; i <4; i++){
        retArr[i+8] = sendArr[i+4];
    }
    
    for (int i = 0; i < 16; i++)
    {
        printf("%d", retArr[i]);
    
    }   
    printf("\n");
}

void ec_decode(int * decArr, int * decRetArr){

    int p1 = (decArr[0] + decArr[2] + decArr[3]) %2;
    int p2 = (decArr[0] + decArr[1] + decArr[3]) %2;
    int p3 = (decArr[1] + decArr[2] + decArr[3]) %2;
    int p4 = (decArr[0] + decArr[1] + decArr[2] + decArr[3] + p1 + p2 +p3) %2;

    int p5 = (decArr[8] + decArr[10] +decArr[11]) %2;
    int p6 = (decArr[8] + decArr[9] + decArr[11]) %2;
    int p7 = (decArr[9] + decArr[10] + decArr[11]) %2;
    int p8 = (decArr[8] + decArr[9] + decArr[10] + decArr[11] + p1 + p2 +p3) %2;
    
    int c_0 = p1 == decArr[4]; //boolints: 1 true 0 false 
    int c_1 = p2 == decArr[5];
    int c_2 = p3 == decArr[6];
    int c_3 = p4 == decArr[7];
    int c_4 = p5 == decArr[12];
    int c_5 = p6 == decArr[13];
    int c_6 = p7 == decArr[14];
    int c_7 = p8 == decArr[15];


    if(c_0&&c_1&&c_2&&c_3&&c_4&&c_5&&c_6&&c_7) {
         printf("Decoded with no errors!!!");

    } else if (c_3 || c_7){
        printf("Two errors!\n");
    } else if(!c_0&&!c_1&&c_2&&c_3){
        bitFlip(decArr, 3);
    }else if(!c_0&&!c_1&&!c_3){
        //flip the bit
        bitFlip(decArr, 0);
    }else if(!c_1&&!c_2&&c_3){
        bitFlip(decArr, 1);
    }else if(!c_0&&!c_2&&c_3){
        bitFlip(decArr, 2);
    }else if(!c_0){
        bitFlip(decArr, 4);
    }else if(!c_1){
        bitFlip(decArr, 5);
    }else if(!c_2){
        bitFlip(decArr, 6);
    }else if(!c_3){
        bitFlip(decArr, 7);
    }/* SECOND HALF */
    else if(!c_0&&!c_1&&c_2&&c_3){
        bitFlip(decArr, 11);
    }else if(!c_0&&!c_1&&!c_3){
        //flip the bit
        bitFlip(decArr, 8);
    }else if(!c_1&&!c_2&&c_3){
        bitFlip(decArr, 9);
    }else if(!c_0&&!c_2&&c_3){
        bitFlip(decArr, 10);
    }else if(!c_0){
        bitFlip(decArr, 12);
    }else if(!c_1){
        bitFlip(decArr, 13);
    }else if(!c_2){
        bitFlip(decArr, 14);
    }else if(!c_3){
        bitFlip(decArr, 15);
    
    }
    //recalculate
    decRetArr[0] = decArr[0];
    decRetArr[1] = decArr[1];
    decRetArr[2] = decArr[2];
    decRetArr[3] = decArr[3];

    decRetArr[4] = decArr[8];
    decRetArr[5] = decArr[9];
    decRetArr[6] = decArr[10];
    decRetArr[7] = decArr[11];

    
    printf("\n");
}

void bitFlip(int * arr, int i){
    if(arr[i] == 0){
            arr[i] = 1;
        }else{
            arr[i] = 0;
        }
}
