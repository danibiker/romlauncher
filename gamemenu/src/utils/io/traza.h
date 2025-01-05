#pragma once

#include "utils/const/Constant.h"
#include <iostream>
#include <stdio.h>

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
            cout << "deleting Traza" << endl;
            close();
        }
        
        static void print(int l, string message){
            if (fp != NULL && l >= level){
                fprintf(fp, "%s\n", message.c_str());
                if (level == T_DEBUG) fflush(fp);
            }
        }

        static void print(int l){
            if (fp != NULL && l >= level){
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

