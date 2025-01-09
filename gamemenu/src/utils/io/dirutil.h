#pragma once

#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
#include <memory>
//#include <filesystem>
#include <fstream>

#include "../compat/libs.h"
#include "fileprops.h"

#define STR_DIR_EXT "Dir"

class dirutil{
    public :
        dirutil();
        ~dirutil();
        bool isDir(const char* ruta);
        bool fileExists(const char* file);
        bool dirExists(const char* ruta);
        char * getDir(char *buffer);
        char * getDirActual();
        string getExtension(string file);
        bool setFileProperties(FileProps *propFile, string ruta);
        unsigned int listarFilesSuperFast(const char *strdir, vector<unique_ptr<FileProps>> &filelist, string filtro, bool order, bool properties);
        string getFileNameNoExt(string file);
        string getFolder(string file);
        string getFileName(string file);
        bool changeDirAbsolute(const char *str);
        bool borrarArchivo(string ruta);
    private:
        char rutaActual[PATH_MAX]; //Ruta actual que se esta navegando
        char* formatdate(char* str, time_t val);
        int findIcon(const char *filename);
        
};

dirutil::dirutil(){

}

dirutil::~dirutil(){
    //cout << "deleting dirutil" << endl;
}

/**
 * 
 * @param ruta
 * @return 
 */
bool dirutil::isDir(const char* ruta){
    //struct stat info;
    //stat(ruta, &info);
    //
    //if(S_ISDIR(info.st_mode)){
    //    return true;
    //} else{
    //    return false;
    //}

    #ifdef DOS
        //Much faster for dos
        DIR* dir = opendir(ruta);
        bool ret = dir != NULL;
        if (ret)
            closedir(dir);
        return ret;
    #else 
        struct stat info;
        stat(ruta, &info);
        
        if(S_ISDIR(info.st_mode)){
            return true;
        } else{
            return false;
        }
    #endif
}

/**
 * Check if a file exists
 * @return true if and only if the file exists, false else
 */
bool dirutil::fileExists(const char* file) {
    //return std::filesystem::exists(file);

    #ifdef DOS
        //Much faster for dos
        ifstream f(file);
        bool ret = f.good();
        if (ret)
            f.close();
        return ret;
    #else
        struct stat buf;
        return (stat(file, &buf) == 0);
    #endif
}

/**
* Comprueba si existe el directorio o fichero pasado por parametro
*/
bool dirutil::dirExists(const char* ruta){
    if(isDir(ruta)){
        return true;
    } else {
        return fileExists(ruta);
    }
}

/**
*/
char * dirutil::getDir(char *buffer){
    memset(buffer, '\0',FILENAME_MAX-1);
    return getcwd(buffer, PATH_MAX);
}

/**
*/
char * dirutil::getDirActual(){
    return getDir(rutaActual);
}


/**
* Obtiene la extension del fichero
*/
string dirutil::getExtension(string file){
    string ext = "";
    //if (!file.empty() && !isDir(file.c_str())){
    /**TODO: isDir is giving errors not detecting files */
    if (!file.empty()){
        size_t found = file.find_last_of(".");
        if (found > 0 && found < file.length()){
            ext = file.substr(found);
            Constant::lowerCase(&ext);
        }
    }
    return ext;
}

/**
 *
 * @param str
 * @param val
 * @return
 */
char* dirutil::formatdate(char* str, time_t val){
    int tam = 36;
    strftime(str, tam, "%d/%m/%y %H:%M", localtime(&val));
    return str;
}

bool dirutil::setFileProperties(FileProps *propFile, string ruta){
    bool ret = true;

    struct stat info;
    stat(ruta.c_str(), &info);

    if(S_ISDIR(info.st_mode)){
        propFile->filetype = TIPODIRECTORIO;
        propFile->extension = STR_DIR_EXT;
    } else {
        propFile->filetype = TIPOFICHERO;
        propFile->fileSize = (size_t)info.st_size;
        propFile->extension = getExtension(ruta);
    }
    char mbstr[36];
    propFile->creationTime = formatdate(mbstr, info.st_ctime);
    propFile->modificationTime = formatdate(mbstr, info.st_mtime);
    propFile->iCreationTime = time(&info.st_ctime);
    propFile->iModificationTime = time(&info.st_mtime);

    return ret;
}

int dirutil::findIcon(const char *filename){

    char ext[5] = {' ',' ',' ',' ','\0'};
    int len = 0;

    if (filename != NULL){
        len = strlen(filename);
        if ( len > 4){
            ext[3] = filename[len-1];
            ext[2] = filename[len-2];
            ext[1] = filename[len-3];
            ext[0] = filename[len-4];
        }
    }
    string data = ext;
    std::transform(data.begin(), data.end(), data.begin(), ::tolower);

    if (data.find(".txt") != string::npos || data.find(".inf") != string::npos){
        return page_white_text;
    } else if (data.find(".gpu") != string::npos || data.find(".gpe") != string::npos
        || data.find(".exe") != string::npos || data.find(".bat") != string::npos
        || data.find(".com") != string::npos){
        return page_white_gear;
    } else if (data.find(".gz") != string::npos || data.find(".z") != string::npos
        || data.find(".tar") != string::npos || data.find(".zip") != string::npos
        || data.find(".rar") != string::npos){
	    return page_white_compressed;
	} else if (data.find(".bmp") != string::npos || data.find(".jpg") != string::npos
        || data.find(".jpeg") != string::npos || data.find(".png") != string::npos
        || data.find(".gif") != string::npos ){
        return page_white_picture;
    } else if (data.find(".bin") != string::npos){
        return page_white_zip;
    } else {
        return page_white;
    }

}

