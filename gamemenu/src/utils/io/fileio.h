#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <zlib.h>

#include "messages.h"
#include "../compat/libs.h"

#define CHUNKZIP 5120
#define MAXLINELEN 4096



/**
 * Reads a file and return the first line encountered. It can be called secuentially to 
 * obtain all the file's lines.
 * 
 * Parameters:
 * fp: Pointer that stores the file position
 * 
 * Return:
 * char *: Return the next line. It should be freed with free() to avoid memory leaks
 * 
 * */
char * readFileLine(FILE *fp){
    
    char buffer[MAXLINELEN];
    char *line = NULL;
    char *ptrLine = NULL;
    unsigned int tamAlloc = MAXLINELEN;
    int newLine = 0;

    if (fp && !feof(fp)){
        line = (char *)malloc(MAXLINELEN);
        if (line != NULL)
            memset(line, 0, MAXLINELEN);

        do {
            ptrLine = fgets(buffer, MAXLINELEN, fp);
            if (ptrLine){
                newLine = 0;
                int lenBuff = strlen(buffer) - 1;
                
                while (lenBuff > 0 && (buffer[lenBuff] == '\r' || buffer[lenBuff] == '\n')){
                    newLine = 1;
                    buffer[lenBuff] = '\0';
                    lenBuff--;
                }
                if (line != NULL) {
                    strcat_s(line, MAXLINELEN, buffer);
                    if (!newLine && tamAlloc < strlen(line) + MAXLINELEN) {
                        tamAlloc += MAXLINELEN;
                        char* tmp;
                        tmp = (char*)realloc(line, tamAlloc);
                        if (tmp != NULL)
                            line = tmp;
                    }
                }
            }
        } while (!newLine && ptrLine);
    }
    return line;
}

/**
 *  Example to read a file line by line
 * */
// void printfFile (char * filename){
//     FILE *fp = fopen(filename, "rt");
//     char *linea;

//     while ((linea = readFileLine(fp)) != NULL){
//         printf("%s\n", linea);
//         free(linea);
//     }
//     fclose(fp);
// }

/**
* Writes the specified amount of data of the memory buffer to disk
*
* ios::in	Open for input operations.
* ios::out	Open for output operations.
* ios::binary	Open in binary mode.
* ios::ate	Set the initial position at the end of the file. If this flag is not set to any value, the initial position is the beginning of the file.
* ios::app	All output operations are performed at the end of the file, appending the content to the current content of the file. This flag can only be used in streams open for output-only operations.
* ios::trunc	If the file opened for output operations already existed before, its previous content is deleted and replaced by the new one.
*/
int writeToFile(const char *uri, char * memblocktowrite, size_t tam, int append){
    int ret = 0;
    FILE *file;
    errno_t err;

    err = fopen_s(&file, uri, "wb");

    if (err == 0 && file != NULL){
        if (append) {
            fseek(file, 0, SEEK_END);
        }
        fwrite(memblocktowrite, 1, tam, file);
        fclose(file);
        ret = 1;
        printf("writeToFile: downloaded file in: %s\n", uri);
    } else {
        printf("writeToFile: file: %s not found or could not be opened", uri);
        decodeError(err);
    }
    #ifdef GP2X
        sync();
    #endif
    return ret;
}


/**
 * Uncompress a gzipped memory block into a new array.
 * CAUTION: If succeed extracting the gzziped file, it replaces the original content
 * with the uncompressed one
 * */
char * gunzipFromMem(char **memblocktoread, size_t *tam){
    size_t size = 0;
    int ret;
    unsigned have;
    z_stream strm;
    unsigned char out[CHUNKZIP];
    char *data = (char *) malloc(CHUNKZIP);

    if (data == NULL)
        return NULL;

    memset(data, 0, CHUNKZIP);

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;

    /*windowBits can also be greater than 15 for optional gzip decoding. Add 32 to windowBits 
    to enable zlib and gzip decoding with automatic header detection, or add 16 to decode only 
    the gzip format (the zlib format will return a Z_DATA_ERROR). If a gzip stream is being 
    decoded, strm->adler is a crc32 instead of an adler32.*/
    ret = inflateInit2(&strm, 16+MAX_WBITS);

    if (ret != Z_OK)
        return *memblocktoread;
    
    strm.avail_in = *tam;
    strm.next_in = (unsigned char *)*memblocktoread;

    /* run inflate() on input until output buffer not full */
    do {
        strm.avail_out = CHUNKZIP;
        strm.next_out = out;
        ret = inflate(&strm, Z_NO_FLUSH);
        //assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
        switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
                printf("Z_NEED_DICT %d\n", ret);
            case Z_DATA_ERROR:
                printf("Z_DATA_ERROR %d\n", ret);
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return *memblocktoread;
        }
        have = CHUNKZIP - strm.avail_out;
        char* tmp;
        tmp = (char *)realloc(data, strm.total_out);
        if (tmp != NULL) {
            data = tmp;
            memcpy(data + size, out, have);
        }
        size = strm.total_out; 
    } while (strm.avail_out == 0);
    /* clean up and return */
    (void)inflateEnd(&strm);

    if (size > 0 && data != NULL){
        //printf("Uncompressing %d bytes from a compressed %d bytes file\n", size, *tam);
        free(*memblocktoread);
        *memblocktoread = (char *) malloc(size + 1);
        if (*memblocktoread == NULL){
            //printf("gunzipFromMem::Error reserving %d bytes\n", size);
            return NULL;
        }
        memcpy(*memblocktoread, data, size);
        (*memblocktoread)[size] = 0; //Important to strlen for example
        *tam = size;
        free(data);
    }
    return *memblocktoread;
}

/**
 * 
 * */
long getFileSize(const char *uri){
    int ret = 0;
    FILE *file;
    errno_t err;

    err = fopen_s(&file, uri, "rb");
    if (err == 0 && file != NULL){
        if (fseek(file, 0, SEEK_END) == 0)
            ret = ftell(file);
        
        fclose(file);
    } else {
        printf("getFileSize: file: %s not found or could not be opened", uri);
        decodeError(err);
    }

    return ret;
}

char * loadFromFile(const char *uri, size_t *size){
    char *ret = NULL;
    FILE *file;
    errno_t err;
    long filetam = getFileSize(uri);

    err = fopen_s(&file, uri, "rb");
    if (err == 0 && file != NULL && filetam > 0){
        ret = (char *)malloc(filetam);
        if (ret != NULL){
            *size = fread(ret, 1, filetam, file);
        }
    } else {
        printf("getFileSize: file: %s not found or could not be opened", uri);
        decodeError(err);
    }
    return ret;

}