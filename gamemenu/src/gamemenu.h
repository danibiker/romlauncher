#pragma once

#include "utils/const/Constant.h"
#include "utils/io/dirutil.h"
#include "utils/io/screen.h"
#include "utils/io/sound.h"
#include "utils/so/launcher.h"
#include "utils/beans/structures.h"
#include "utils/uiobjects/image.h"
#include "utils/uiobjects/textarea.h"
#include "utils/uiobjects/listmenu.h"
#include "utils/font/fonts.h"

#include "alpng.h"

#include <memory>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <chrono>

using namespace std;
using namespace std::chrono;

class GameMenu{
    public:
        BITMAP *video_page;
        GameMenu(){
            loadMainConfig();
            emuCfgPos = 0;
            gameTicks.ticks = 0;
        };
        ~GameMenu(){
            
        }
        
        void createMenuImages(ListMenu &);
        void loadEmuCfg(ListMenu &);
        void refreshScreen(ListMenu &);
        void launchProgram(ListMenu &, string);
        
        bool initDblBuffer(){
            /* Create bitmap for page flipping */
            video_page = create_bitmap(this->getWidth(), this->getHeight());
            return video_page != NULL;
        }
        int saveGameMenuPos(ListMenu &);
        int recoverGameMenuPos(ListMenu &, struct ListStatus &);
        void showMessage(string);

        
        ConfigEmu getNextCfgEmu(){
            emuCfgPos++;
            emuCfgPos = emuCfgPos % configEmus.size();
            return configEmus.at(emuCfgPos);
        }

        ConfigEmu getPrevCfgEmu(){
            if (emuCfgPos <= 0 && configEmus.size() > 0)
                emuCfgPos = configEmus.size() - 1;
            else 
                emuCfgPos--;
            return configEmus.at(emuCfgPos);
        }

        ConfigMain getConfigMain(){
            return configMain;
        }

        int getWidth(){
            return configMain.resolution[0] < 0 ? 320 : configMain.resolution[0];
        }

        int getHeight(){
            return configMain.resolution[1] < 0 ? 240 : configMain.resolution[1];
        }

        void setWidth(int w){
            configMain.resolution[0] = w;
        }

        void setHeight(int h){
            configMain.resolution[1] = h;
        }

        GameTicks gameTicks;

    private:
        void loadMainConfig();
        void loadEmuConfig(string);
        string getPathPrefix(string);
        string encloseWithCharIfSpaces(string, string);
        
        ConfigMain configMain;
        vector<ConfigEmu> configEmus;
        int emuCfgPos;
        
        map<string, Image> menuImages;
        map<string, TextArea> menuTextAreas;

        const string SNAP = "snap";
        const string BOX2D = "box2d";
        const string SNAPTIT = "snaptit";
        const string SNAPFS = "snapFs";
        const string SYNOPSIS = "synopsis";
        const string MENUTMP = "menu.tmp";
};

/**
 * 
 */
void GameMenu::createMenuImages(ListMenu &listMenu){
    /** snap */
    Image imageSnap;
    const int snapW = SCREEN_W / 2;
    const int snapH = listMenu.getH() / 2;
    const int snapOffset = SCREEN_W / 10;
    //const int snapOffset = 5;

    if (SCREEN_W / 2 >= 320){
        imageSnap.setX(SCREEN_W / 2 + snapOffset);
        imageSnap.setY(listMenu.getY());
        imageSnap.setW(snapW - snapOffset * 2);
        imageSnap.setH(snapH - snapOffset);
    } else {
        imageSnap.setX(SCREEN_W / 2);
        imageSnap.setY(listMenu.getY());
        imageSnap.setW(snapW);
        imageSnap.setH(snapH);
    }
    imageSnap.vAlign = ALIGN_TOP;
    menuImages.insert(make_pair(SNAP, imageSnap));

    /** Box2d */
    Image imageBox2d;
    const int box2dH = listMenu.getH() / 4;
    const int box2dW = SCREEN_W / 8;
    imageBox2d.setX(SCREEN_W / 2);
    imageBox2d.setY(SCREEN_H / 2 - box2dH);
    imageBox2d.setW(box2dW);
    imageBox2d.setH(box2dH);
    menuImages.insert(make_pair(BOX2D, imageBox2d));

    /** snaptit*/
    Image imageSnaptit;
    const int snapTitH = listMenu.getH() / 4;
    const int snapTitW = SCREEN_W / 6;
    imageSnaptit.setX(SCREEN_W - snapTitW);
    imageSnaptit.setY(SCREEN_H / 2 - snapTitH);
    imageSnaptit.setW(snapTitW);
    imageSnaptit.setH(snapTitH);
    menuImages.insert(make_pair(SNAPTIT, imageSnaptit));

    Image imageSnapFs(0, 0, SCREEN_W, SCREEN_H);
    imageSnapFs.drawfaded = true;
    menuImages.insert(make_pair(SNAPFS, imageSnapFs));
    
    const int sectionGap = 0;
    const int textAreaY = listMenu.getH() / 2 + listMenu.getY() + sectionGap;
    TextArea textarea(SCREEN_W / 2, textAreaY, SCREEN_W / 2, SCREEN_H - textAreaY);
    menuTextAreas.insert(make_pair(SYNOPSIS, textarea));
}