/**
 *
 * @param strdir
 * @param filelist
 * @param filtro
 * @param superfast
 * @return
 */
unsigned int dirutil::listarFilesSuperFast(const char *strdir, vector<unique_ptr<FileProps>> &filelist, string filtro, bool order, bool properties){
    DIR *dp;
    struct dirent *dirp;
    unsigned int totalFiles = 0;

    if (!dirExists(strdir))
        return 0;

    string parentDir = strdir;
    //Miramos a ver si el directorio a explorar tiene una / al final
    if (strdir != NULL){
        if (!parentDir.empty() && parentDir.at(parentDir.length()-1) != Constant::FILE_SEPARATOR){
            parentDir.append(Constant::tempFileSep);
        }
        string extension;

        if((dp  = opendir(strdir)) == NULL) {
            //Traza::print("Error al listar el directorio: " + string(strdir), W_ERROR);
            return 0;
        } else {
            //Traza::print("Recorriendo ficheros", W_PARANOIC);
            while ((dirp = readdir(dp)) != NULL) {
                string concatDir = parentDir + string(dirp->d_name);
                //if (!isDir(concatDir)){
                    extension = getExtension(dirp->d_name);
                    if (filtro.empty() || (filtro.find(extension) != string::npos && extension.length() > 1)){
                        FileProps propFile(strdir, dirp->d_name, findIcon(dirp->d_name), TIPOFICHERO);
                        if (properties){
                            setFileProperties(&propFile, concatDir);
                            propFile.ico = findIcon(dirp->d_name);
                            propFile.filetype = TIPOFICHERO;
                        }
                        filelist.emplace_back(make_unique<FileProps>(propFile));
                    }
                //}
            }
            closedir(dp);
            totalFiles = filelist.size();
            if (order) {
                std::sort (filelist.begin(), filelist.end(), FileProps::sortByTextUnique);
            }
        }
    }
    return totalFiles;
}


string dirutil::getFileNameNoExt(string file){
    if(isDir(file.c_str())){
        return file;
    } else {
        size_t found, foundExt;
        
        char sep = Constant::FILE_SEPARATOR;
        if (file.find(sep) == string::npos && file.find(Constant::FILE_SEPARATOR_UNIX) != string::npos){
            sep = Constant::FILE_SEPARATOR_UNIX;
        } else if (file.find(sep) == string::npos && file.find(0x5C) != string::npos){
            sep = 0x5C;
        }
        
        found = file.find_last_of(sep);
        foundExt = file.find_last_of(".");

        if (found == string::npos){
            found = 0;
        }

        if (found > 0 && foundExt > found){
            string name = file.substr(found  + 1, foundExt - found - 1);
            return name;
        } else if (foundExt > found){
            string name = file.substr(0, foundExt);
            return name;
        } else {
            return file;
        }
    }
}

/**
* Obtiene el directorio de un fichero
*/
string dirutil::getFolder(string file){
    if(isDir(file.c_str())){
        return file;
    } else {
        size_t found;

        char sep = Constant::FILE_SEPARATOR;
        if (file.find(sep) == string::npos && file.find(Constant::FILE_SEPARATOR_UNIX) != string::npos){
            sep = Constant::FILE_SEPARATOR_UNIX;
        } else if (file.find(sep) == string::npos && file.find(0x5C) != string::npos){
            sep = 0x5C;
        }

        found = file.find_last_of(sep);
        if (found > 0){
            return file.substr(0,found);
        } else {
            return file;
        }
    }
}

/**
* Obtiene el directorio de un fichero
*/
string dirutil::getFileName(string file){
    if(isDir(file.c_str())){
        return file;
    } else {
        size_t found;

        char sep = Constant::FILE_SEPARATOR;
        if (file.find(sep) == string::npos && file.find(Constant::FILE_SEPARATOR_UNIX) != string::npos){
            sep = Constant::FILE_SEPARATOR_UNIX;
        } else if (file.find(sep) == string::npos && file.find(0x5C) != string::npos){
            sep = 0x5C;
        }

        found = file.find_last_of(sep);
        if (found != string::npos){
            return file.substr(found  + 1, file.length() - found - 1);
        } else {
            return file;
        }
    }
}

/**
* Devuelve true si se ha hecho el cambio al directorio.
* False si no se ha podido hacer el cambio. P.ejm: Cambio por un fichero
*/
bool dirutil::changeDirAbsolute(const char *str){
    if(isDir(str)){
        return (chdir(str) != -1);
    } else {
        return false;
    }
}

/**
*
*/
bool dirutil::borrarArchivo(string ruta){

    if (isDir(ruta.c_str()))
        return false;
    else {
        if (fileExists(ruta.c_str()))
            return (remove(ruta.c_str()) != 0) ? false : true;
        else
            return false;
    }
}