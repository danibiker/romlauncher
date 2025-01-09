#ifndef FONTS
#define FONTS

#include <vector>
#include <string>

#include "utils/const/Constant.h"
#include "utils/font/alfont.h"

class Fonts{
    public:
        typedef enum{ FONTBIG = 0, FONTSMALL } enumFonts;

        Fonts(){}
        ~Fonts(){
            Traza::print(Traza::T_DEBUG, "Deleting Fonts...");
            exit();
        }

        /**
         * 
         */
        static void init(){
            alfont_init();
        }

        /**
         * 
         */
        static void exit(){
            for (unsigned int i=0; i < 2; i++){
                if (vFonts[i] != NULL){
                    alfont_destroy_font(vFonts[i]);
                    vFonts[i] = NULL;
                }
            }
            alfont_exit();
        }
        
        /**
         * 
         */
        static void initFonts(int fontSize){
            const string fontRoute = Constant::getAppDir() + Constant::getFileSep() + "assets" 
                + Constant::getFileSep() + "art" + Constant::getFileSep();
                
            string fontRouteBig = fontRoute + (fontSize > 16 ? "cour.ttf" : "Unibody.ttf");
            ALFONT_FONT * fontloaded = alfont_load_font(fontRouteBig.c_str());

            if (fontloaded != NULL){
                alfont_set_font_size(fontloaded, fontSize <= 16 ? 12 : fontSize);
                if (vFonts[FONTBIG] != NULL){
                    alfont_destroy_font(vFonts[FONTBIG]);
                }
                vFonts[FONTBIG] = fontloaded;
            }

            string fontRouteSmall = fontRoute + ((fontSize / 2) > 12 ? "cour.ttf" : "Unibody.ttf");
            ALFONT_FONT * fontloaded2 = alfont_load_font(fontRouteSmall.c_str());
            if (fontloaded2 != NULL){
                alfont_set_font_size(fontloaded2, fontSize / 2 < 12 ? 12 : fontSize / 2);
                if (vFonts[FONTSMALL] != NULL){
                    alfont_destroy_font(vFonts[FONTSMALL]);
                }
                vFonts[FONTSMALL] = fontloaded2;
            }
        }

        static ALFONT_FONT *getFont(int fontId){
            if (fontId <= FONTSMALL && fontId >= 0)
                return vFonts[fontId];
            else 
                return NULL;
        }

        static size_t idxToCutTTF(string text, int maxW, int fontId){
            if (text.empty())
                return 0;

            if (alfont_text_length(vFonts[fontId], text.c_str()) < maxW){
                return text.length();
            }

            size_t i = 1;
            while(i < text.length()){
                if (alfont_text_length(vFonts[fontId], text.substr(0, i).c_str()) >= maxW){
                    i--;
                    break;
                }
                i++;
            }
            return i;
        }

    private:
        static ALFONT_FONT * vFonts[2];
        static const int fsbig = 20;
        static const int fsmall = 10;
};

#endif

