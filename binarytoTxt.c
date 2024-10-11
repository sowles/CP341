int main(void)
{
    char binary[559];
    char *text;
    int binaryLength, symbolCount;
    
    scanf("%558[^\n]s", binary);
    binaryLength = strlen(binary);
    symbolCount = binaryLength / 8 + 1;
    text = malloc(symbolCount + 1);
        
    binaryToText(binary, binaryLength, text, symbolCount);
    printf("The result text is: %s\n", text);
    
    free(text);
    
    return 0;
}