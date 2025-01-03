#pragma once

#include <string>
#include <sstream>
#include <algorithm>  //transform
#include <vector>

#include "allegro.h"
#include "utils/font/alfont.h"

using namespace std;


const int maxFps = 30;
const int bkgSpeedPixPerS = 15;
const float maxFrameTime = 1000.0 / maxFps;
const float bkgFrameTimeTick = 1000.0 / bkgSpeedPixPerS;
const int SCREENHDIV = 15;

typedef enum{ TIPODIRECTORIO, TIPOFICHERO} enumFileAttr;
typedef enum{ COMPAREWHOLEWORD, COMPAREBEGINNING} enumFileCompare;
typedef enum{ LAYTEXT, LAYSIMPLE, LAYBOXES} enumLayout;
typedef enum{ ALIGN_TOP, ALIGN_MIDDLE} enumAlign;
typedef enum{ SBTNCLICK, SBTNLOAD } enumSounds;

typedef enum {
        page_white_text,
        folder,
        page_white,
        page_white_gear,
        page_white_compressed,
        page_white_picture,
        page_white_zip
}enumIco;

typedef enum {
    launch_system,          //0
    launch_spawn,           //1
    launch_create_process,  //2
    launch_batch
} launchMethods;

class Constant{
    public:
        Constant(){}
        ~Constant(){};

        static string getAppDir(){ 
            return appDir; 
        }
        static void setAppDir(string var){
            appDir = var;
        }

        static char FILE_SEPARATOR;
        static char tempFileSep[2];
        static const string WHITESPACE;
        //Dependientes del sistema
        static const char FILE_SEPARATOR_UNIX;
        static const string MAME_SYS_ID;
        static int backgroundColor;
        static int textColor;
        static COLOR_MAP global_trans_table;
        static volatile uint32_t totalTicks;

        /**
        * Obtiene el separador de directorios de windows o unix
        */
        static string getFileSep(){
            char tmpFileSep[2] = {FILE_SEPARATOR,'\0'};
            return string(tmpFileSep);
        }

        static string replaceAll(string str, string tofind, string toreplace){
            size_t position = 0;
            size_t lastPosition = 0;
            string replaced = "";

            if (!str.empty()){
                for ( position = str.find(tofind); position != string::npos; position = str.find(tofind,lastPosition) ){
                        replaced.append(str.substr(lastPosition, position - lastPosition));
                        replaced.append(toreplace);
                        lastPosition = position + tofind.length();
                }
                if (str.length() > 0){
                    replaced.append(str.substr(lastPosition, str.length()));
                }
            }
            return(replaced);
        }

        static string TrimLeft(const string& s)
        {
            size_t startpos = s.find_first_not_of(WHITESPACE);
            return (startpos == string::npos) ? "" : s.substr(startpos);
        }

        static string TrimRight(const string& s)
        {
            size_t endpos = s.find_last_not_of(WHITESPACE);
            return (endpos == string::npos) ? "" : s.substr(0, endpos+1);
        }

        static string Trim(const string& s)
        {
            return TrimRight(TrimLeft(s));
        }

        static string toString(char c){
            std::string str(1, c); // creates a string with a single character 'A'
            return str;
        }

        /**
        *
        */
        static vector<string> splitChar(const string &s, char delim) {
            vector<string> elems;
            return splitChar(s, delim, elems);
        }

        /**
        *
        */
        static vector<string> &splitChar(const string &s, char delim, vector<string> &elems) {
            stringstream ss(s);
            string item;
            while(getline(ss, item, delim)) {
                elems.push_back(item);
            }
            return elems;
        }

        template<class TIPO> static string TipoToStr(TIPO number){
           stringstream ss;//create a stringstream
           ss << number;//add number to the stream
           return ss.str();//return a string with the contents of the stream
        }

        static void lowerCase(string *var){
            std::transform(var->begin(), var->end(), var->begin(), ::tolower);
        }

        static void upperCase(string *var){
            std::transform(var->begin(), var->end(), var->begin(), ::toupper);
        }

        template<class TIPO> static TIPO strToTipo(string str){
                TIPO i;
                stringstream s_str( str );
                s_str >> i;
                return i;
        }

        static void setExecMethod(int var){EXEC_METHOD = var;}
        static int getExecMethod(){return EXEC_METHOD;}

        //Obtains the milliseconds since the application start
        static uint32_t getTicks(){
            return maxFrameTime * totalTicks;
        }

        static void drawText(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color, int bg){
            if (f == NULL)
                return;
                
            if (SCREEN_H < 768){
                alfont_textout_ex(bmp, f, s, x, y, color, bg);
            } else {
                alfont_textout_aa_ex(bmp, f, s, x, y, color, bg);
            }
        }

        static void drawTextCentre(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color, int bg){
            if (f == NULL)
                return;
            if (SCREEN_H < 768){
                alfont_textout_centre_ex(bmp, f, s, x, y, color, bg);
            } else {
                alfont_textout_centre_aa_ex(bmp, f, s, x, y, color, bg);
            }
        }

    private:
        static string appDir;
        static int EXEC_METHOD;
        
};
