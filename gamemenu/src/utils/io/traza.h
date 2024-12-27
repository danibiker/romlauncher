#pragma once

#include "utils/const/Constant.h"
#include "stdio.h"

class Traza{
    public:
        enum {
            T_ALL = 0,
            T_INFO,
            T_DEBUG,
            T_ERROR
        } ErrorLevel;

        static int level;

        Traza(){
            
        }

        ~Traza(){
            close();
        }
        
        static void print(int l){
            if (fp != NULL && l >= level){
                //sprintf(log_message, "error trying set resolution %dx%d: \"%s\"", w, h, allegro_error);
                fprintf(fp, "%s\n", log_message);
                if (level == T_DEBUG) fflush(fp);
            }
        }

        static void print(){
            print(T_DEBUG);
        }

        static void close(){
            if (fp != NULL){
                fclose(fp);
                fp = NULL;
            }
        }
        static void open(){
            level = T_ERROR;
            // Open the log file in append mode (a+)
            if (fp == NULL){
                fp = fopen( (Constant::getAppDir() + Constant::getFileSep() + "log.txt").c_str() , "a+");
                if (fp == NULL) {
                    printf("Error opening log file!\n");
                }
            }
        }
        static char log_message[256]; // log message buffer
    private:
        static FILE *fp; // file pointer
};
FILE * Traza::fp = NULL; // separate definition
char Traza::log_message[256];
int Traza::level;
