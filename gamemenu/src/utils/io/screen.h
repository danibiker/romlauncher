#pragma once

#include <allegro.h>

class Screen{
    public:
        Screen(){};
        ~Screen(){
            cout << "deleting Screen" << endl;
        };

        static int getScreenNumLines(){
            return SCREEN_H / font->height;
        };
};