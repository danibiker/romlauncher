#pragma once

#include <stdio.h>
#include <string.h>

#if (defined(UNIX) || defined(MSDOS)) && !defined(__STDC_LIB_EXT1__)
    int strncpy_s(char *dest, size_t destsz,
                   const char *src, size_t count){
        return strncpy(dest, src, count) != NULL ? 0 : 1;
    } 

    int strcpy_s(char *dest, size_t destsz,
                   const char *src){
        return strncpy(dest, src, destsz) != NULL ? 0 : 1;
    }

    int strcat_s(char *dest, size_t destsz, const char *src){
        return strncat(dest, src, destsz) != NULL ? 0 : 1;
    }

    int fopen_s( FILE ** streamptr, const char *filename, const char *mode ){
        *streamptr = fopen(filename, mode);
        return *streamptr != NULL ? 0 : 1;
    }

    char* strerror_s(char *buf, size_t buflen, int errnum){
        return (char *)strerror_r(errnum, buf, buflen);
    } 
    #define errno_t int
#endif