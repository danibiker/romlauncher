#pragma once

#include <allegro.h>

class Screen{
    public:
        Screen();
        ~Screen();

        static int getScreenNumLines(){
            return SCREEN_H / font->height;
        };
};