#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>


int main() {
    char *binary;
    char text[101] = "Hello, World!";
    int binaryLength = strlen(text) * 8; //8 bit ASCII chars
	binary =  malloc(binaryLength + 1);
    asciiToBinary(text, binary);
    printf(binary);
    printf(binaryToASCII(binary));

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