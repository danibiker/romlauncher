#include <string>
#include <time.h>
#include <thread>
#include "utils/const/Constant.h"
#include "utils/io/dirutil.h"
#include "allegro/timer.h"

class SOUtils{
    public :
        SOUtils(){};
        ~SOUtils(){};
        static string iniciarSistema(char** argv);
        static void waitms(unsigned long ms);
};

/**
 *
 * @param argv
 */
string SOUtils::iniciarSistema(char** argv){
    dirutil dir;

    string appDir = argv[0];
    size_t pos = appDir.rfind(Constant::getFileSep());
    if (pos == string::npos){
        pos = appDir.rfind(FILE_SEPARATOR_UNIX);
        #if defined(WIN) || defined(DOS)
            appDir = Constant::replaceAll(appDir, "/", "\\");
        #elif UNIX
            FILE_SEPARATOR = FILE_SEPARATOR_UNIX;
            tempFileSep[0] = FILE_SEPARATOR_UNIX;
        #endif
    }
    appDir = appDir.substr(0, pos);

    if (!dir.dirExists(appDir.c_str()) || pos == string::npos){
        appDir = dir.getDirActual();
    }
    Constant::setAppDir(appDir);
    return appDir;
}

void SOUtils::waitms(unsigned long ms){
        rest(ms);
    //#else
    //    auto delay = std::chrono::duration<double, std::milli>(ms);
    //    std::this_thread::sleep_for(delay);
    //#endif
}