/**
 * 
 */
void GameMenu::refreshScreen(ListMenu &listMenu){
    ConfigEmu emu = this->configEmus.at(this->emuCfgPos);
    //Drawing the emulator name
    ALFONT_FONT *fontBig = Fonts::getFont(Fonts::FONTBIG);

    //Drawing the rest of list and images
    if (listMenu.getNumGames() > (size_t)listMenu.curPos){
        auto game = listMenu.listGames.at(listMenu.curPos).get();
        
        if (!game->shortFileName.empty()){
            if (listMenu.layout == LAYBOXES) {
                Constant::drawTextCentre(this->video_page, fontBig, emu.name.c_str(), 
                    this->getWidth() / 2, fontBig->face_h < listMenu.marginY ? (listMenu.marginY - fontBig->face_h) / 2 : 0 , textColor, -1);
                
                static const int menuBars = makecol(128, 128, 128);
                fastline(this->video_page, listMenu.marginX, listMenu.marginY - 1 , SCREEN_W - listMenu.marginX, listMenu.marginY - 1, menuBars);
                listMenu.draw(this->video_page);

                //Draw and update the screen because the loading of images can take a long time
                if (listMenu.keyUp){
                    string assetsDir = getPathPrefix(emu.assets) + string(tempFileSep);
                    //Drawing the rom's synopsis text
                    menuTextAreas[SYNOPSIS].loadTextFileFromGame(assetsDir + "synopsis" + string(tempFileSep), *game, ".txt");
                    menuTextAreas[SYNOPSIS].resetTicks(this->gameTicks);
                    menuTextAreas[SYNOPSIS].draw(this->video_page, this->gameTicks);

                    //Snapshot picture
                    menuImages[SNAP].loadImageFromGame(assetsDir + "snap" + string(tempFileSep), *game, ".png");
                    menuImages[SNAP].printImage(this->video_page);
                    blit(this->video_page, screen, 0, 0, 0, 0, this->video_page->w, this->video_page->h);

                    if (SCREEN_W / 2 < 320){
                        //If it's so small, only show the snapshot
                        return;
                    }

                    //Box picture
                    menuImages[BOX2D].loadImageFromGame(assetsDir + "box2d" + string(tempFileSep), *game, ".png");
                    menuImages[BOX2D].printImage(this->video_page);
                    blit(this->video_page, screen, 0, 0, 0, 0, this->video_page->w, this->video_page->h);

                    //Title picture
                    menuImages[SNAPTIT].loadImageFromGame(assetsDir + "snaptit" + string(tempFileSep), *game, ".png");
                    menuImages[SNAPTIT].printImage(this->video_page);
                    blit(this->video_page, screen, 0, 0, 0, 0, this->video_page->w, this->video_page->h);
                } else {
                    menuImages[SNAP].printImage(this->video_page);
                    menuImages[BOX2D].printImage(this->video_page);
                    menuImages[SNAPTIT].printImage(this->video_page);
                    menuTextAreas[SYNOPSIS].draw(this->video_page, this->gameTicks);
                }

            } else if (listMenu.layout == LAYSIMPLE) {
                if (listMenu.keyUp){
                    //Snapshot picture
                    menuImages[SNAPFS].loadImageFromGame(getPathPrefix(emu.assets) + string(tempFileSep)
                        + "snap" + string(tempFileSep), *game, ".png");
                }
                menuImages[SNAPFS].printImage(this->video_page);
                //Draw the menu element after the image
                alfont_textout_centre_ex(this->video_page, fontBig, emu.name.c_str(), 
                    this->getWidth() / 2, fontBig->face_h < listMenu.marginY ? (listMenu.marginY - fontBig->face_h) / 2 : 0 , textColor, -1);
                fastline(this->video_page, listMenu.marginX, listMenu.marginY - 1, listMenu.getW(), listMenu.marginY - 1, textColor);
                listMenu.draw(this->video_page);

            } else if (listMenu.layout == LAYTEXT) {
                alfont_textout_centre_ex(this->video_page, fontBig, emu.name.c_str(), 
                    this->getWidth() / 2, fontBig->face_h < listMenu.marginY ? (listMenu.marginY - fontBig->face_h) / 2 : 0 , textColor, -1);
                fastline(this->video_page, listMenu.marginX, listMenu.marginY - 1, listMenu.getW(), listMenu.marginY - 1, textColor);
                listMenu.draw(this->video_page);
            }
        }

    } else if (listMenu.getNumGames() == 0){
        alfont_textout_centre_ex(this->video_page, fontBig, emu.name.c_str(), 
            this->getWidth() / 2, fontBig->face_h < listMenu.marginY ? (listMenu.marginY - fontBig->face_h) / 2 : 0 , textColor, -1);
        fastline(this->video_page, listMenu.marginX, listMenu.marginY - 1 , SCREEN_W - listMenu.marginX, listMenu.marginY - 1, textColor);
        textout_centre_ex(this->video_page, font, "No roms found", SCREEN_W / 2, SCREEN_H / 2, textColor, -1);
        
    } else {
        textout_centre_ex(this->video_page, font, "The configuration is not valid", SCREEN_W / 2, SCREEN_H / 2, textColor, -1);
        textout_centre_ex(this->video_page, font, "Press TAB to select the next entry or", SCREEN_W / 2, SCREEN_H / 2 + font->height + 3, textColor, -1);
        textout_centre_ex(this->video_page, font, "Press ESC to exit", SCREEN_W / 2, SCREEN_H / 2 + (font->height + 3) * 2, textColor, -1);
    }
}

