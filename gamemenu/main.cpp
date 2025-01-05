#include <string>
#include <chrono>
#include <sys/time.h>
#ifdef UNIX
    #include <unistd.h>
#else 
    #include <process.h>
#endif

#include "engine/engine.h"

#include <allegro.h>

#include "alpng.h"
#include "utils/io/traza.h"
#include "utils/io/joystick.h"
#include "utils/so/soutils.h"
#include "utils/uiobjects/tilemap.h"
#include "utils/font/fonts.h"
#include "gamemenu.h"

using namespace std;

string argv0;
volatile int fps = 0;
volatile int internalfps = 0;
bool limitFps = true;

const char Constant::FILE_SEPARATOR_UNIX = '/';
const string Constant::MAME_SYS_ID = "75";
const string Constant::WHITESPACE = " \n\r\t";

#if defined(WIN) || defined(DOS)
    char Constant::FILE_SEPARATOR = 0x5C; //Separador de directorios para win32
#else
    char Constant::FILE_SEPARATOR = Constant::FILE_SEPARATOR_UNIX; //Separador de directorios para unix
#endif

char Constant::tempFileSep[2] = {Constant::FILE_SEPARATOR,'\0'};
int Constant::Constant::backgroundColor;
int Constant::Constant::textColor;
string Constant::appDir = "";
COLOR_MAP Constant::global_trans_table;
volatile uint32_t Constant::totalTicks = 0;
int Constant::EXEC_METHOD = launch_batch;
//uint32_t Joystick::last = 0;
ALFONT_FONT * Fonts::vFonts[2] = {NULL, NULL};
volatile int Engine::closeRequested = 0;
volatile uint32_t Engine::gameTimeCounter = 0;
volatile int Engine::key_up = 0;
FILE * Traza::fp = NULL; // separate definition
char Traza::log_message[256];
int Traza::level = Traza::T_ERROR;
const string CfgLoader::CONFIGFILE = "gmenu.cfg";

void processKeys(ListMenu &, GameMenu &);

void drawFps(BITMAP *video_page){
    ALFONT_FONT *fontsmall = Fonts::getFont(Fonts::FONTSMALL);
    Constant::drawText(video_page, fontsmall, std::to_string(fps).c_str(), 0, fontsmall->face_h , Constant::textColor, -1);
}

