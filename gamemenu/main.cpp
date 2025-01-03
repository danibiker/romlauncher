/*
 * Copyright (C) 2016, Michiel Sikma <michiel@sikma.org>
 * MIT License
 */
#include <string>
#include <chrono>
#include <sys/time.h>
#ifdef UNIX
    #include <unistd.h>
#else 
    #include <process.h>
#endif

#include <allegro.h>

#include "alpng.h"
#include "utils/io/traza.h"
#include "utils/io/joystick.h"
#include "utils/so/soutils.h"
#include "utils/uiobjects/tilemap.h"
#include "utils/font/fonts.h"
#include "gamemenu.h"


using namespace std;
using namespace std::chrono;

string argv0;
volatile int closeRequested = false;
volatile int fps = 0;
volatile int internalfps = 0;
volatile int key_up;
// Variable usada para la velocidad
volatile uint32_t gameTimeCounter = 0;


bool limitFps = true;

void processKeys(ListMenu &, GameMenu &);

void closeRequestedHandler(void){
    closeRequested = true;
}
END_OF_FUNCTION(closeRequestedHandler)

// Función para controlar la velocidad
void incGameTimeCounter(){
    Constant::totalTicks++;
    gameTimeCounter++;
}
END_OF_FUNCTION(incGameTimeCounter)

void keypress_watcher(int scancode){
    if (scancode & 0x80) {
        key_up = 1;
    } 
} END_OF_FUNCTION(keypress_watcher)

void drawFps(BITMAP *video_page){
    ALFONT_FONT *fontsmall = Fonts::getFont(Fonts::FONTSMALL);
    Constant::drawText(video_page, fontsmall, std::to_string(fps).c_str(), 0, fontsmall->face_h , textColor, -1);
}

void updateScreen(TileMap &tileMap, ListMenu &menuData, GameMenu &gameMenu, bool keypress){
    //if (scrollBkg && !keypress){
    //    if (animateBkg) tileMap.draw(gameMenu.video_page);
    //    else clear_to_color(gameMenu.video_page, backgroundColor);
    //    gameMenu.refreshScreen(menuData);
    //    tileMap.speed = tileMap.speed + 0.2;
    //} else if (!scrollBkg && keypress){
    //    if (animateBkg) tileMap.draw(gameMenu.video_page);
    //    else clear_to_color(gameMenu.video_page, backgroundColor);
    //    gameMenu.refreshScreen(menuData);
    //} 

    if (menuData.animateBkg) tileMap.draw(gameMenu.video_page);
    else clear_to_color(gameMenu.video_page, backgroundColor);
    gameMenu.refreshScreen(menuData);

    static uint32_t lastTime = Constant::getTicks();
    if (Constant::getTicks() - lastTime > bkgFrameTimeTick && (lastTime = Constant::getTicks()) > 0){
        tileMap.speed++;
    }
}

bool isKeyUp(){
    bool ret = key_up;
    //Now we reset the values
    if (key_up) {
        key_up = 0;
    }
    return ret;
}

/**
 * 
 */
