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
#if defined(WIN) || defined(DOS)
    char Constant::FILE_SEPARATOR = 0x5C; //Separador de directorios para win32
#elif UNIX
    char Constant::FILE_SEPARATOR = Constant::FILE_SEPARATOR_UNIX; //Separador de directorios para unix
#endif

char Constant::tempFileSep[2] = {Constant::FILE_SEPARATOR,'\0'};
int Constant::Constant::backgroundColor;
int Constant::Constant::textColor;
COLOR_MAP Constant::global_trans_table;
const string Constant::MAME_SYS_ID = "75";
const string Constant::WHITESPACE = " \n\r\t";
volatile uint32_t Constant::totalTicks = 0;
uint32_t Joystick::last = 0;
string Constant::appDir = "";
int Constant::EXEC_METHOD = launch_batch;
ALFONT_FONT * Fonts::vFonts[2] = {NULL, NULL};
volatile int Engine::closeRequested = 0;
volatile uint32_t Engine::gameTimeCounter = 0;
volatile int Engine::key_up = 0;
FILE * Traza::fp = NULL; // separate definition
char Traza::log_message[256];
int Traza::level = Traza::T_ERROR;

void processKeys(ListMenu &, GameMenu &);