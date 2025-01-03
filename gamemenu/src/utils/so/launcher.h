#pragma once

#include <allegro.h>
#include "utils/const/Constant.h"
#include "utils/io/dirutil.h"
#include "utils/beans/structures.h"

//#include "utils/unzip/unziptool.h"

#ifdef __GNUC__
#include <unistd.h>
#else
#include <process.h>
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
        bool launch(vector<string> &commands, bool debug, string argv0);
        string descomprimirZIP(string filename);
    private:
        Executable rutaEspecial(string ejecutable, string param, string filerompath);
        void deleteUnzipedRom(string romfile);
        int dosbatch(vector<string> &commands, string comando, bool debug, string argv0);
        string getBatchPath();
};  


/**
* Realiza un lanzamiento del programa que se encontro al parsearArgumentos
*/
/*
bool Launcher::lanzarProgramaUNIXFork(FileLaunch &emulInfo){

    string ejecutable;
    string param;
    string rom;

    ejecutable = emulInfo.rutaexe;
    if (emulInfo.rutaexe.length() > 0 && emulInfo.fileexe.length() > 0){
        ejecutable += (emulInfo.rutaexe.at(emulInfo.rutaexe.length()-1) == FILE_SEPARATOR
               || emulInfo.fileexe.at(emulInfo.fileexe.length()-1) == FILE_SEPARATOR ? "" : tempFileSep);
    }
    ejecutable += emulInfo.fileexe;

    param = emulInfo.parmsexe;

    rom = emulInfo.rutaroms;
    if (emulInfo.rutaroms.length() > 0 && emulInfo.nombrerom.length() > 0){
            rom += (emulInfo.rutaroms.at(emulInfo.rutaroms.length()-1) == FILE_SEPARATOR
               || emulInfo.nombrerom.at(emulInfo.nombrerom.length()-1) == FILE_SEPARATOR ? "" : tempFileSep);
    }
    rom += emulInfo.nombrerom;


    #ifdef UNIX

        int extPos = rom.rfind(".");
        bool isZipped = false;
        if (extPos > 0){
            string extension = rom.substr(extPos);
            Constant::lowerCase(&extension);
            if (extension.compare(".zip") == 0){
                isZipped = true;
            }
        }

//        log( "*************Launcher::lanzarProgramaUNIX*************");
        string buffer = "";
        if (emulInfo.descomprimir){
            buffer = descomprimirZIP(rom);
        } else {
            buffer = rom;
        }

        dirutil  dir;
        dir.changeDirAbsolute(emulInfo.rutaexe.c_str());
        Executable execCmd = rutaEspecial(ejecutable, param, buffer);
        bool errorLaunch = false;
        
        string comando = execCmd.comandoFinal;
        cout << "Vamos a lanzar el comando: " + comando <<endl;
        
        //int ret = 0;
        if (Constant::getExecMethod() == launch_spawn || Constant::getExecMethod() == launch_create_process){
            pid_t pid = fork();
            switch(pid){
                case -1 : // Error
                        errorLaunch = true;
                        cerr << "EXIT_FAILURE" <<endl;
                        break;
                case 0 :
                        cout << "Child exiting" <<endl;
                        // Do what you want
                        // Call one of the exec -- personally I prefer execlp
                        if (execCmd.filenameinparms){
                            const char* argv[] = { execCmd.ejecutable.c_str(), execCmd.param.c_str(), NULL };
                            execl(argv[0], argv[0], argv[1], NULL);
                        } else {
                            const char* argv[] = { execCmd.ejecutable.c_str(), execCmd.param.c_str(), execCmd.filerompath.c_str(), 0 };
                            execl(argv[0], argv[0], argv[1], NULL);
                        }
                        break;
                default :
                        //printf("Child is PID %jd\n", (intmax_t) pid);
                        cout << "Parent exiting" <<endl;
                        break;  
            }
        } else if (Constant::getExecMethod() == launch_system ){
            if (system(comando.c_str()) != 0){
                errorLaunch = true;
                cerr << "Error al lanzar el comando" <<endl;
            }
        }
        
        if(errorLaunch){
            return false;
        } else {
            //if (emulInfo.descomprimir && isZipped){
            //    cout << "Borrando el fichero: " + buffer <<endl;
            //    deleteUnzipedRom(buffer);
            //}
            return true;
        }
        
    #elif WIN
        Traza::print( "*************Launcher::lanzarProgramaUNIX - En Windows*************", W_DEBUG);
        string buffer = "";

        int extPos = rom.rfind(".");
        bool isZipped = false;
        if (extPos > 0){
            string extension = rom.substr(extPos);
            Constant::lowerCase(&extension);
            Traza::print( "extension: " + extension, W_DEBUG);
            if (extension.compare(".zip") == 0){
                isZipped = true;
            }
        }

        if (emulInfo.descomprimir){
            buffer = descomprimirZIP(rom);
        } else {
            buffer = rom;
        }

        Traza::print("Launcher::launch. Lanzando ejecutable: " + ejecutable, W_INFO);
        Traza::print("Launcher::launch. Lanzando parmsexe: " + param, W_INFO);
        Traza::print("Launcher::launch. Lanzando rom: " + buffer, W_INFO);

        Dirutil dir;
        dir.changeDirAbsolute(emulInfo.rutaexe.c_str());
        Executable execCmd = rutaEspecial(ejecutable, param, buffer);
        bool errorLaunch = false;

        
        //P_OVERLAY 	Overlays parent process with child, which destroys the parent. This has the same effect as the exec* functions.
        //P_WAIT 	Suspends parent process until the child process has finished executing (synchronous spawn).
        //P_NOWAIT, P_NOWAITO 	Continues to execute calling process concurrently with new process (asynchronous spawn).
        //P_DETACH 	the child is run in background without access to the console or keyboard. Calls to _cwait upon the new process will fail (asynchronous spawn)
        
        if (Constant::getExecMethod() == launch_spawn){
            int pid = -1;
            if (execCmd.filenameinparms){
                char* argv[] = { (char *)execCmd.ejecutable.c_str(),(char *)execCmd.param.c_str(), 0 };
                pid = spawnv(P_WAIT, argv[0], argv);
            } else {
                char* argv[] = { (char *)execCmd.ejecutable.c_str(),(char *) execCmd.param.c_str(), (char *)execCmd.filerompath.c_str(), 0 };
                pid = spawnv(P_WAIT, argv[0], argv);
            }
            if (pid == -1){
                Traza::print("Launcher::launch. error" + string(strerror(errno)), W_ERROR);
                errorLaunch = true;
            }
        } else if (Constant::getExecMethod() == launch_system){
            string comando = execCmd.comandoFinal;
            Traza::print( "Vamos a lanzar el comando: " + comando, W_INFO);
            if (system(comando.c_str()) != 0){
                errorLaunch = true;
                Traza::print( "Error al lanzar el comando", W_ERROR);
            }
        } else if (Constant::getExecMethod() == launch_create_process){
            errorLaunch = createProcess(execCmd);
        }

        if(errorLaunch){
            return false;
        } else {
            if (emulInfo.descomprimir && isZipped){
                Traza::print( "Borrando el fichero: " + buffer, W_DEBUG);
                deleteUnzipedRom(buffer);
            }
            return true;
        }

    #endif
    // executed only by parent
    cout << "lanzarProgramaUNIX Fin" << endl;
    return true;
}*/