void processKeys(ListMenu &menuData, GameMenu &gameMenu){
    struct ListStatus menuBeforeExit;
    int retMenu = gameMenu.recoverGameMenuPos(menuData, menuBeforeExit);
    if (retMenu == 0){
        if (menuBeforeExit.layout != menuData.layout){
            menuData.setLayout(menuBeforeExit.layout, SCREEN_W, SCREEN_H);
        }
        menuData.animateBkg = menuBeforeExit.animateBkg;
    }
    gameMenu.loadEmuCfg(menuData);
    if (retMenu == 0 && menuData.maxLines == menuBeforeExit.maxLines){
        menuData.iniPos = menuBeforeExit.iniPos;
        menuData.endPos = menuBeforeExit.endPos;
        menuData.curPos = menuBeforeExit.curPos;
    }
    
    TileMap tileMap(9, 0, 16, 16);
    tileMap.load(Constant::getAppDir() + "/assets/art/bricks2.png");
    gameMenu.createMenuImages(menuData);

    menuData.keyUp = true;
    updateScreen(tileMap, menuData, gameMenu, false);

    bool exit = false;
    Sound sound;
    Joystick joystick;
    joystick.init();

    while (!exit && !closeRequested) {
        //If resizing detected
        //if (menuData.maxLines != menuData.getScreenNumLines() - 1){
        //    menuData.maxLines = menuData.getScreenNumLines() - 1;
        //    menuData.endPos = (int)menuData.getNumGames() > menuData.maxLines ? menuData.maxLines : menuData.getNumGames();
        //    //clear_to_color(gameMenu.video_page, backgroundColor);
        //    updateScreen(tileMap, menuData, gameMenu, true, animateBkg);
        //}

        gameTimeCounter = !limitFps ? 1 : gameTimeCounter;

        static uint32_t lastGameTick = Constant::getTicks();
        if (Constant::getTicks() - lastGameTick > bkgFrameTimeTick && (lastGameTick = Constant::getTicks()) > 0){
            gameMenu.gameTicks.ticks++;
        }

        if (gameTimeCounter){
            joystick.readJoystick();

            if ((keypressed() && readkey()) || joystick.isJoyPressed()){
                if (key[KEY_ESC] || (joystick.getButtonStat(joystick.J_SELECT) && joystick.getButtonStat(joystick.J_START))){
                    exit = true;
                } else if (key[KEY_ENTER] || joystick.getButtonStat(joystick.J_A)){
                    sound.play(SBTNLOAD, true);
                    gameMenu.launchProgram(menuData, argv0);
                } else {
                    if (key[KEY_UP] || joystick.getButtonStat(joystick.J_UP)){
                        menuData.prevPos();
                        sound.play(SBTNCLICK);
                        //updateScreen(tileMap, menuData, gameMenu, true, animateBkg);
                    }
                    if (key[KEY_DOWN] || joystick.getButtonStat(joystick.J_DOWN)){
                        menuData.nextPos();
                        sound.play(SBTNCLICK);
                    }
                    if (key[KEY_RIGHT] || joystick.getButtonStat(joystick.J_RIGHT)){
                        menuData.nextPage();
                        sound.play(SBTNCLICK);
                    }
                    if (key[KEY_LEFT] || joystick.getButtonStat(joystick.J_LEFT)){
                        menuData.prevPage();
                        sound.play(SBTNCLICK);
                    }
                    if ((key[KEY_TAB] && !(key_shifts & KB_SHIFT_FLAG)) || joystick.getButtonStat(joystick.J_RB)){
                        sound.play(SBTNCLICK);
                        gameMenu.showMessage("Refreshing gamelist...");
                        gameMenu.getNextCfgEmu();
                        gameMenu.loadEmuCfg(menuData);
                    }
                    if ( (key[KEY_TAB] && (key_shifts & KB_SHIFT_FLAG)) || joystick.getButtonStat(joystick.J_LB)){
                        sound.play(SBTNCLICK);
                        gameMenu.showMessage("Refreshing gamelist...");
                        gameMenu.getPrevCfgEmu();
                        gameMenu.loadEmuCfg(menuData);
                    } 
                    if (key[KEY_F2]){
                        sound.play(SBTNCLICK);
                        gameMenu.showMessage("Changing layout...");
                        menuData.setLayout((menuData.layout + 1) % 3, SCREEN_W, SCREEN_H);
                        menuData.animateBkg = menuData.layout == LAYBOXES;
                    } else if (key[KEY_F3]){
                        sound.play(SBTNCLICK);
                        tileMap.findTile((tileMap.tileX + 1) % 27, 0);
                    } else if (key[KEY_F4]){
                        sound.play(SBTNCLICK);
                        menuData.animateBkg = !menuData.animateBkg;
                    } else if (key[KEY_F5]){
                        sound.play(SBTNCLICK);
                        limitFps = !limitFps;
                    }           
                }
            }
            menuData.keyUp = isKeyUp() || joystick.isJoyReleased();
            updateScreen(tileMap, menuData, gameMenu, false);
            if (gameTimeCounter > 0) 
                gameTimeCounter--;
        
            if (gameMenu.isDebug()) drawFps(gameMenu.video_page);
            /* Blit the video buffer on the screen. */
            blit(gameMenu.video_page, screen, 0, 0, 0, 0, gameMenu.video_page->w, gameMenu.video_page->h);

            static uint32_t lastTime = Constant::getTicks();
            if (Constant::getTicks() - lastTime >= 1000){
                fps = internalfps;
                internalfps = 0;
                lastTime = Constant::getTicks();
            } else {
                internalfps++;
            }
        }else{
            rest(1);  
        }

        //if (exit || closeRequested){
        //    sprintf(Traza::log_message, "closeRequested: %d; exit: %d; joy: %d", 
        //        closeRequested, exit, (joystick.getButtonStat(joystick.J_SELECT) && joystick.getButtonStat(joystick.J_START)));
        //    Traza::print();
        //}
    }
}