/**
 * 
 */
void GameMenu::loadMainConfig(){
    dirutil dir;

    string filepath = Constant::getAppDir() //+ string(tempFileSep) + "gmenu" 
        + string(tempFileSep) + "gmenu.cfg";

    //if (dir.fileExists(filepath.c_str()) && !dir.isDir(filepath.c_str())){
        fstream emucfg;
        emucfg.open(filepath, ios::in);

        bool fileopened = emucfg.is_open();
        if (fileopened){
            string line;
            while(getline(emucfg, line)){
                line = Constant::Trim(Constant::replaceAll(Constant::replaceAll(line, "\r", ""), "\n", ""));
                if (line.length() > 1 && line.at(0) != '#' && line.find("=") != string::npos){
                    vector<string> keyvalue = Constant::splitChar(line, '=');
                    if (keyvalue.size() < 2)
                        continue;

                    string key = Constant::Trim(keyvalue.at(0));
                    string value = Constant::Trim(keyvalue.at(1));

                    if (key.compare("emulators") == 0){
                        configMain.emulators = Constant::splitChar(value, ' '); 
                    } else if (key.compare("debug") == 0){
                        configMain.debug = value.compare("yes") == 0 ? true : false ; 
                    } else if (key.compare("path_prefix") == 0){
                        configMain.path_prefix = value;
                    } else if (key.compare("resolution") == 0){
                        vector<string> res = Constant::splitChar(value, ' ');
                        configMain.resolution[0] = Constant::strToTipo<int>(res[0]);
                        configMain.resolution[1] = Constant::strToTipo<int>(res[1]);
                    }
                }
            }
        }
        emucfg.close();

        if (fileopened){
            if (getConfigMain().debug) cout << "Loading emulators:" << flush;
            for (size_t i=0; i < configMain.emulators.size(); i++){
                loadEmuConfig(configMain.emulators.at(i));
            }
            if (getConfigMain().debug) cout << endl;
        }
    //}
}

/**
 * 
 */
