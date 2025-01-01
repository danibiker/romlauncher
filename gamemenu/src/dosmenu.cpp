#include <stdio.h>
#include <stdlib.h> /* for exit */
#include <stdbool.h>
#include <unistd.h>
#include <process.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>

#define min(a, b) (((a) < (b)) ? (a) : (b)) /* for min*/


//#include <process.h>    /* for 'execl' */
//#include <stdio.h>      /* for 'printf' and 'NULL' */
//#include <errno.h>      /* for 'errno', 'ENOENT' and 'ENOMEM' */

int main(int argc, char *argv[]){

    //spawnlp(P_WAIT, "C:\\juegos\\emu\\CCS64\\ccs64.exe", "ccs64", "ROMS\\TAPES\\C64-DI~1\\EUROPE\\1942(E~1.D64", "-autorun", NULL);
    //spawnlp(P_WAIT, "command", "command", "/k", "C:\\juegos\\emu\\CCS64\\ccs64.exe", "ROMS\\TAPES\\C64-DI~1\\EUROPE\\1942(E~1.D64", "-autorun", NULL);
    //spawnlp(P_WAIT, "C:\\juegos\\emu\\genesis\\GENECYST.EXE", "GENECYST.EXE", NULL);

    /*FILE *fp; // file pointer
    char log_message[256]; // log message buffer

    // Open the log file in append mode (a+)
    fp = fopen("log.txt", "a+");
    if (fp == NULL) {
        printf("Error opening log file!\n");
        return 1;
    }

    fprintf(fp, "%s\n", "Before launching game");
    fflush(fp);

    //system("./RUN.BAT");
    //execl("child.exe", "child", "arg1", "arg2", NULL);
    //execl("./RUN.BAT", "RUN", NULL);
    //execl("C:\\juegos\\emu\\CCS64\\ccs64.exe", "ccs64", "ROMS\\TAPES\\C64-DI~1\\EUROPE\\1942(E~1.D64", "-autorun", NULL);
    spawnl(P_WAIT, "C:\\juegos\\emu\\CCS64\\ccs64.exe", "ccs64", "ROMS\\TAPES\\C64-DI~1\\EUROPE\\1942(E~1.D64", "-autorun", NULL);

    if (errno == ENOENT){
        //printf("child.exe not found in current directory\n");
        fprintf(fp, "%s\n", "child.exe not found in current directory");
    } else if (errno == ENOMEM){
        //printf("not enough memory to execute child.exe\n");
        fprintf(fp, "%s\n", "not enough memory to execute child.exe");
    } else {
        //printf("error #%d trying to exec child.exe\n", errno);
        sprintf(log_message, "error #%d trying to exec child.exe\n", errno);
        fprintf(fp, "%s\n", log_message);
    }
    fflush(fp);
    fclose(fp);
    */

    const char* batFile = "/RUN.BAT";
    const char* menuFile = "/gamemenu.exe";

    bool showMenu = true;
    struct stat buf;
    char cwd[PATH_MAX];
    char *BAT_FILE;
    char *EXE_MENU;

    memset(cwd, '\0',PATH_MAX-1);

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
       perror("getcwd() error");
       return 1;
    }

    //Setting the routes for the menu and the bat file
    const size_t batLen = min(strlen(cwd) + strlen(batFile), PATH_MAX);
    const size_t menuLen = min(strlen(cwd) + strlen(menuFile), PATH_MAX);


    BAT_FILE = (char *)malloc(batLen + 1);
    EXE_MENU = (char *)malloc(menuLen + 1);
    memset(BAT_FILE, '\0', batLen);
    memset(EXE_MENU, '\0', menuLen);
    strcpy(BAT_FILE, cwd);
    strcat(BAT_FILE, batFile);
    strcpy(EXE_MENU, cwd);
    strcat(EXE_MENU, menuFile);

    while (showMenu){
        printf("launching the menu...\n");

        //if (spawnl(P_WAIT, EXE_MENU, EXE_MENU, NULL) == -1) {
        //    perror("Failed to launch gamemenu.exe");
        //    pause();
        //    exit(1);
        //}

        if (system(EXE_MENU) != 0){
            perror("Failed to launch gamemenu.exe");
            return 1;
        }


        if ((showMenu = stat(BAT_FILE, &buf) == 0)){
            printf("launching the game...\n");
            //if (spawnl(P_WAIT, BAT_FILE, BAT_FILE, NULL) == -1) {
            if (system(BAT_FILE) != 0){
                perror("Failed to launch run.bat");
                //pause();
                return 1;
            }
            remove(BAT_FILE);
        } else {
            //printf("There is no bat. Exiting...\n");
            showMenu = false;
            //pause();
        }
    }
    free(BAT_FILE);
    free(EXE_MENU);

    return 0;
}