/**
 * 
 */
int main(int argc, char *argv[]){
    int colorDepth = 16;
    argv0 = argv[0];
    string rutaApp = SOUtils::iniciarSistema(argv);
    CfgLoader cfgLoader;

    Traza::open();
    if (cfgLoader.isDebug()){
        Traza::level = Traza::T_DEBUG;
    }

    Traza::print(Traza::T_DEBUG, "Initializing allegro...");
    // Initializes the Allegro library.
    if (allegro_init() != 0) {
        Traza::print(Traza::T_ERROR, "Unable to initialize allegro");
        return 1;
    }

    LOCK_FUNCTION(closeRequestedHandler);
    set_close_button_callback(closeRequestedHandler);

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
        Constant::setExecMethod(launch_spawn);
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
            sprintf(Traza::log_message, "error trying set resolution %dx%d: \"%s\"", w, h, allegro_error);
            Traza::print(Traza::T_ERROR);
            // Switch to text mode, 320x200.
            set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
            allegro_message("Cannot set graphics mode:\r\n%s\r\n", allegro_error);
            readkey();
            return 1;
        }
    }

    Traza::print(Traza::T_DEBUG, "Loading emulators config...");
    GameMenu gameMenu(&cfgLoader);

    Traza::print(Traza::T_DEBUG, "Creating menu data...");
    ListMenu listMenu(SCREEN_W, SCREEN_H);

    Traza::print(Traza::T_DEBUG, "Loading fonts...");
    Fonts::init();
    Fonts::initFonts(SCREEN_H / SCREENHDIV);
    ALFONT_FONT *fontsmall = Fonts::getFont(Fonts::FONTSMALL);

    Traza::print(Traza::T_DEBUG, "Setting layout...");
    listMenu.setLayout(LAYBOXES, SCREEN_W, SCREEN_H);

    //if (is_windowed_mode()) {
    //     /* Windowed mode stuff. */
    //} else {
    //    /* Fullscreen mode stuff. */
    //}         

    Traza::print(Traza::T_DEBUG, "Setting palette...");
    set_palette(default_palette);
    Traza::print(Traza::T_DEBUG, "clearing screen...");
    clear_bitmap(screen);
    
    //set_palette(desktop_palette);
    textColor = makecol(255, 255, 255);
    backgroundColor = makecol(0, 0, 0);

    if (cfgLoader.isDebug()) 
        Constant::drawTextCentre(screen, fontsmall, "Creating bitmap buffer...", SCREEN_W / 2, SCREEN_H / 2, textColor, backgroundColor);

    Traza::print(Traza::T_DEBUG, "Creating bitmap buffer...");
    if (!gameMenu.initDblBuffer()){
        Traza::print(Traza::T_ERROR, "Could not create bitmap");
        return 1;
    }

    Traza::print(Traza::T_DEBUG, "Installing sound...");
    if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL) != 0){
        sprintf(Traza::log_message, "Error openning sound %s", allegro_error);
        Traza::print(Traza::T_ERROR);
    }
    
    clear(screen);
    /* Some one time initialisation code. */
    //create_trans_table(&global_trans_table, desktop_palette, 128, 128, 128, NULL);
    Traza::print(Traza::T_DEBUG, "Loading games...");
    Constant::drawTextCentre(screen, fontsmall, "Loading games...", SCREEN_W / 2, SCREEN_H / 2, textColor, -1);
    
    if (cfgLoader.configMain.debug){
        string appDir = argv[0];
        clear_to_color(screen, backgroundColor);
        string msg = "argv[0]:" + appDir;
        Constant::drawTextCentre(screen, fontsmall, msg.c_str(), SCREEN_W / 2, SCREEN_H / 2, textColor, -1);
        Constant::drawTextCentre(screen, fontsmall, Constant::getAppDir().c_str(), SCREEN_W / 2, SCREEN_H / 2 + fontsmall->face_h + 3, textColor, -1);
        readkey();
    }

    Traza::print(Traza::T_DEBUG, "Processing keys...");
    processKeys(listMenu, gameMenu);
    remove_keyboard();
    remove_timer();
    Fonts::exit();
    allegro_exit();
    Traza::close();
    return 0;
}
END_OF_MAIN()