void GameMenu::loadEmuConfig(string emuname){
    ConfigEmu cfgEmu;
    dirutil dir;
    string strFilepath = Constant::getAppDir() + string(tempFileSep) //+ "gmenu" + string(tempFileSep) 
        + "config" + string(tempFileSep) + emuname + ".cfg";
    const char *filepath = strFilepath.c_str();

    if (getConfigMain().debug) cout << " " << emuname << flush;
    //cout << " " << emuname << endl;
    bool fileopened = false;
    //cout << "Checking if exists" <<endl;
    if (dir.fileExists(filepath) && !dir.isDir(filepath)){
    //if (dir.fileExists(filepath)){
        fstream fileCfg;
        //cout << "Opening file" <<endl;
        fileCfg.open(filepath, ios::in);

        //cout << "Checking if is open" <<endl;
        fileopened = fileCfg.is_open();
        if (fileopened){
            string line;
            ConfigEmu cfgEmu;

            while(getline(fileCfg, line)){
                //cout << "reading line" <<endl;
                if (line.length() > 1 && line.at(0) != '#' && line.find("=") != string::npos){
                    //cout << "splitting line and trimming" <<endl;
                    vector<string> keyvalue = Constant::splitChar(line, '=');        
                    string key = Constant::Trim(keyvalue.at(0));
                    string value = Constant::Trim(keyvalue.at(1));

                    if (keyvalue.size() < 2)
                        continue;

                    //cout << "assigning value for: " << key <<endl;
                    if (key.compare("name") == 0){
                        cfgEmu.name = value;
                    } else if (key.compare("system") == 0){
                        cfgEmu.system = value;
                    } else if (key.compare("description") == 0){
                        cfgEmu.description = value;
                    } else if (key.compare("directory") == 0){
                        cfgEmu.directory = value;
                    } else if (key.compare("executable") == 0){
                        cfgEmu.executable = value;
                    } else if (key.compare("global_options") == 0){
                        cfgEmu.global_options = value;
                    } else if (key.compare("map_file") == 0){
                        cfgEmu.map_file = value;
                    } else if (key.compare("options_before_rom") == 0){
                        cfgEmu.options_before_rom = value.compare("yes") == 0 ? true : false;
                    } else if (key.compare("screen_shot_directory") == 0){
                        cfgEmu.screen_shot_directory = value;
                    } else if (key.compare("assets") == 0){
                        cfgEmu.assets = value;
                    } else if (key.compare("use_rom_file") == 0){
                        cfgEmu.use_rom_file = value.compare("yes") == 0 ? true : false;
                    } else if (key.compare("rom_directory") == 0){
                        cfgEmu.rom_directory = value;
                    } else if (key.compare("rom_extension") == 0){
                        cfgEmu.rom_extension = value;
                    } else if (key.compare("use_extension") == 0){
                        cfgEmu.use_extension = value.compare("yes") == 0 ? true : false;
                    } else if (key.compare("use_rom_directory") == 0){
                        cfgEmu.use_rom_directory = value.compare("yes") == 0 ? true : false;
                    }
                }
            }             
            //cout << "adding emu " << configEmus.size() <<endl;   
            configEmus.emplace_back(cfgEmu);            
        }
        //cout << "closing file..." <<endl;   
        fileCfg.close();
    }

    if (!fileopened){
        string msg = "There is no config file for " + emuname + ". Exiting..."; 
        //textout_centre_ex(screen, font, msg.c_str(), SCREEN_W / 2, SCREEN_H / 2, textColor, -1);
        //textout_centre_ex(screen, font, "Press a key to continue", SCREEN_W / 2, SCREEN_H / 2 + (font->height + 3), textColor, -1);
        cout << msg << endl;
        //readkey();
    }
}

void GameMenu::showMessage(string msg){
    int startGray = 240;
    static const int bkg = makecol(startGray, startGray, startGray);
    static const int black = makecol(0, 0, 0);

    int rw = text_length(font, msg.c_str()) + 5; 
    //int rh = this->video_page->h / 3;
    int rh = font->height * 2;
    int rx = (this->video_page->w - rw) / 2;
    int ry = (this->video_page->h - rh) / 2 + font->height / 2;

    //drawing_mode(DRAW_MODE_TRANS, this->video_page, rx, ry);
    rectfill(this->video_page, rx, ry, rx + rw, ry + rh, bkg);
    
    const int step = 40;
    for (int i=1; i < 5; i++){
        int fadingBkg = makecol(startGray - i*step, startGray - i*step, startGray - i*step);
        rect(this->video_page, rx - i, ry - i, rx + rw + i, ry + rh + i, fadingBkg);
    }

    //drawing_mode(DRAW_MODE_SOLID, this->video_page, rx, ry);
    textout_centre_ex(this->video_page, font, msg.c_str(), this->video_page->w / 2, this->video_page->h / 2,  black, -1);
    blit(this->video_page, screen, 0, 0, 0, 0, this->video_page->w, this->video_page->h);
}

