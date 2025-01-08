#include "focus.h"
#include <windows.h>

void * Focus::handle;

bool Focus::notFocused(){
    // Get the handle of the Allegro window
    //HWND hwnd = GetForegroundWindow(); 
    // Check if the current window is the foreground window 
    return ((HWND) handle) == GetForegroundWindow();
}

void Focus::setFocus(){
    HWND hwnd = GetActiveWindow(); 
    // Get the handle of the Allegro window
    handle = hwnd; 
}