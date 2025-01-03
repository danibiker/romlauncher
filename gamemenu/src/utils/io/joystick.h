#pragma once

//Not needed
//#ifdef DOS
//    #include <allegro/platform/aldos.h>
//#endif
#include <allegro.h>
#include <utils/const/Constant.h>
#include "utils/font/fonts.h"

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

class Joystick{
    public:
        enum {J_UP = 0,J_DOWN,J_LEFT,J_RIGHT,J_A,J_B,/*J_X,J_Y,*/J_LB,J_RB,J_SELECT,J_START,J_NONE} jbuttons;
        const char *jbuttonsTxt[J_NONE + 1] = {"Up", "Down", "Left", "Right", "Accept", "Cancel", 
            //"X (No action)", "Y (No action)", 
            "Previous emulator", "Next Emulator", "Select", "Start", "None"};
        Joystick();
        ~Joystick();

        int init();
        int readJoystick();
        int getButtonStat(int btn);
        bool isJoyPressed();
        bool isJoyReleased();
        static uint32_t last;
        void resetButtons();

    private:
        const uint32_t MAXJOYPAUSE = 700;
        const uint32_t SHORTJOYPAUSE = 35;
        const char *idBtnChr = "btn_";
        const char *idHatChr = "hat_";
        const char *idHatRestChr = "hatRest_";
        const char *numBtnChr = "numBtns";
        const char *idAnalogChr = "analog";
        const unsigned int joyPos0 = 0;
        uint32_t longJoyPause;
        int analogmode;

        bool readConfig();
        bool writeConfig();
        int getAxisPos(int nStick, int nAxis);
        int shouldExit();
        int shouldGetNext();
        int configureButtons();

        int joypress, joyrelease;
        int* arrButtons;
        int jbuttonsStat[J_NONE + 1];
        int jbuttonsPrevStat[J_NONE + 1];
        struct t_hats{
            t_hats(){
                pressed = false;
            }
            int arrHats[2][2];
            bool pressed;
        } *hats;
};

Joystick::Joystick(){
    arrButtons = NULL;
    joypress = 0;
    joyrelease = 0;
    analogmode = -1;
    longJoyPause = 0;
    memset(jbuttonsStat, 0, sizeof(jbuttonsStat));
    memset(jbuttonsPrevStat, 0, sizeof(jbuttonsPrevStat));
}
Joystick::~Joystick(){
    if (arrButtons != NULL){
        delete [] arrButtons;
        delete [] hats;
    }
    remove_joystick();
}

void Joystick::resetButtons(){
    memset(jbuttonsStat, 0, sizeof(jbuttonsStat));
    joypress = 0;
    joyrelease = 0;
}

int Joystick::init(){
    if (arrButtons != NULL){
        delete [] arrButtons;
        delete [] hats;
    }

    int joyType = JOY_TYPE_AUTODETECT;
    #ifdef DOS
        joyType = JOY_TYPE_8BUTTON;
    #endif
      
    /* the first thing is to initialise the joystick driver */
    if (install_joystick(joyType) != 0) {
        sprintf(Traza::log_message, "Info initialising joystick\n %s", allegro_error);
        Traza::print();
        return 1;
    }

    /* make sure that we really do have a joystick */
    if (!num_joysticks) {
        sprintf(Traza::log_message, "Info: joystick not found");
        Traza::print();
        return 1;
    }

    sprintf(Traza::log_message, "Detected joystick \"%s\" with %d buttons", joystick_driver->name, joy[joyPos0].num_buttons);
    Traza::print();

    arrButtons = new int[joy[joyPos0].num_buttons];
    hats = new t_hats[joy[joyPos0].num_sticks];

    std::fill(arrButtons, arrButtons + joy[joyPos0].num_buttons, J_NONE);

    for (int i=0; i < joy[joyPos0].num_sticks; i++){
        hats[i].arrHats[0][0] = J_NONE;
        hats[i].arrHats[0][1] = J_NONE;
        hats[i].arrHats[1][0] = J_NONE;
        hats[i].arrHats[1][1] = J_NONE;
    }

    if (!readConfig() && configureButtons()){
        sprintf(Traza::log_message, "Writing config");
        Traza::print();
        writeConfig();
    }
    
    //Needed to poll the joystick once to avoid repeating the last press
    //when the app was closed on the next app open. Very weird allegro thing
    readJoystick();
    resetButtons();
    return 0;
}

/**
 * 
 */
