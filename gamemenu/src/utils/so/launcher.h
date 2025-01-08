#pragma once

#include <allegro.h>
#include "utils/const/Constant.h"
#include "utils/io/dirutil.h"
#include "utils/beans/structures.h"
#include "utils/font/fonts.h"

//#include "utils/unzip/unziptool.h"

#ifdef __GNUC__
#include <unistd.h>
#else
#include <process.h>
#endif

#if defined(_POSIX_VERSION)
    #include <sys/wait.h>
#endif

#include <string>
#include <fstream>
#include <iostream>
#include <string.h>

using namespace std;

class Launcher{
    public:
        Launcher(){};
        ~Launcher(){};
        bool lanzarProgramaUNIXFork(FileLaunch &emulInfo);
        bool launch(vector<string> &commands, bool debug);
        string descomprimirZIP(string filename);
    private:
        Executable rutaEspecial(string ejecutable, string param, string filerompath);
        void deleteUnzipedRom(string romfile);
        int dosbatch(vector<string> &commands, string comando, bool debug);
        string getBatchPath();
};  

/**
 * 
 */
Executable Launcher::rutaEspecial(string ejecutable, string param, string filerompath){
    Executable cmdExec;
    cmdExec.filenameinparms = false;
    dirutil dir;

    if (param.find("%") != std::string::npos){

        if (param.find("%ROMNAME%") !=std::string::npos || param.find("%ROMNAMEXT%") !=std::string::npos
            || param.find("%ROMFULLPATH%") !=std::string::npos){
            cmdExec.filenameinparms = true;
        }

        param = Constant::replaceAll(param, "%ROMPATH%", dir.getFolder(filerompath));
        param = Constant::replaceAll(param, "%ROMNAME%", dir.getFileNameNoExt(filerompath));
        param = Constant::replaceAll(param, "%ROMNAMEXT%", dir.getFileName(filerompath));
        param = Constant::replaceAll(param, "%ROMFULLPATH%", filerompath);

        if (cmdExec.filenameinparms){
            cmdExec.comandoFinal = ejecutable + " " + param;
        } else {
            cmdExec.comandoFinal = ejecutable + " " + param + " " + filerompath;
        }
    } else {
        if (filerompath.empty())
            filerompath = "";
        else
            filerompath = "\"" + filerompath + "\"";

        cmdExec.comandoFinal = ejecutable + " " + param + " " + filerompath ;
    }
    cout << "Launcher::launch. comandoFinal: " + cmdExec.comandoFinal <<endl;

    cmdExec.ejecutable = ejecutable;
    cmdExec.param = param;
    cmdExec.filerompath = filerompath;

    return cmdExec;
}

/**
 * 
 */
bool Launcher::launch(vector<string> &commands, bool debug){
    bool launchOk = false;
    string comando;
    //#ifdef WIN
    //    comando = "start ";
    //#endif

    clear_to_color(screen, Constant::backgroundColor);
    ALFONT_FONT *fontsmall = Fonts::getFont(Fonts::FONTSMALL);
    Constant::drawTextCentre(screen, fontsmall, "launching command", SCREEN_W / 2, SCREEN_H / 2, Constant::textColor, -1);

    for (size_t i=0; i < commands.size(); i++){
        string parm = Constant::Trim(commands.at(i));
        if (!parm.empty()){
            comando += (i > 0 ? " " : "") + parm;
        } 
        if (debug){
            Constant::drawTextCentre(screen, fontsmall, parm.c_str(), SCREEN_W / 2, SCREEN_H / 2 + (fontsmall->face_h + 3) * (i+1), Constant::textColor, -1);
        }
    }

    if (debug){
        Constant::drawTextCentre(screen, fontsmall, comando.c_str(), SCREEN_W / 2, SCREEN_H / 2 + (fontsmall->face_h + 3) * (commands.size()+1), Constant::textColor, -1);
        Constant::drawTextCentre(screen, fontsmall, "Press a key to continue", SCREEN_W / 2, SCREEN_H / 2 + (fontsmall->face_h + 3) * (commands.size()+2), Constant::textColor, -1);
        readkey();
    }

    if (Constant::getExecMethod() == launch_system ){
        snprintf(Traza::log_message, sizeof(Traza::log_message), "Launching command -> %s", comando.c_str());
        Traza::print(Traza::T_ALL);
        #ifdef UNIX
            Fonts::exit();
            allegro_exit();
        #endif
        launchOk = system(comando.c_str()) == 0;
        Traza::print(Traza::T_DEBUG, "Comando terminado");
    } else if (Constant::getExecMethod() == launch_spawn || Constant::getExecMethod() == launch_create_process){
        snprintf(Traza::log_message, sizeof(Traza::log_message),"Launching command -> %s", comando.c_str());
        Traza::print(Traza::T_ALL);
        // extra room for program name and sentinel
        char **argv = new char* [commands.size()+1];  
        size_t j = 0;
        size_t posDirSep;
        for (; j < commands.size(); j++){
            if (j==0 && ((posDirSep = commands[j].find_last_of(Constant::tempFileSep)) != string::npos)){
                argv[j] = strdup(commands[j].substr(posDirSep + 1).c_str());
            } else {
                argv[j] = strdup(commands[j].c_str());
            }
            snprintf(Traza::log_message, sizeof(Traza::log_message),"argv[%s]=%s", std::to_string(j).c_str(), argv[j]);
            Traza::print(Traza::T_DEBUG);
        }     
        // end of arguments sentinel is NULL
        argv [j] = NULL;  
        
        #ifdef UNIX
            if ( fork() == 0 ){
                //by convention, argv[0] is the full program path
                if (execv(commands[0].c_str(), argv) == -1){
                    Traza::print(Traza::T_ERROR, "No se ha podido ejecutar el programa");
                    return false;
                } else {
                    return true;
                }
            } else {
                Fonts::exit();
                allegro_exit();
                int ret;
                wait(&ret); 
            }        
        #else
            Fonts::exit();
            allegro_exit();
            if (execv(commands[0].c_str(), argv) == -1){
                Traza::print(Traza::T_ERROR, "No se ha podido ejecutar el programa");
                return false;
            } else {
                return true;
            }
        #endif 
        // Deallocate memory
        for (j = 0; j < commands.size(); j++)     
            free(argv[j]);
        delete [] argv;

    } else if (Constant::getExecMethod() == launch_batch && dosbatch(commands, comando, debug) == 0){
        Fonts::exit();
        allegro_exit();
        cout << "Exiting to launch the game: " << comando << endl;
        exit(0);
    }
    return launchOk;
}

int Launcher::dosbatch(vector<string> &commands, string comando, bool debug){
    
    dirutil dir;        
    std::fstream file(getBatchPath(), std::ios::out);

    if (!file) {
        std::cerr << "Error opening file!" << std::endl;
        return 1; // or handle the error as needed
    }

    if (debug) file << "@echo on" <<endl;
    else file << "@echo off" <<endl;

    file << "cd " << dir.getFolder(commands.at(0)) << endl;
    if (debug) {
        file << "echo will run: \"" + comando + "\"" << endl;
        file << "pause" << endl;
    }
    file << comando << endl;
    file << "cd " << Constant::getAppDir() << endl;
    
    //if (debug) file << "pause" << endl;
    //file << Constant::getAppDir() << Constant::getFileSep() << dir.getFileName(argv0) << endl;
    
    file.close();
    return 0;
}

string Launcher::getBatchPath(){
    return Constant::getAppDir() //+ Constant::getFileSep() + "gmenu" 
        + Constant::getFileSep() + "run.bat";
}