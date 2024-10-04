#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sender_calculate_parity(char arr[], int totalCount);
void receiver_calculate_parity(char arr[], char recarr[], int totalCount);
void print_array(char arr[], int size);
void ec_encode(int * sendArr, int * retArr);
void ec_decode(int * decArr, int * decRetArr);
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
    decArr[0] = 0;
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
    
    deRetArr[0] = 1;
    deRetArr[1] = 1;
    deRetArr[2] = 1;
    deRetArr[3] = 1;
    deRetArr[4] = 1;
    deRetArr[5] = 1;
    deRetArr[6] = 1;
    deRetArr[7] = 1;

   ec_encode(sendArr, retArr);
   ec_decode(decArr, deRetArr);
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

   //recalculate
    decRetArr[0] = decArr[0];
    decRetArr[1] = decArr[1];
    decRetArr[2] = decArr[2];
    decRetArr[3] = decArr[3];

    decRetArr[4] = decArr[8];
    decRetArr[5] = decArr[9];
    decRetArr[6] = decArr[10];
    decRetArr[7] = decArr[11];

    int p1 = (decRetArr[0] + decRetArr[2] + decRetArr[3]) %2;
    int p2 = (decRetArr[0] + decRetArr[1] + decRetArr[3]) %2;
    int p3 = (decRetArr[1] + decRetArr[2] + decRetArr[3]) %2;
    int p4 = (decRetArr[0] + decRetArr[1] + decRetArr[2] + decRetArr[3] + p1 + p2 +p3) %2;

    int p5 = (decRetArr[4] + decRetArr[6] + decRetArr[7]) %2;
    int p6 = (decRetArr[4] + decRetArr[5] + decRetArr[7]) %2;
    int p7 = (decRetArr[5] + decRetArr[6] + decRetArr[7]) %2;
    int p8 = (decRetArr[4] + decRetArr[5] + decRetArr[6] + decRetArr[7] + p1 + p2 +p3) %2;

    int c_0 = p1 == decArr[4];
    int c_1 = p2 == decArr[5];
    int c_2 = p3 == decArr[6];
    int c_3 = p4 == decArr[7];
    int c_4 = p5 == decArr[12];
    int c_5 = p6 == decArr[13];
    int c_6 = p7 == decArr[14];
    int c_7 = p8 == decArr[15];


    if( c_0&&c_1&&c_2&&c_3&&c_4&&c_5&&c_6&&c_7) {
         printf("Decoded with no errors!");

    }else if(!c_0&&!c_1&&!c_3){
        //flip the bit
        if(decRetArr[0] == 0){
            decRetArr[0] = 1;
        }else{
            decRetArr[0] = 0;
        }
    }else if(!c_0&&!c_1&&c_2&&c_3){
        if(decRetArr[3] == 0){
            decRetArr[3] = 1;
        }else{
            decRetArr[3] = 0;
        }
    }else if(!c_1&&!c_2&&c_3){
        if(decRetArr[1] == 0){
            decRetArr[1] = 1;
        }else{
            decRetArr[1] = 0;
        }
    }else if(!c_0&&!c_2&&c_2&&c_3){
        if(decRetArr[2] == 0){
            decRetArr[2] = 1;
        }else{
            decRetArr[2] = 0;
        }
    }else if(!c_0){
        if(decArr[4] == 0){
            decArr[4] = 1;
        }else{
            decArr[4] = 0;
        }
    }else if(!c_1){
        if(decArr[5] == 0){
            decArr[5] = 1;
        }else{
            decArr[5] = 0;
        }
    }else if(!c_2){
        if(decArr[6] == 0){
            decArr[6] = 1;
        }else{
            decArr[6] = 0;
        }
    }else if(!c_3){
        if(decArr[7] == 0){
            decArr[7] = 1;
        }else{
            decArr[7] = 0;
        }
    }/* SECOND HALF */
    else if(!c_4&&!c_5&&!c_7){
        //flip the bit
        if(decRetArr[4] == 0){
            decRetArr[4] = 1;
        }else{
            decRetArr[4] = 0;
        }
    }else if(!c_4&&!c_5&&c_6&&c_7){
        if(decRetArr[7] == 0){
            decRetArr[7] = 1;
        }else{
            decRetArr[7] = 0;
        }
    }else if(!c_5&&!c_6&&c_7){
        if(decRetArr[5] == 0){
            decRetArr[5] = 1;
        }else{
            decRetArr[5] = 0;
        }
    }else if(!c_4&&!c_6&&c_2&&c_7){
        if(decRetArr[6] == 0){
            decRetArr[6] = 1;
        }else{
            decRetArr[6] = 0;
        }
    }else if(!c_4){
        if(decArr[12] == 0){
            decArr[12] = 1;
        }else{
            decArr[12] = 0;
        }
    }else if(!c_5){
        if(decArr[13] == 0){
            decArr[13] = 1;
        }else{
            decArr[13] = 0;
        }
    }else if(!c_6){
        if(decArr[14] == 0){
            decArr[14] = 1;
        }else{
            decArr[14] = 0;
        }
    }else if(!c_7){
        if(decArr[15] == 0){
            decArr[15] = 1;
        }else{
            decArr[15] = 0;
        }
    }

    
    printf("\n");
}