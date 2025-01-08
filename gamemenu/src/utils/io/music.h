#pragma once 

#include <map>
#include <allegro.h>
#include <almp3.h>
#include <string>
#include <iostream>
#include <limits.h>

#include "utils/const/Constant.h"

using namespace std;

typedef struct {
  PACKFILE *f;
  ALMP3_MP3STREAM *s;
  bool loop = false;
  bool autopoll = false;
  char filepath[PATH_MAX];
  uint64_t filesize;
} MP3FILE;

#define BUFSZ   (1<<16) /* (65536) size of audiostream buffer */
#define DATASZ  (1<<15) /* (32768) amount of data to read from disk each time */

class Music{

    public:
        static void auto_poll_mp3_file(void *p_mp3);
        void init();
        Music(){
            //musicLists[0] = Mp3Music{ NULL, 0 };
            init();
        }
        
        ~Music(){
            closeAll();
        }

        void closeAll();
        int loadBG();
        int playBG(int ntrack);
        void pauseBG(int ntrack);
        void rewindBG(int ntrack);

    private:
        const int volume = 64;
        const int pan = 128;
        MP3FILE **mp3;

        int poll_mp3_file(MP3FILE *mp3);
        MP3FILE *open_mp3_file(const char *filename);
        int play_mp3_file(MP3FILE *mp3, int buflen, int vol, int pan);
        void close_mp3_file(MP3FILE *mp3);
};

void Music::closeAll(){
    cout << "deleting Music" << endl;
    //almp3_destroy_mp3(musicLists[0].mp3); 
    //free(data);

    if (!mp3)
        return;

    int n = 1;
    for (int i = 0; i < n; i++) {
        if ((mp3[i]) && (poll_mp3_file(mp3[i]) != ALMP3_OK)) {
            close_mp3_file(mp3[i]);
            mp3[i] = NULL;
        }
    }
    
    free(mp3);
    mp3 = NULL;
}

/**
 * 
 */
void Music::auto_poll_mp3_file(void *p_mp3) {   
    MP3FILE *mp3file = (MP3FILE *)p_mp3;
    char *data;
    long len;

    data = (char *)almp3_get_mp3stream_buffer(mp3file->s);
    if (data) {       
        //pack_fseek(mp3file->f, mp3file->filesize/2);
        len = pack_fread(data, DATASZ, mp3file->f);
        //cout << "Reading stream from mp3 file" << endl;
        if (len < DATASZ){
            almp3_free_mp3stream_buffer(mp3file->s, len);
        } else
            almp3_free_mp3stream_buffer(mp3file->s, -1);
    }

    almp3_poll_mp3stream(mp3file->s);
}
END_OF_STATIC_FUNCTION(auto_poll_mp3_file)

void Music::init(){
    mp3 = NULL;
    LOCK_FUNCTION(auto_poll_mp3_file);
}

int Music::poll_mp3_file(MP3FILE *mp3) {
    char *data;
    long len;

    data = (char *)almp3_get_mp3stream_buffer(mp3->s);
    if (data) {
        len = pack_fread(data, DATASZ, mp3->f);
        if (len < DATASZ)
            almp3_free_mp3stream_buffer(mp3->s, len);
        else
            almp3_free_mp3stream_buffer(mp3->s, -1);
    }

    return almp3_poll_mp3stream(mp3->s);
}

/**
 * 
 */
void Music::pauseBG(int ntrack){
    MP3FILE *mp3file = mp3[ntrack];
    if (almp3_is_playing_mp3stream(mp3file->s)){
        almp3_stop_mp3stream(mp3file->s);
        cout << "stoping" << endl;
        if (mp3file->autopoll){
            remove_param_int(&auto_poll_mp3_file, (void *)mp3file);
        }
    } else{
        cout << "It's already stopped" << endl;
    }
}

void Music::rewindBG(int ntrack){
    MP3FILE *mp3file = mp3[ntrack];
    string filepathRewind = mp3file->filepath;
    if (filepathRewind.empty()){
        cout << "No filename to rewind: " << filepathRewind << endl;    
        return;
    }
    cout << "Rewinding file: " << filepathRewind << endl;
    pauseBG(ntrack);
    close_mp3_file(mp3file);
    mp3file = open_mp3_file(filepathRewind.c_str());
    playBG(0);
}

/**
 * 
 */
int Music::playBG(int ntrack){

    /* get the number of total frames */
    int total_frames = almp3_get_length_frames_mp3stream(mp3[ntrack]->s, 4);
    int posBytes = almp3_seek_abs_frames_mp3stream(mp3[ntrack]->s, total_frames / 2, 0);
    pack_fseek(mp3[ntrack]->f, posBytes);


    if (!almp3_is_playing_mp3stream(mp3[ntrack]->s) && play_mp3_file(mp3[ntrack], BUFSZ, volume, pan) == ALMP3_OK){
        //Code to play in a thread without blocking the interface
        mp3[ntrack]->autopoll = true;
        install_param_int(&auto_poll_mp3_file, (void *)mp3[0], 200);
        return 1;
    } 
    return 0;
}

MP3FILE *Music::open_mp3_file(const char *filename) {
    MP3FILE *p = NULL;
    PACKFILE *f = NULL;
    ALMP3_MP3STREAM *s = NULL;
    char data[DATASZ];
    int len;

    if (!(p = (MP3FILE *)malloc(sizeof(MP3FILE))))
        goto error;
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
    strncpy(p->filepath, filename, PATH_MAX-1);
    p->filesize = file_size_ex(filename);
    return p;

    error:
    pack_fclose(f);
    free(p);
    return NULL;
}


int Music::play_mp3_file(MP3FILE *mp3, int buflen, int vol, int pan) {
    return almp3_play_mp3stream(mp3->s, buflen, vol, pan);
}


void Music::close_mp3_file(MP3FILE *mp3) {
    if (mp3) {
        pack_fclose(mp3->f);
        almp3_destroy_mp3stream(mp3->s);
        free(mp3);
    }
}

int Music::loadBG(){
    /*TODO: scan a directory of n files in it*/
    int n = 1;
    if (mp3 == NULL){
        mp3 = (MP3FILE **)malloc(sizeof(MP3FILE *) * n);
        mp3[0] = NULL;
    }

    string pathtomusic = Constant::getAppDir() + "/assets/music/strange.mp3";
    if (mp3[0] != NULL){
        close_mp3_file(mp3[0]);
    }

    if ((mp3[0] = open_mp3_file(pathtomusic.c_str()))){
        return 1;
    } else {
        mp3[0] = NULL;
        return 0;
    }
    return 1;
}