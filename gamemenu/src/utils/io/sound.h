#pragma once 

#include <map>
#include <allegro.h>
#include <string>
#include <iostream>

#include "utils/const/Constant.h"

using namespace std;

class Sound{
    public:
        Sound(){
            SAMPLE *sample = load_wav((Constant::getAppDir() + "/assets/sound/bclick2.wav").c_str());
            if (sample){
                samples.insert(make_pair(SBTNCLICK, std::move(sample)));
            } 

            sample = load_wav((Constant::getAppDir() + "/assets/sound/load.wav").c_str());
            if (sample){
                samples.insert(make_pair(SBTNLOAD, std::move(sample)));
            } 
        }
        ~Sound(){
            cout << "deleting Sound" << endl;
        }

        bool play(int soundid, bool wait){
            auto it = samples.find(soundid);
            if (it != samples.end()) {
                std::unique_ptr<SAMPLE>& ptr = it->second;
                play_sample(ptr.get(), volume, pan, freq, 0);
                if (wait && ptr.get()->freq > 0 && ptr.get()->len / (float)ptr.get()->freq * 1000.0 < maxTimeWait){
                    rest(ptr.get()->len / (float)ptr.get()->freq * 1000.0);  
                }
                return true;
            }
            return false;
        }

        bool play(int soundid){
            return play(soundid, false);
        }
    
    private:
        map<int, unique_ptr<SAMPLE>> samples;
        const float maxTimeWait = 2000.0;
        const int volume = 255;
        const int pan = 0;
        //1000 represents the frequency that the sample was recorded at, 2000 is twice this, etc
        const int freq = 1000;
};