#pragma once 

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <limits.h>

#include <allegro.h>
#include <almp3.h>

#include "utils/const/Constant.h"

using namespace std;

class Mp3File{
  public:
    Mp3File(){
        this->f = NULL;
        this->s = NULL;
    }

    ~Mp3File(){
        close();
    }

    void close() {
        if (this->f != NULL && this->s != NULL) {
            pack_fclose(this->f);
            almp3_destroy_mp3stream(this->s);
            this->f = NULL;
            this->s = NULL;
        }
    }

    PACKFILE *f;
    ALMP3_MP3STREAM *s;
    bool loop = false;
    bool autopoll = false;
    string filepath;
    uint64_t filesize;
};

#define BUFSZ   (1<<16) /* (65536) size of audiostream buffer */
#define DATASZ  (1<<15) /* (32768) amount of data to read from disk each time */

typedef enum{ TRACKSTOP, TRACKPLAY, TRACKPAUSE, TRACKEND} enumMusicStatus;

class Music{
    public:
        Music(){
            init();
        }
        
        ~Music(){
            closeAll();
        }

        static void auto_poll_mp3_file(void *p_mp3);
        void init();
        void closeAll();
        int loadBG(CfgLoader &cfgLoader);
        int playBG(unsigned int ntrack);
        void pauseBG(unsigned int ntrack);
        void rewindBG(unsigned int ntrack);
        void pauseBG();
        int playBG();

        static int getStatus(){
            return status;
        }

        unsigned int getNextSongIndex();
        unsigned int getSongIndex();

    private:
        const int volume = 64;
        const int pan = 128;
        
        static int status;
        int songIndex;

        vector<unique_ptr<Mp3File>> musicFiles;
        Mp3File *open_mp3_file(const char *filename);
        int play_mp3_file(Mp3File *mp3file, int buflen, int vol, int pan);
};


/**
 * 
 */
void Music::auto_poll_mp3_file(void *p_mp3) {   
    Mp3File *mp3file = (Mp3File *)p_mp3;
    char *data;
    long len;

    data = (char *)almp3_get_mp3stream_buffer(mp3file->s);
    if (data) {       
        len = pack_fread(data, DATASZ, mp3file->f);
        if (len < DATASZ){
            almp3_free_mp3stream_buffer(mp3file->s, len);
            status = TRACKEND;
        } else {
            almp3_free_mp3stream_buffer(mp3file->s, -1);
            status = TRACKPLAY;
        }
    }
    almp3_poll_mp3stream(mp3file->s);
}
END_OF_STATIC_FUNCTION(auto_poll_mp3_file)


/**
 * 
 */
void Music::closeAll(){
    Traza::print(Traza::T_DEBUG, "Deleting Music...");
    if (musicFiles.empty())
        return;
    musicFiles.clear();
}

/**
 * 
 */
void Music::init(){
    status = TRACKSTOP;
    songIndex = 0;
    LOCK_FUNCTION(auto_poll_mp3_file);
}

void Music::pauseBG(){
    pauseBG(this->songIndex);
}

int Music::playBG(){
    return playBG(this->songIndex);
}

/**
 * 
 */
void Music::pauseBG(unsigned int ntrack){
    if (musicFiles.size() <= ntrack)
        return;

    Mp3File *mp3file = musicFiles.at(ntrack).get();

    if (almp3_is_playing_mp3stream(mp3file->s)){
        almp3_stop_mp3stream(mp3file->s);
        cout << "stopping" << endl;
        if (mp3file->autopoll){
            cout << "remove param" << endl;
            remove_param_int(&auto_poll_mp3_file, (void *)mp3file);
        }
    } else{
        cout << "It's already stopped" << endl;
    }
    status = TRACKPAUSE;
}

/**
 * 
 */
void Music::rewindBG(unsigned int ntrack){
    if (musicFiles.size() <= ntrack)
        return;

    string filepathRewind = musicFiles.at(ntrack).get()->filepath;
    if (filepathRewind.empty()){
        Traza::print(Traza::T_ERROR, "No filename to rewind");   
        return;
    }

    snprintf(Traza::log_message, sizeof(Traza::log_message), "Rewinding file %s", filepathRewind.c_str());
    Traza::print(Traza::T_DEBUG);

    pauseBG(ntrack);

    Mp3File *newMp3file = open_mp3_file(filepathRewind.c_str());
    if (newMp3file){
        musicFiles[ntrack] = make_unique<Mp3File>(*newMp3file);
        playBG(ntrack);
    } else {
        snprintf(Traza::log_message, sizeof(Traza::log_message), "Error inserting mp3 file %s", filepathRewind.c_str());
        Traza::print(Traza::T_ERROR);
    }
}

/**
 * 
 */
int Music::playBG(unsigned int ntrack){
    if (musicFiles.size() <= ntrack)
        return 0;

    if (!almp3_is_playing_mp3stream(musicFiles.at(ntrack).get()->s) && play_mp3_file(musicFiles.at(ntrack).get(), BUFSZ, volume, pan) == ALMP3_OK){
        //Code to play in a thread without blocking the interface
        musicFiles.at(ntrack).get()->autopoll = true;
        install_param_int(&auto_poll_mp3_file, (void *)musicFiles.at(ntrack).get(), 200);
        status = TRACKPLAY;
        return 1;
    } 
    return 0;
}

unsigned int Music::getSongIndex(){
    return songIndex;
}

unsigned int Music::getNextSongIndex(){
    songIndex = (songIndex + 1) % musicFiles.size();
    return songIndex;
}

/**
 * 
 */
Mp3File *Music::open_mp3_file(const char *filename) {
    Mp3File *p = NULL;
    PACKFILE *f = NULL;
    ALMP3_MP3STREAM *s = NULL;
    char data[DATASZ];
    int len;

    try {
        p = new Mp3File();
    } catch (const std::bad_alloc& ba) {
        goto error;
    }

    if (!(f = pack_fopen(filename, F_READ)))
        goto error;
    if ((len = pack_fread(data, DATASZ, f)) <= 0)
        goto error;
    if (len < DATASZ) {
        if (!(s = almp3_create_mp3stream(data, len, TRUE)))
        goto error;
    }
    else {
        if (!(s = almp3_create_mp3stream(data, DATASZ, FALSE)))
        goto error;
    }

    p->f = f;
    p->s = s;
    p->loop = true;
    p->filepath = filename;
    p->filesize = file_size_ex(filename);
    return p;

    error:
        pack_fclose(f);
        delete p;
        return NULL;
}

/**
 * 
 */
int Music::play_mp3_file(Mp3File *mp3file, int buflen, int vol, int pan) {
    return almp3_play_mp3stream(mp3file->s, buflen, vol, pan);
}


/**
 * 
 */
int Music::loadBG(CfgLoader &cfgLoader){
    string pathtomusic = Constant::getAppDir() + "/assets/music";
    dirutil dir;

    vector<unique_ptr<FileProps>> files;
    const string extFilter = ".mp3";
    dir.listarFilesSuperFast(pathtomusic.c_str(), files, extFilter, true, false);

    for (size_t i=0; i < files.size(); i++){
        auto file = files.at(i).get();
        if (!cfgLoader.configMain.mp3_file.empty() && file->filename.compare(cfgLoader.configMain.mp3_file) != 0)
            continue;

        Mp3File *mp3file = open_mp3_file((file->dir + Constant::getFileSep() + file->filename).c_str());
        if (mp3file){
            musicFiles.emplace_back(make_unique<Mp3File>(*mp3file));
        }
    }
    return musicFiles.size() > 0;
}