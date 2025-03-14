#include <time.h>
#include <stdio.h>

char* nameFile()
{
    time_t currentTime;
    time(&currentTime);
    struct tm *localTime = localtime(&currentTime);
    int min = localTime->tm_min;
    int sec = localTime->tm_sec;
    static char timeNow[8];
    sprintf( timeNow , "%d%d.jpg", min, sec);
    return timeNow;
}