/**
* Descomprime el fichero .zip pasado por parametro y devuelve el nombre del fichero extraido
*/
string Launcher::descomprimirZIP(string filename){
    string filenameReturn = filename;
//    if (filename.find(".zip") != string::npos || filename.find(".ZIP") != string::npos){
//        //Descomprimir el fichero
//        UnzipTool unzipTool;
//        unzippedFileInfo salida = unzipTool.descomprimirZip(filename.c_str());
//        filenameReturn = salida.rutaEscritura;
//    }
    return filenameReturn;
}

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
void Launcher::deleteUnzipedRom(string romfile){
    if (romfile.compare("") != 0 && romfile.find(".zip") == string::npos && romfile.find(".ZIP") == string::npos){
        dirutil dir;
        if (dir.borrarArchivo(romfile))
            cout << "deleteUnzipedRom Fichero eliminado: " + romfile <<endl;
        else
            cerr << "No se ha encontrado el fichero a eliminar: " + romfile <<endl;
    }
}

/**
 * 
 */
bool Launcher::launch(vector<string> &commands, bool debug, string argv0){
    bool launchOk = false;
    string comando;
    clear_to_color(screen, backgroundColor);
    ALFONT_FONT *fontsmall = Fonts::getFont(Fonts::FONTSMALL);
    Constant::drawTextCentre(screen, fontsmall, "launching command", SCREEN_W / 2, SCREEN_H / 2, textColor, -1);

    for (size_t i=0; i < commands.size(); i++){
        string parm = Constant::Trim(commands.at(i));
        if (!parm.empty()){
            comando += (i > 0 ? " " : "") + parm;
        } 
        if (debug){
            Constant::drawTextCentre(screen, fontsmall, parm.c_str(), SCREEN_W / 2, SCREEN_H / 2 + (fontsmall->face_h + 3) * (i+1), textColor, -1);
        }
    }

    if (debug){
        Constant::drawTextCentre(screen, fontsmall, comando.c_str(), SCREEN_W / 2, SCREEN_H / 2 + (fontsmall->face_h + 3) * (commands.size()+1), textColor, -1);
        Constant::drawTextCentre(screen, fontsmall, "Press a key to continue", SCREEN_W / 2, SCREEN_H / 2 + (fontsmall->face_h + 3) * (commands.size()+2), textColor, -1);
        readkey();
    }

    if (Constant::getExecMethod() == launch_system ){
        launchOk = system(comando.c_str()) == 0;
        if (!launchOk){
            cerr << "Error al lanzar el comando" <<endl;
        }
    } else if (Constant::getExecMethod() == launch_spawn || Constant::getExecMethod() == launch_create_process){
        // extra room for program name and sentinel
        char **argv = new char* [commands.size()+1];  
        //by convention, argv[0] is program name
        argv[0] = strdup(commands.at(0).c_str());     
        size_t j = 1;
        for (; j < commands.size(); j++)     
            argv[j] = strdup(commands[j].c_str());
        // end of arguments sentinel is NULL
        argv [j] = NULL;  

        //char** longcmd = (char**)malloc(commands.size());
        //longcmd[0] = strdup(argv0.c_str());
        //int i;
        //for (i = 1; i < commands.size(); i++) {
        //    longcmd[i] = strdup(commands.at(i).c_str());
        //}
        //longcmd[i] = (char *)NULL;

        cout << "Child exiting" << endl;
        if ( execv(argv[0], argv) == -1){
            perror("execv failed");
            launchOk = false;
        } else {
            launchOk = true;
        }
        // Deallocate memory
        //free(longcmd);
        for (j = 0; j < commands.size(); j++)     
            free(argv[j]);

        delete [] argv;
    } else if (Constant::getExecMethod() == launch_batch && dosbatch(commands, comando, debug, argv0) == 0){
        allegro_exit();
        cout << "Exiting to launch the game: " << comando << endl;
        exit(0);
    }
    return launchOk;
}

int Launcher::dosbatch(vector<string> &commands, string comando, bool debug, string argv0){
    
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