/**
 * 
 */
void GameMenu::loadEmuCfg(ListMenu &menuData){
    if (this->configEmus.size() == 0){
        clear(screen);
        string msg = "There are no emulators configured. Exiting..."; 
        textout_centre_ex(screen, font, msg.c_str(), SCREEN_W / 2, SCREEN_H / 2, textColor, -1);
        textout_centre_ex(screen, font, "Press a key to continue", SCREEN_W / 2, SCREEN_H / 2 + (font->height + 3), textColor, -1);
        readkey();
        exit(0);
    }

    if (this->configEmus.size() <= (size_t)this->emuCfgPos){
        this->emuCfgPos = 0;
    } 

    dirutil dir;
    ConfigEmu emu = this->configEmus.at(this->emuCfgPos);
    string mapfilepath = Constant::getAppDir() //+ string(tempFileSep) + "gmenu" 
            + string(tempFileSep) + "config" + string(tempFileSep) + emu.map_file;
    
    if (emu.use_rom_file && !emu.map_file.empty() && dir.fileExists(mapfilepath.c_str())){
        menuData.mapFileToList(mapfilepath);
    } else {
        mapfilepath = getPathPrefix(emu.rom_directory);
        vector<unique_ptr<FileProps>> files;
        emu.rom_extension = " " + emu.rom_extension;
        string extFilter = Constant::replaceAll(emu.rom_extension, " ", ".");

        if (configMain.debug){
            clear_to_color(screen, backgroundColor);
            string msg = "searching " + mapfilepath; 
            textout_centre_ex(screen, font, msg.c_str(), SCREEN_W / 2, SCREEN_H / 2, textColor, -1);
        }

        dir.listarFilesSuperFast(mapfilepath.c_str(), files, extFilter, true, false);

        ConfigEmu emu = this->configEmus.at(this->emuCfgPos);
        string mapfilepath = getPathPrefix(emu.rom_directory);

        if (configMain.debug){
            clear_to_color(screen, backgroundColor);
            string msg = "roms found: " + Constant::TipoToStr(files.size()); 
            string msg2 = "In dir " + mapfilepath;
            textout_centre_ex(screen, font, msg.c_str(), SCREEN_W / 2, SCREEN_H / 2, textColor, -1);
            textout_centre_ex(screen, font, msg2.c_str(), SCREEN_W / 2, SCREEN_H / 2 + font->height + 3, textColor, -1);
            textout_centre_ex(screen, font, "Press a key to continue", SCREEN_W / 2, SCREEN_H / 2 + (font->height + 3) * 2, textColor, -1);
            readkey();
        }

        menuData.filesToList(files, emu);
        files.clear();
    }
}

/**
 * 
 */
string GameMenu::getPathPrefix(string filepath){
    ConfigEmu emu = this->configEmus.at(this->emuCfgPos);
    string finalpath = configMain.path_prefix + filepath;

    string drivestr = string(":") + string(tempFileSep);
    //Checking if the path to the roms is absolute
    if (!filepath.empty() && (filepath.at(0) == FILE_SEPARATOR || filepath.find(drivestr) != string::npos)){
        finalpath = filepath;
    }
    #if defined(WIN) || defined(DOS)
        finalpath = Constant::replaceAll(finalpath, "/", "\\");
    #endif
    return finalpath;
}

/**
 * 
 */
string GameMenu::encloseWithCharIfSpaces(string str, string encloseChar){
    str = Constant::Trim(str);
    return str.find(" ") != string::npos ? encloseChar + str + encloseChar : str;
}

/**
 * 
 */
