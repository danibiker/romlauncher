#pragma once

#include <stdbool.h>

class Focus{
    public:
        Focus(){
        }
        
        ~Focus(){
        }
        static void *handle;
        static bool notFocused();
        static void setFocus();
};