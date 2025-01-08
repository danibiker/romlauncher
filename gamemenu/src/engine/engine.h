#pragma once


#include <allegro.h>

#include "utils/const/Constant.h"
#include "utils/io/traza.h"
#include "alpng.h"
#include "utils/io/cfgloader.h"
#include "utils/font/fonts.h"
#include "utils/io/joystick.h"
#include "utils/io/music.h"

class Engine{
    public:
        Engine(){};
        ~Engine(){};
        // Variable usada para la velocidad
        static volatile uint32_t gameTimeCounter;
        int isCloseRequested();
        bool isKeyUp();
        Joystick joystick;
        Music music;
        static void incGameTimeCounter();
    protected:
        int initEngine(CfgLoader &cfgLoader);
        void stopEngine();
    private:
        static volatile int closeRequested;
        static volatile int key_up;
        
        static void closeRequestedHandler(void);
        static void keypress_watcher(int scancode);
};

void Engine::stopEngine(){
    Fonts::exit();
    allegro_exit();
}

int Engine::isCloseRequested(){
    return closeRequested;
}

bool Engine::isKeyUp(){
    bool ret = key_up;
    //Now we reset the values
    if (key_up) {
        key_up = 0;
    }
    return ret;
}

// Función para controlar la velocidad
void Engine::incGameTimeCounter(){
    Constant::totalTicks++;
    gameTimeCounter++;
}
END_OF_FUNCTION(incGameTimeCounter)

void Engine::closeRequestedHandler(void){
    closeRequested = true;
}
END_OF_FUNCTION(closeRequestedHandler)

void Engine::keypress_watcher(int scancode){
    if (scancode & 0x80) {
        key_up = 1;
    } 
} END_OF_FUNCTION(keypress_watcher)


int Engine::initEngine(CfgLoader &cfgLoader){
    int colorDepth = 16;

    Traza::print(Traza::T_DEBUG, "Initializing allegro...");
    // Initializes the Allegro library.
    if (allegro_init() != 0) {
        Traza::print(Traza::T_ERROR, "Unable to initialize allegro");
        return 1;
    }

    LOCK_FUNCTION(closeRequestedHandler);
    set_close_button_callback(Engine::closeRequestedHandler);

    Traza::print(Traza::T_DEBUG, "Initializing timers...");
    // Installs the Allegro timer interrupt handler.
    install_timer();

    LOCK_VARIABLE(gameTimeCounter);
    LOCK_VARIABLE(Constant::totalTicks);
    LOCK_FUNCTION(incGameTimeCounter);
    install_int_ex(incGameTimeCounter, MSEC_TO_TIMER(maxFrameTime));

    Traza::print(Traza::T_DEBUG, "Initializing keyboard...");
    // Installs the Allegro keyboard interrupt handler.
    install_keyboard();
    LOCK_VARIABLE(key_up);
    keyboard_lowlevel_callback = keypress_watcher;

    Traza::print(Traza::T_DEBUG, "Initializing allegro png...");
    // Init allegro png library
    alpng_init();

    //Very slow
    //set_color_conversion(COLORCONV_KEEP_ALPHA);

    int card = GFX_AUTODETECT_WINDOWED;
    #if defined(WIN) || defined(UNIX)
        Constant::setExecMethod(launch_system);
        if (desktop_color_depth() != 0){
            colorDepth = desktop_color_depth();
        }
    #endif

    #ifdef DOS 
        card = GFX_AUTODETECT;
        Constant::setExecMethod(launch_batch);
    #endif

    int w = cfgLoader.getWidth(); 
    int h = cfgLoader.getHeight();
    if ( (w == 0 || h == 0)){
        if (get_desktop_resolution(&w, &h) == 0){
            card = GFX_AUTODETECT_FULLSCREEN;
        } else {
            Traza::print(Traza::T_ERROR, "Error getting desktop resolution falling back to 320x240");
            w = 320;
            h = 240;
        }
        cfgLoader.setWidth(w);
        cfgLoader.setHeight(h);
    }

    set_color_depth(colorDepth);

    Traza::print(Traza::T_DEBUG, "Setting GFX mode...");
    if (set_gfx_mode(card, w, h, 0, 0) != 0) {
        if (set_gfx_mode(GFX_SAFE, 320, 200, 0, 0) != 0) {
            snprintf(Traza::log_message, sizeof(Traza::log_message), "error trying set resolution %dx%d: \"%s\"", w, h, allegro_error);
            Traza::print(Traza::T_ERROR);
            // Switch to text mode, 320x200.
            set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
            allegro_message("Cannot set graphics mode:\r\n%s\r\n", allegro_error);
            readkey();
            return 1;
        }
    }

    //if (is_windowed_mode()) {
    //     /* Windowed mode stuff. */
    //} else {
    //    /* Fullscreen mode stuff. */
    //}  

    Traza::print(Traza::T_DEBUG, "Loading fonts...");
    Fonts::init();
    Fonts::initFonts(SCREEN_H / SCREENHDIV);

    Traza::print(Traza::T_DEBUG, "Setting palette...");
    set_palette(default_palette);

    /* Some one time initialisation code. Very slow*/
    //create_trans_table(&global_trans_table, desktop_palette, 128, 128, 128, NULL);

    joystick.init();

    Traza::print(Traza::T_DEBUG, "Installing sound...");
    if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL) != 0){
        snprintf(Traza::log_message, sizeof(Traza::log_message), "Error openning sound %s", allegro_error);
        Traza::print(Traza::T_ERROR);
    } 
    
    return 0;
}
