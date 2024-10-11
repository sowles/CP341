#include <stdio.h>
#include <string.h>
#include <stdlib.h>
char* decimalToBinary(unsigned char decimal, char *eight)
{
    for(int i = 7; i >= 0; --i)
    {
        eight[7 - i] = ((decimal >> i) & 1) + '0';
    }
    eight[8] = '\0'; // null-terminate the string

    
}
void asciiToBinary(char *text, char *binary) { //exclude spaces (line25)
    char *eight = malloc(9);
    if (eight == NULL)
        exit(1);
    while(*text)
    {
        decimalToBinary(*text, eight);
        char *eight_ptr = eight;

        while(*eight_ptr)
            *binary++ = *eight_ptr++;
        // *binary++ = ' ';
        ++text;
    }
    *binary = '\0';
    // binary -= binaryLength;
    free(eight);
}
//     int i;
    
//     // Iterate through each bit from left to right (MSB to LSB)
//     for (i = 6; i >= 0; i--) {
//         // Use bitwise operations to extract the bit
//         int binary = (ch >> i) & 1;
//         printf("%d", binary);
//     }
// }

int main(void) {
    char text[101];
    char *binary;
    // int textLength;
    int binaryLength;

    printf("Enter a sentence (max 100 characters): "); //text101 limits size
    scanf("%100[^\n]", text);

    binaryLength = strlen(text) * 9;      // 8 binary digits + 1 space separator
    binary = malloc(binaryLength + 1);    // +1 to include null terminator
    if (binary == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    asciiToBinary(text, binary);
    printf("Your binary encoding is:\n%s\n", binary);

    free(binary);
    return 0;
    // char ASCII[] = "ABCDEFGHIJKLMNOPQRSTUVabcdefghijklmnopqrstuv0123456789";
    //find index of character--calculate binary--add to binary print out string
    // char ASCII;
    // printf("Enter a char: ");
    // scanf("%c", &ASCII);

    // printf("Binary representation of '%c' (ASCII): ", ASCII);
    // asciiToBinary(ASCII);
    // printf("\n");

    // return 0;
}


//to implement-- converting binary back to letters / sentences
// == https://www.c-programming-simple-steps.com/binary-to-text.html


// for dec to binary:

    // char *binary = malloc(65);
    // if (binary == NULL)
    //     exit(1);
    // binary += 64;
    // *binary ='\0';
    // if(decimal==0){
    //     --binary;
    //     *binary = decimal % 2 + '0';
    //     decimal = decimal / 2;

    // }
    // return binary;





    