bool Joystick::writeConfig(){
    std::ofstream file(Constant::getAppDir() + Constant::getFileSep() + "joy.ini", std::ios_base::trunc);

    if (file.is_open()) {
        file << "name" << "=" << joystick_driver->name << endl;
        for (int i=0; i < joy[joyPos0].num_buttons; i++){
            file << idBtnChr << i << "=" << arrButtons[i] << endl;
        }

        for (int nStick=0; nStick < joy[joyPos0].num_sticks; nStick++){
            for (int i=0; i < 2; i++){
                for (int j=0; j < 2; j++){
                    file << idHatChr  << nStick << "_" << i << "_" << j << "=" << hats[nStick].arrHats[i][j] << endl;
                }
            }
        }
        file << numBtnChr << "=" << joy[joyPos0].num_buttons << endl;
        file << idAnalogChr << "=" << analogmode << endl;
        file.close();
        return true;
    }
    return false;
}

/**
 * 
 */
bool Joystick::readConfig(){
    std::ifstream file(Constant::getAppDir() + Constant::getFileSep() + "joy.ini");
    if (file.is_open()) {
        vector<string> v;
        string line;
        int buttonsCfg = 0;
        int buttonsSaved = 0;
        while (std::getline(file, line)) {
            vector<string> nameVal = Constant::splitChar(line, '=');
            size_t pos = 0;
            if ((pos = nameVal.at(0).find(idBtnChr)) != string::npos){
                string str_i_pos = Constant::Trim(nameVal.at(0).substr(pos + strlen(idBtnChr)));
                int i = Constant::strToTipo<int>(str_i_pos);
                string val = Constant::Trim(nameVal.at(1));
                if (i < joy[joyPos0].num_buttons){
                    arrButtons[i] = Constant::strToTipo<int>(val);
                    buttonsCfg++;
                }
            } else if ((pos = nameVal.at(0).find(idHatChr)) != string::npos){
                string str_i_pos = Constant::Trim(nameVal.at(0).substr(pos + strlen(idHatChr)));
                vector<string> indices = Constant::splitChar(str_i_pos, '_');
                int ihat = Constant::strToTipo<int>(indices.at(0));
                int i = Constant::strToTipo<int>(indices.at(1));
                int j = Constant::strToTipo<int>(indices.at(2));

                string val = Constant::Trim(nameVal.at(1));
                if (i < 2 && j < 2){
                    hats[ihat].arrHats[i][j] = Constant::strToTipo<int>(val);
                }
            } else if ((pos = nameVal.at(0).find(numBtnChr)) != string::npos){
                string val = Constant::Trim(nameVal.at(1));
                buttonsSaved = Constant::strToTipo<int>(val);
            } else if ((pos = nameVal.at(0).find(idAnalogChr)) != string::npos){
                string val = Constant::Trim(nameVal.at(1));
                analogmode = Constant::strToTipo<int>(val);
            } 
        }
        file.close();
        return buttonsSaved == joy[joyPos0].num_buttons;
    }
    return false;
}        

/**
 * 
 */
bool Joystick::isJoyReleased(){
    bool ret = joyrelease;
    //Now we reset the values
    //if (joypress) {
    //    joypress = 0;
    //}
    if (joyrelease) {
        longJoyPause = 0;
        joyrelease = 0;
    }
    return ret;
}

/**
 * 
 */
bool Joystick::isJoyPressed(){
    if ((Constant::getTicks() - last > MAXJOYPAUSE || last > MAXJOYPAUSE) && longJoyPause < MAXJOYPAUSE && joypress){
        bool first = longJoyPause == 0;
        longJoyPause += maxFrameTime;
        return first ? joypress == 1 : 0;
    } else if (Constant::getTicks() - last > 35 && joypress){
        last = Constant::getTicks();
        return joypress == 1;
    } else 
        return 0;
}

/**
 * 
 */
int Joystick::getButtonStat(int btn){
    return jbuttonsStat[btn];
}

/**
 * 
 */