void updateScreen(TileMap &tileMap, ListMenu &menuData, GameMenu &gameMenu, bool keypress){
    if (menuData.animateBkg) tileMap.draw(gameMenu.video_page);
    else clear_to_color(gameMenu.video_page, Constant::backgroundColor);
    gameMenu.refreshScreen(menuData);

    static uint32_t lastTime = Constant::getTicks();
    if (Constant::getTicks() - lastTime > bkgFrameTimeTick && (lastTime = Constant::getTicks()) > 0){
        tileMap.speed++;
    }
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

    while (!exit && !gameMenu.isCloseRequested()) {
        //If resizing detected
        //if (menuData.maxLines != menuData.getScreenNumLines() - 1){
        //    menuData.maxLines = menuData.getScreenNumLines() - 1;
        //    menuData.endPos = (int)menuData.getNumGames() > menuData.maxLines ? menuData.maxLines : menuData.getNumGames();
        //    //clear_to_color(gameMenu.video_page, Constant::backgroundColor);
        //    updateScreen(tileMap, menuData, gameMenu, true, animateBkg);
        //}

        gameMenu.gameTimeCounter = !limitFps ? 1 : gameMenu.gameTimeCounter;

        static uint32_t lastGameTick = Constant::getTicks();
        if (Constant::getTicks() - lastGameTick > bkgFrameTimeTick && (lastGameTick = Constant::getTicks()) > 0){
            gameMenu.gameTicks.ticks++;
        }

        if (gameMenu.gameTimeCounter){
            gameMenu.joystick.readJoystick();

            if ((keypressed() && readkey()) || gameMenu.joystick.isJoyPressed()){
                if (key[KEY_ESC] || (gameMenu.joystick.getButtonStat(gameMenu.joystick.J_SELECT) && gameMenu.joystick.getButtonStat(gameMenu.joystick.J_START))){
                    exit = true;
                } else if (key[KEY_ENTER] || gameMenu.joystick.getButtonStat(gameMenu.joystick.J_A)){
                    sound.play(SBTNLOAD, true);
                    gameMenu.joystick.resetButtons();
                    gameMenu.launchProgram(menuData, argv0);
                } else {
                    if (key[KEY_UP] || gameMenu.joystick.getButtonStat(gameMenu.joystick.J_UP)){
                        menuData.prevPos();
                        sound.play(SBTNCLICK);
                        //updateScreen(tileMap, menuData, gameMenu, true, animateBkg);
                    }
                    if (key[KEY_DOWN] || gameMenu.joystick.getButtonStat(gameMenu.joystick.J_DOWN)){
                        menuData.nextPos();
                        sound.play(SBTNCLICK);
                    }
                    if (key[KEY_RIGHT] || gameMenu.joystick.getButtonStat(gameMenu.joystick.J_RIGHT)){
                        menuData.nextPage();
                        sound.play(SBTNCLICK);
                    }
                    if (key[KEY_LEFT] || gameMenu.joystick.getButtonStat(gameMenu.joystick.J_LEFT)){
                        menuData.prevPage();
                        sound.play(SBTNCLICK);
                    }
                    if ((key[KEY_TAB] && !(key_shifts & KB_SHIFT_FLAG)) || gameMenu.joystick.getButtonStat(gameMenu.joystick.J_RB)){
                        sound.play(SBTNCLICK);
                        gameMenu.showMessage("Refreshing gamelist...");
                        gameMenu.getNextCfgEmu();
                        gameMenu.loadEmuCfg(menuData);
                    }
                    if ( (key[KEY_TAB] && (key_shifts & KB_SHIFT_FLAG)) || gameMenu.joystick.getButtonStat(gameMenu.joystick.J_LB)){
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
            menuData.keyUp = gameMenu.isKeyUp() || gameMenu.joystick.isJoyReleased();
            updateScreen(tileMap, menuData, gameMenu, false);
            if (gameMenu.gameTimeCounter > 0) 
                gameMenu.gameTimeCounter--;
        
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
    }
}

/**
 * 
 */
int main(int argc, char *argv[]){
    argv0 = argv[0];
    string rutaApp = SOUtils::iniciarSistema(argv);
    CfgLoader cfgLoader;

    Traza::open();
    if (cfgLoader.isDebug()){
        Traza::level = Traza::T_DEBUG;
    }

    Traza::print(Traza::T_DEBUG, "Loading emulators config...");
    GameMenu gameMenu(&cfgLoader);

    Traza::print(Traza::T_DEBUG, "Creating menu data...");
    ListMenu listMenu(SCREEN_W, SCREEN_H);

    ALFONT_FONT *fontsmall = Fonts::getFont(Fonts::FONTSMALL);

    Traza::print(Traza::T_DEBUG, "Setting layout...");
    listMenu.setLayout(LAYBOXES, SCREEN_W, SCREEN_H);
    
    Constant::textColor = makecol(255, 255, 255);
    Constant::backgroundColor = makecol(0, 0, 0);

    if (cfgLoader.isDebug()) 
        Constant::drawTextCentre(screen, fontsmall, "Creating bitmap buffer...", SCREEN_W / 2, SCREEN_H / 2, Constant::textColor, Constant::backgroundColor);

    Traza::print(Traza::T_DEBUG, "Creating bitmap buffer...");
    if (!gameMenu.initDblBuffer()){
        Traza::print(Traza::T_ERROR, "Could not create bitmap");
        return 1;
    }
    
    clear(screen);
    Traza::print(Traza::T_DEBUG, "Loading games...");
    Constant::drawTextCentre(screen, fontsmall, "Loading games...", SCREEN_W / 2, SCREEN_H / 2, Constant::textColor, -1);
    
    if (cfgLoader.configMain.debug){
        string appDir = argv[0];
        clear_to_color(screen, Constant::backgroundColor);
        string msg = "argv[0]:" + appDir;
        Constant::drawTextCentre(screen, fontsmall, msg.c_str(), SCREEN_W / 2, SCREEN_H / 2, Constant::textColor, -1);
        Constant::drawTextCentre(screen, fontsmall, Constant::getAppDir().c_str(), SCREEN_W / 2, SCREEN_H / 2 + fontsmall->face_h + 3, Constant::textColor, -1);
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