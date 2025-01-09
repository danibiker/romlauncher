#pragma once

#include <allegro.h>

class Screen{
    public:
        Screen(){};
        ~Screen(){
            Traza::print(Traza::T_DEBUG, "Deleting Screen...");
        };

        static int getScreenNumLines(){
            return SCREEN_H / font->height;
        };
};