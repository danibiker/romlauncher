#pragma once 

#include "utils/io/fileio.h"
#include "utils/io/dirutil.h"
#include "utils/io/fileprops.h"

#include <unistd.h>
#include <zlib.h>
#include <minizip/unzip.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>


using namespace std;

struct unzippedFileInfo{
    int errorCode;
    string rutaEscritura;
    unsigned long romsize;
    uint32_t nFilesInZip;
    uint32_t nFilesWritten;
    vector<FileProps> files;
};

class UnzipTool
{
    public:
        UnzipTool();
        virtual ~UnzipTool();
        unzippedFileInfo descomprimirZip(const char *);
        unzippedFileInfo descomprimirZip(const char *rutaZip, const char *rutaDest);
        unzippedFileInfo descomprimirZip(const char *rutaZip, const char *rutaDest, bool uncompress);
        unzippedFileInfo listZipContent(const char *rutaZip);

    protected:
    private:
        unzippedFileInfo extraerFichero(unzFile *myZip, const char *ruta, bool uncompress);
        unzippedFileInfo extraerFichero(unzFile *myZip, const char *ruta);
};

UnzipTool::UnzipTool()
{
    //ctor
}

UnzipTool::~UnzipTool()
{
    //dtor
}

unzippedFileInfo UnzipTool::descomprimirZip(const char *ruta){
    return descomprimirZip(ruta, ruta);
}

unzippedFileInfo UnzipTool::descomprimirZip(const char *rutaZip, const char *rutaDest){
    return descomprimirZip(rutaZip, rutaDest, true);
}

unzippedFileInfo UnzipTool::listZipContent(const char *rutaZip){
    return descomprimirZip(rutaZip, rutaZip, false);
}

unzippedFileInfo UnzipTool::descomprimirZip(const char *rutaZip, const char *rutaDest, bool uncompress){
    unzFile myZip = NULL;
    myZip = unzOpen(rutaZip);
    unz_global_info zip_global_info;
    int ret;
    dirutil dir;

    unsigned long nFilesInZip = 0;
    unzippedFileInfo retorno;
    unzippedFileInfo tempRetorno;

    tempRetorno.errorCode = -1;
    tempRetorno.rutaEscritura = rutaZip;
    tempRetorno.romsize = 0;
    tempRetorno.nFilesInZip = 0;
    tempRetorno.nFilesWritten = 0;
    
    retorno.errorCode = -1;
    retorno.rutaEscritura = rutaZip;
    retorno.romsize = 0;
    retorno.nFilesInZip = 0;
    retorno.nFilesWritten = 0;


    if (myZip == NULL){
        cerr << "UnzipTool::descomprimirZip. Unable to open zip file: " + string(rutaZip) << endl;
        return(retorno);
    }
    else{
        cout << "UnzipTool::descomprimirZip. File found: " + string(rutaZip) << endl;
    }

    ret = unzGetGlobalInfo(myZip, &zip_global_info);
    if (ret != UNZ_OK){
        cerr << "unzGetGlobalInfo() call failed." + nFilesInZip << endl;
        return(retorno);
    } else {
        nFilesInZip = zip_global_info.number_entry;
        tempRetorno.nFilesInZip = nFilesInZip;
        cout << "Found files in archive." + nFilesInZip <<endl;
    }

    ret = unzGoToFirstFile(myZip);

    if (ret != UNZ_OK)
    {
        cerr << "unzGoToFirstFile() call failed." << endl;
        return(retorno);
    }

    while (ret == UNZ_OK){
        retorno = extraerFichero(&myZip, rutaDest, uncompress);
        if (retorno.romsize > tempRetorno.romsize){
            tempRetorno.errorCode = retorno.errorCode;
            tempRetorno.romsize = retorno.romsize;
            tempRetorno.rutaEscritura = retorno.rutaEscritura;
        }
        
        if (retorno.errorCode == 0){
            tempRetorno.nFilesWritten++;
        }
        
        if (!uncompress){
            FileProps fileprop;
            fileprop.filename = dir.getFileName(retorno.rutaEscritura);
            fileprop.dir = dir.getFolder(retorno.rutaEscritura);
            fileprop.fileSize = retorno.romsize;
            tempRetorno.files.push_back(fileprop);
        }
        
        
        ret = unzGoToNextFile(myZip);
    }

    unzClose(myZip);
    //unzCloseCurrentFile(myZip);
    
    
    return tempRetorno;
}



#define MAX_FILENAME 512

/**
*
*/
unzippedFileInfo UnzipTool::extraerFichero(unzFile *myZip, const char *ruta){
    return extraerFichero(myZip, ruta, true);
}

unzippedFileInfo UnzipTool::extraerFichero(unzFile *myZip, const char *ruta, bool uncompress){
    int ret;
    unsigned long rom_size = 0;
    char filename[MAX_FILENAME];
    char *cartridge;
    unzippedFileInfo retorno;
    retorno.errorCode = -1;
    retorno.rutaEscritura = ruta;
    retorno.romsize = 0;
    unz_file_info zip_file_info;

    ret = unzGetCurrentFileInfo(*myZip, &zip_file_info, filename, MAX_FILENAME, NULL, 0, NULL, 0);

    if (ret == UNZ_OK){
        cout << "Uncompressed size is in bytes" + zip_file_info.uncompressed_size <<endl;
        rom_size = zip_file_info.uncompressed_size;
        retorno.romsize = rom_size;
    }

    ret = unzOpenCurrentFile(*myZip);
    if (ret != UNZ_OK){
        cerr << "Unable to open file from zip archive." <<endl;
        return(retorno);
    }
    
    if (uncompress){
        cartridge = new char[rom_size];
        if (cartridge == NULL){
            cerr << "Unable allocate memory for cartridge." <<endl;
            return(retorno);
        }

        // Load rom file.
        ret = unzReadCurrentFile(*myZip, cartridge, rom_size);
        dirutil dir;
        string filenameOut = dir.getFolder(ruta) + FILE_SEPARATOR + filename;

        if (writeToFile(filenameOut.c_str(), cartridge, rom_size, false) == 1){
            retorno.rutaEscritura = filenameOut;
            retorno.errorCode = 0;
        } else {
            cerr << "Error UnzipTool::extraerFichero" <<endl;
        }
        delete [] cartridge;
    } else {
        retorno.rutaEscritura = filename;
    }

    return retorno;
}