int Joystick::readJoystick(){
    /* we HAVE to do this to read the joystick */
    if (poll_joystick() != 0)
        return 1;

    int axisPos = -1;
    //Reading direction joystick inputs
    int nStick = 0;
    memset(jbuttonsStat, 0, sizeof(jbuttonsStat));
    while (nStick < joy[joyPos0].num_sticks){
        if (analogmode){
            if ((axisPos = getAxisPos(nStick, 0)) != -1 && hats[nStick].arrHats[0][axisPos] != J_NONE) {
                jbuttonsStat[hats[nStick].arrHats[0][axisPos]] = 1;
            } 
            if ((axisPos = getAxisPos(nStick, 1)) != -1 && hats[nStick].arrHats[1][axisPos] != J_NONE) {
                jbuttonsStat[hats[nStick].arrHats[1][axisPos]] = 1;
            } 
        } else {
            jbuttonsStat[hats[nStick].arrHats[0][0]] = joy[joyPos0].stick[nStick].axis[0].d1; //up
            jbuttonsStat[hats[nStick].arrHats[0][1]] = joy[joyPos0].stick[nStick].axis[0].d2; //down  
            jbuttonsStat[hats[nStick].arrHats[1][0]] = joy[joyPos0].stick[nStick].axis[1].d1; //left
            jbuttonsStat[hats[nStick].arrHats[1][1]] = joy[joyPos0].stick[nStick].axis[1].d2; //right
        }
        nStick++;
    }

    //Reading buttons joystick inputs
    for (int c=0; c < joy[joyPos0].num_buttons; c++) {
        if (arrButtons[c] < J_NONE){
            jbuttonsStat[arrButtons[c]] = joy[joyPos0].button[c].b;
        }
    }

    //Saving the states to detect key press
    for (unsigned int c = 0; c < J_NONE; c++){
        if (jbuttonsStat[c] && !jbuttonsPrevStat[c]){
            //cout << "joy pressed" << endl;
            joypress = 1;
            joyrelease = 0;
        } else if (!jbuttonsStat[c] && jbuttonsPrevStat[c]){
            //cout << "joy released" << endl;
            joypress = 0;
            joyrelease = 1;
        } 
    }
    std::copy(std::begin(jbuttonsStat), std::end(jbuttonsStat), std::begin(jbuttonsPrevStat));
    return 0;
}

/**
 * 
 */
int Joystick::getAxisPos(int nStick, int nAxis){
    
    int i = joy[joyPos0].stick[nStick].axis[nAxis].pos;
    //cout << "stick: " << nStick << " has " << joy[joyPos0].stick[nStick].num_axis << " axis" 
    //     << ((joy[joyPos0].stick[nStick].flags & JOYFLAG_UNSIGNED) ? " and it's unsigned" : "")
    //     << ((joy[joyPos0].stick[nStick].flags & JOYFLAG_SIGNED) ? " and it's signed" : "") <<  endl;
        if (i > 0) 
            return 1;
        else if (i < 0) 
            return 0;
        else 
            return -1;
}


/**
 * 
 */
int Joystick::shouldExit(){
    if (keypressed() && readkey() && key[KEY_BACKSPACE]){
        return 1;
    }
    return 0;
}

int Joystick::shouldGetNext(){
    if (key[KEY_DEL]){
        return 1;
    }
    return 0;
}

/**
 * 
 */