void GameMenu::launchProgram(ListMenu &menuData, string argv0){
    Launcher launcher;
    dirutil dir;
    vector<string> commands;

    if (this->configEmus.size() <= (size_t)this->emuCfgPos)
        return;

    ConfigEmu emu = this->configEmus.at(this->emuCfgPos);

    commands.emplace_back(getPathPrefix(emu.directory) + string(tempFileSep)
        + emu.executable);
    
    if (emu.options_before_rom){
        commands.emplace_back(emu.global_options);
    }

    if (menuData.listGames.size() <= (size_t)menuData.curPos)
        return;

    auto game = menuData.listGames.at(menuData.curPos).get();
    
    //Ignoring the fields if a rom file is used
    if (emu.use_rom_file){
        commands.emplace_back(encloseWithCharIfSpaces(game->shortFileName, "\"")); 
    } else {
        string romdir = emu.use_rom_directory ? getPathPrefix(emu.rom_directory) + string(tempFileSep) : "";
        string romFile = game->longFileName;

        #ifdef DOS
            //Maybe the long filename support is activated on msdos,
            //Otherwise just pick up the shortfilename
            string fileabslongname = encloseWithCharIfSpaces(romdir + game->longFileName, "\"");
            string msg = "romdir: " + romdir;
            string msg1;
            string msg2;

            if (!dir.fileExists( fileabslongname.c_str()  )){
                romFile = game->shortFileName;
                msg1 = "file " + game->longFileName + "doesn't exists.";
                msg2 = "launching " + romFile;
            } else {
                msg1 = "launching " + romFile;
            }

            if (configMain.debug){
                clear(screen);
                textout_centre_ex(screen, font, msg.c_str(), SCREEN_W / 2, SCREEN_H / 2, textColor, -1);
                textout_centre_ex(screen, font, msg1.c_str(), SCREEN_W / 2, SCREEN_H / 2 + (font->height + 3) * 2, textColor, -1);
                textout_centre_ex(screen, font, msg2.c_str(), SCREEN_W / 2, SCREEN_H / 2 + (font->height + 3) * 3, textColor, -1);
                textout_centre_ex(screen, font, "Press a key to continue", SCREEN_W / 2, SCREEN_H / 2 + (font->height + 3) * 4, textColor, -1);
                readkey();
            }
        #endif

        string rom = emu.use_extension ? romFile : dir.getFileNameNoExt(romFile);
        commands.emplace_back(encloseWithCharIfSpaces(romdir + rom, "\"")); 
    }

    //string romToLaunch = romdir + rom;
    //if (!dir.fileExists(romToLaunch.c_str())){
    //    clear_to_color(screen, backgroundColor);
    //    string msg = "roms doesn't exist: " + romToLaunch; 
    //    textout_centre_ex(screen, font, msg.c_str(), SCREEN_W / 2, SCREEN_H / 2, textColor, -1);
    //    textout_centre_ex(screen, font, "Press a key to continue", SCREEN_W / 2, SCREEN_H / 2 + (font->height + 3) * 2, textColor, -1);
    //    readkey();
    //    return; // or handle the error as needed
    //}

    if (!emu.options_before_rom){
        commands.emplace_back(emu.global_options);
    }

    saveGameMenuPos(menuData);
    launcher.launch(commands, configMain.debug, argv0);
}

/**
 * 
 */
int GameMenu::saveGameMenuPos(ListMenu &menuData){
    FILE* outfile;
    string filepath = Constant::getAppDir() + Constant::getFileSep() + MENUTMP;
    int ret = 0;
    // open file for writing
    outfile = fopen(filepath.c_str(), "wb");
    if (outfile == NULL) {
        cerr << "Error openning file: " << filepath << endl;
        return 1;
    }

    struct ListStatus input1 = { this->emuCfgPos, menuData.iniPos, menuData.endPos, 
        menuData.curPos, menuData.maxLines, menuData.layout, menuData.animateBkg};

    int flag = 0;
    flag = fwrite(&input1, sizeof(struct ListStatus), 1, outfile);

    if (flag) {
        cout << "Contents of the structure written successfully" << endl;
    } else {
        cerr << "Error Writing to File: " << filepath << endl;
        ret = 1;
    }
    fclose(outfile);
    return ret;
}

/**
 * 
 */
int GameMenu::recoverGameMenuPos(ListMenu &menuData, struct ListStatus &read_struct){
    FILE* infile;
    string filepath = Constant::getAppDir() + Constant::getFileSep() + MENUTMP;
    int ret = 0;

    // Open person.dat for reading
    infile = fopen(filepath.c_str(), "rb");
    if (infile == NULL) {
        cerr << "Error openning file: " << filepath << endl;
        return 1;
    }

    if (fread(&read_struct, sizeof(read_struct), 1, infile) > 0){
        cout << "emupos: " << read_struct.emuLoaded << "; inipos: " << read_struct.iniPos
            << "; endpos: " << read_struct.endPos << "; curpos: " << read_struct.curPos 
            << "; maxlines: " << read_struct.maxLines
            << "; layout: " << read_struct.layout
            << "; animateBkg: " << read_struct.animateBkg << endl;
        //Setting the emulator selected        
        this->emuCfgPos = read_struct.emuLoaded;
    } else {
        ret = 1;
    }

    fclose(infile);
    return ret;
}
