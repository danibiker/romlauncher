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

int main(int argc, char *argv[]){
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