int Joystick::configureButtons(){
    unsigned int btnConfig = 0;
    int axisPos = -1;
    cout << "joy 0 with " << joy[joyPos0].num_buttons << " buttons" << endl;
    ALFONT_FONT *fontsmall = Fonts::getFont(Fonts::FONTSMALL);
    
    while (joy[joyPos0].flags & JOYFLAG_CALIBRATE) {
        //const char *msg = calibrate_joystick_name(0);
        calibrate_joystick_name(0);
        /**In my experience, is not needed to ask for the user to press buttons in Linux and DOS
         * so, we only call to the calibrate_joystick function.
         */
        //clear_bitmap(screen);
        //textout_centre_ex(screen, font, ("Joy 0 with " + std::to_string(joy[joyPos0].num_buttons) + " buttons").c_str(), 
        //    SCREEN_W/2, SCREEN_H / 2 - (font->height + 5), palette_color[255], 0);
        //textout_centre_ex(screen, font, msg, SCREEN_W/2, SCREEN_H / 2 , palette_color[255], 0);
        //textout_centre_ex(screen, font, "and press a key.", SCREEN_W/2, SCREEN_H / 2 + font->height + 5, palette_color[255], 0);
        //if ((readkey()&0xFF) == 27)
        //    return 0;
        if (calibrate_joystick(0) != 0) {
            set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
            allegro_message("Error calibrating joystick!\n");
            return 1;
        }
    }
    clear_keybuf();

    while (btnConfig < J_NONE) {
        int cfgBtn = btnConfig % J_NONE;
        if (shouldExit()) return 0;
        
        if (shouldGetNext()){
            cfgBtn++;
            continue;
        }

        clear_bitmap(screen);
        
        string msg = "Press joystick button " + string(jbuttonsTxt[cfgBtn]);
        Constant::drawTextCentre(screen, fontsmall, joystick_driver->name, SCREEN_W/2, SCREEN_H / 2 - (fontsmall->face_h + 5)*2, palette_color[255], 0);
        Constant::drawTextCentre(screen, fontsmall, msg.c_str(), SCREEN_W/2, SCREEN_H / 2, palette_color[255], 0);
        Constant::drawTextCentre(screen, fontsmall, "(Press \"BACKSPACE\" to exit", SCREEN_W/2, SCREEN_H / 2 + (fontsmall->face_h + 5) * 2, palette_color[255], 0);
        Constant::drawTextCentre(screen, fontsmall, "or press \"DEL\" to avoid button)", SCREEN_W/2, SCREEN_H / 2 + (fontsmall->face_h + 5) * 3, palette_color[255], 0);

        while (!joypress){
            if (shouldExit()) return 0;
            
            if (shouldGetNext()){
                cfgBtn++;
                break;
            }

            poll_joystick();
            joyrelease = 1;

            for (int c=0; c<joy[joyPos0].num_buttons && !joypress; c++) {
                if (joy[joyPos0].button[c].b){
                    //textprintf_centre_ex(bmp, font, 160, c*10, palette_color[15], 0, "%s pressed", joy[joyPos0].button[c].name);
                    cout << "setting: " << c << " for " << cfgBtn << "->" << jbuttonsTxt[cfgBtn] << endl;
                    joypress = 1;
                    joyrelease = 0;
                    arrButtons[c] = cfgBtn;
                }
            }
            
            int nStick = 0;
            while (nStick < joy[joyPos0].num_sticks){
                //if (analogmode && !joypress){
                if (!joypress){
                    if ((axisPos = getAxisPos(nStick, 0)) != -1) {
                        joypress = 1;
                        joyrelease = 0;
                        hats[nStick].arrHats[0][axisPos] = cfgBtn;
                        //cout << "Axis 0: " << joy[joyPos0].stick[nStick].axis[0].pos << " Axis 1: " << joy[joyPos0].stick[nStick].axis[1].pos << " -> " << "Left" << endl;
                        if (analogmode == -1) analogmode = 1;
                        hats[nStick].pressed = true;
                    }
                    if ((axisPos = getAxisPos(nStick, 1)) != -1) {
                        joypress = 1;
                        joyrelease = 0;
                        hats[nStick].arrHats[1][axisPos] = cfgBtn;
                        //cout << "Axis 0: " << joy[joyPos0].stick[nStick].axis[0].pos << " Axis 1: " << joy[joyPos0].stick[nStick].axis[1].pos << " -> " << "Up" << endl;
                        if (analogmode == -1) analogmode = 1;
                        hats[nStick].pressed = true;
                    }
                //} else if (!analogmode && !joypress){
                } if (!joypress){
                    if (joy[joyPos0].stick[nStick].axis[0].d1) {
                        joypress = 1;
                        joyrelease = 0;
                        //cout << "Left" << endl;
                        hats[nStick].arrHats[0][0] = cfgBtn;
                        if (analogmode == -1) analogmode = 0;
                        hats[nStick].pressed = true;
                    }
                    if (joy[joyPos0].stick[nStick].axis[0].d2) {
                        joypress = 1;
                        joyrelease = 0;
                        //cout << "Right" << endl;
                        hats[nStick].arrHats[0][1] = cfgBtn;
                        if (analogmode == -1) analogmode = 0;
                        hats[nStick].pressed = true;
                    }
                    if (joy[joyPos0].stick[nStick].axis[1].d1) {
                        joypress = 1;
                        joyrelease = 0;
                        //cout << "Up" << endl;
                        hats[nStick].arrHats[1][0] = cfgBtn;
                        if (analogmode == -1) analogmode = 0;
                        hats[nStick].pressed = true;
                    }
                    if (joy[joyPos0].stick[nStick].axis[1].d2) {
                        joypress = 1;
                        joyrelease = 0;
                        //cout << "Down" << endl;
                        hats[nStick].arrHats[1][1] = cfgBtn;
                        if (analogmode == -1) analogmode = 0;
                        hats[nStick].pressed = true;
                    }
                }
                nStick++;
            }
            rest(5);
        }

        //Waiting for joybutton release
        while (!joyrelease){
            poll_joystick();
            joyrelease = 1;
            for (int c=0; c<joy[joyPos0].num_buttons; c++) {
                if (joy[joyPos0].button[c].b){
                    joyrelease = 0;
                }
            }

            int nStick = 0;
            while (nStick < joy[joyPos0].num_sticks){
                if (hats[nStick].pressed){
                    if (getAxisPos(nStick, 0) != -1 
                        || getAxisPos(nStick, 1) != -1
                        || joy[joyPos0].stick[nStick].axis[0].d1
                        || joy[joyPos0].stick[nStick].axis[0].d2
                        || joy[joyPos0].stick[nStick].axis[1].d1
                        || joy[joyPos0].stick[nStick].axis[1].d2){
                        joyrelease = 0; 
                    } else {
                        hats[nStick].pressed = false;
                    }
                }
                nStick++;
            }
            if (shouldExit()) return 0;
            
            if (shouldGetNext()){
                cfgBtn++;
                break;
            }
        }
        //if ((readkey()&0xFF) == 27) //ESC
        //    break;
        btnConfig++;
        joypress = 0;
    }
    return 1;
}