#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "libjpeg.lib")  // Link to libjpeg (assuming it's installed)


void CaptureScreen(const char* filename, int quality, char* logger);

int SaveBitmapToJPEG(HBITMAP hBitmap, const char* filename, int quality);
