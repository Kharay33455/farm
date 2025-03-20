#ifndef SHOOTER_H
#define SHOOTER_H

#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "libjpeg.lib")  // Link to libjpeg (assuming it's installed)

#define MAX_THREADS 50

// data must contain a const char *filename and a char logger[]
typedef struct DataToSend
{
    const char* filename;
    char logger[100];
} DataToSend, *PDataToSend;

extern DataToSend listOfPending[MAX_THREADS];  // This is a main counter in our program.
extern HANDLE threadHandles[MAX_THREADS];

void CaptureScreen(const char* filename, int quality, char* logger);

#endif