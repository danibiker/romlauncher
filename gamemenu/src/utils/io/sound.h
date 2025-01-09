#pragma once 

#include <map>
#include <allegro.h>
#include <string>
#include <iostream>

#include "utils/const/Constant.h"

#ifdef UNIX
#include <alsa/asoundlib.h>
#endif

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
            Traza::print(Traza::T_DEBUG, "Deleting Sound...");
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
        
        static int alsaInit(){
            #ifdef UNIX
                const char *pcm_name = "hw:1";  // Explicitly use hw:1 for card 1
                const char *ctl_name = "hw:1";  // Explicitly use hw:1 for control

                // Set up the PCM (audio device) and control to hw:1 (card 1)
                snd_pcm_t *pcm_handle;
                snd_ctl_t *ctl_handle;
                int err;

                // Open the PCM device for playback with the given settings
                err = snd_pcm_open(&pcm_handle, pcm_name, SND_PCM_STREAM_PLAYBACK, 0);
                if (err < 0) {
                    std::cerr << "Failed to open PCM device: " << snd_strerror(err) << std::endl;
                    return 1;
                }

                // Set the PCM device to use the hardware device 1
                snd_pcm_hw_params_t *params;
                snd_pcm_hw_params_alloca(&params);
                err = snd_pcm_hw_params_any(pcm_handle, params);
                if (err < 0) {
                    std::cerr << "Failed to initialize hardware parameters: " << snd_strerror(err) << std::endl;
                    return 1;
                }

                err = snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
                if (err < 0) {
                    std::cerr << "Failed to set access type: " << snd_strerror(err) << std::endl;
                    return 1;
                }

                // Configure the control device to hw:1 (card 1)
                err = snd_ctl_open(&ctl_handle, ctl_name, 0);
                if (err < 0) {
                    std::cerr << "Failed to open control device: " << snd_strerror(err) << std::endl;
                    return 1;
                }

                std::cout << "ALSA initialized successfully with hw:1" << std::endl;

                // Clean up and close handles
                snd_pcm_close(pcm_handle);
                snd_ctl_close(ctl_handle);
            #endif
            return 0;
        }
    
    private:
        map<int, unique_ptr<SAMPLE>> samples;
        const float maxTimeWait = 2000.0;
        const int volume = 255;
        const int pan = 128;
        //1000 represents the frequency that the sample was recorded at, 2000 is twice this, etc
        const int freq = 1000;
};