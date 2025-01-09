#pragma once

#include <string>
#include <vector>

using namespace std;

class FileLaunch{
    public:
            FileLaunch(){};
            ~FileLaunch(){};
            string nombreemu;
            string rutaexe;
            string fileexe;
            string parmsexe;
            string rutaroms;
            string nombrerom;
            string titlerom;
            string nplayers;
            string categ;
            bool descomprimir;
            bool fixoption;
};

class Executable{
    public:
        Executable(){
            ejecutable = "";
            param = "";
            filerompath = "";
            comandoFinal = "";
            filenameinparms = false;
        }
        ~Executable(){}
        string ejecutable;
        string param;
        string filerompath;
        string comandoFinal;
        bool filenameinparms;
};

struct GameTicks{
    uint16_t ticks;
};

struct Dimension{
    int w, h;
};

struct ListStatus{
    int emuLoaded;
    int iniPos;
    int endPos;
    int curPos;
    int maxLines;
    int layout;
    bool animateBkg;
};

class GameFile{
    public:
    GameFile(){
    }
    ~GameFile(){
    }
    string shortFileName;
    string longFileName;
    string gameTitle;
    size_t cutTitleIdx;
    //string gameImage;
};

class FileName8_3 {
    string shortFN;
    string longFN;

    FileName8_3(string shortFN, string longFN) {
        this->shortFN = shortFN;
        this->longFN = longFN;
    }
};

class ConfigMain{
    public:
    ConfigMain(){
        resolution[0] = 0;
        resolution[1] = 0;
        debug = false;
        path_prefix = "";
        alsaReset = false;
        background_music = 0;
        mp3_file = "";
    }
    ~ConfigMain(){
    }
    vector<string> emulators;
    bool debug;
    string path_prefix;
    int resolution[2];
    bool alsaReset;
    int background_music;
    string mp3_file;
};

class ConfigEmu{
    public:
    ConfigEmu(){
        options_before_rom = false;
        use_rom_file = false;
        use_extension = true;
        use_rom_directory = true;
    }
    ~ConfigEmu(){

    }

    string name;
    string system;
    string description;
    //Location of emulator, i.e. c:\mame
    string directory;
    //Name of emulator executable, i.e. mame.exe
    string executable;
    //Global options passed to emulator, i.e. -sound 1
    string global_options;
    string map_file;
    //Options go before ROM when launching: "yes" or "no".
    // i.e. yes: "emulator.exe -option1 -option2 rom"
    //       no: "emulator.exe rom -option1 -option2"
    bool options_before_rom;

    string assets;
    
    string screen_shot_directory;
    //# A ROM file is a list of ROMs to use.  If set to "no", ROMs are
    //# scanned for in the rom_directory.  If set to "yes" a ROM file (which
    //# is essentially just a list of ROMs) is used instead of trying scan.
    //# The default is "no".  ROM files are useful for merged ROMs with
    //# MAME, where the actual ROM names are buried within a ZIP file.
    bool use_rom_file;
    //Directory to ROMs
    string rom_directory;
    //List of possible ROM extensions (without the ".")
    string rom_extension;
    //Use extension when launching game: "yes" or "no"
    // i.e. yes: "emulator.exe rom.ext"
    //       no: "emulator.exe rom"
    bool use_extension;
    //Use rom_directory when launcher game: "yes" or "no"
    // i.e. yes: "emulator.exe c:\full\path\rom"
    //       no: "emulator.exe rom"
    bool use_rom_directory;
};