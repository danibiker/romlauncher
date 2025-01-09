#pragma once

#include "utils/const/Constant.h"
#include "utils/beans/structures.h"
#include "utils/io/dirutil.h"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>

class CfgLoader{
    public:
        CfgLoader(){
            loadMainConfig();
        }
        ~CfgLoader(){
            Traza::print(Traza::T_DEBUG, "Deleting CfgLoader...");
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

        bool isDebug(){
            return configMain.debug;
        }

        ConfigMain configMain;
        vector<ConfigEmu> configEmus;

    private:
        
        static const string CONFIGFILE;
        /**
         * 
         */
        void loadMainConfig(){
            dirutil dir;

            string filepath = Constant::getAppDir() + string(Constant::tempFileSep) + CONFIGFILE;

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
                        } else if (key.compare("alsa_reset") == 0){
                            configMain.alsaReset = value.compare("yes") == 0 ? true : false ; 
                        } else if (key.compare("background_music") == 0){
                            configMain.background_music = Constant::strToTipo<int>(value);
                        } else if (key.compare("mp3_file") == 0){
                            configMain.mp3_file = value;
                        }
                    }
                }
            }
            emucfg.close();

            if (fileopened){
                if (configMain.debug) cout << "Loading emulators:" << flush;
                for (size_t i=0; i < configMain.emulators.size(); i++){
                    loadEmuConfig(configMain.emulators.at(i));
                }
                if (configMain.debug) cout << endl;
            }
        }

        /**
         * 
         */
        void loadEmuConfig(string emuname){
            ConfigEmu cfgEmu;
            dirutil dir;
            string strFilepath = Constant::getAppDir() + string(Constant::tempFileSep)
                + "config" + string(Constant::tempFileSep) + emuname + ".cfg";
            const char *filepath = strFilepath.c_str();

            if (configMain.debug) cout << " " << emuname << flush;
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
};