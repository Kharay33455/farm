#ifndef AUXF_H
#define AUXF_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* nameFile()
{
    char acceptables[26] = {
        'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 
        'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'Z', 'X', 'C', 'V', 'B', 'N', 'M'
    };

    int fileNameLen = 10;
    char tempFileName[fileNameLen + 1];
    
    srand(time(NULL));

    for(int i = 0; i < fileNameLen; i++)
    {
        int randomNumber = rand() % 26;

        tempFileName[i] = acceptables[randomNumber];
    }
    tempFileName[fileNameLen] = '\0';   // terminate

    char fmt[] = ".jpg";
    char* fileNameToReturn = (char*)malloc(fileNameLen + strlen(fmt) + 1);  // +1 for null terminator
    if(fileNameToReturn == NULL)
    {
        printf("Could not allocate memory");
        return NULL;
    }

    sprintf(fileNameToReturn, "%s%s", tempFileName, fmt);
    
    return fileNameToReturn;
}

#endif