/* AllegroFont - a wrapper for FreeType 2 */
/* to render TTF and other font formats with Allegro */


/* FreeType 2 is copyright (c) 1996-2000 */
/* David Turner, Robert Wilhelm, and Werner Lemberg */
/* AllegroFont is copyright (c) 2001, 2002 Javier Gonz�lez */
/* Enhanced by Chernsha since 2004 year */

/* See FTL.txt (FreeType Project License) for license */


//#define ALFONT_WINDOWS   //When compiling in WINDOWS,please uncomment this line.
//#define ALFONT_ICONV 	 //When compiling in DOS,please uncomment this line.
//#define ALFONT_LINUX 	 //When compiling in LINUX,please uncomment this line.
#if defined(_MSC_VER) || defined(__MINGW32__)
#	define ALFONT_WINDOWS
#elif UNIX
#	define ALFONT_LINUX
#else
#	define ALFONT_ICONV
#endif


#ifndef ALFONT_H
#define ALFONT_H


#ifdef ALFONT_WINDOWS	//run in WINDOWS
#define Encode_Code "chinese-traditional"
#endif
#ifdef ALFONT_ICONV	    //run in DOS
#define Encode_Code "BIG5"
#endif
#ifdef ALFONT_LINUX		//run in LINUX
#define Encode_Code "zh_TW.Big5"
#endif

#include <allegro.h>
#include <ft2build.h>

#ifdef ALLEGRO5
	//undefine the ambiguous allegro5 defininations
	#undef bool
	#undef max
	#undef min
#endif

//#include "alfontdll.h"

#include FT_FREETYPE_H
#include FT_GLYPH_H


#ifdef __cplusplus
extern "C" {
#endif


/* common define */

#define ALFONT_MAJOR_VERSION        2
#define ALFONT_MINOR_VERSION        0
#define ALFONT_SUB_VERSION          6
#define ALFONT_VERSION_STR          "2.0.6"
#define ALFONT_DATE_STR             "28/03/2006"
#define ALFONT_DATE                 20060328    /* yyyymmdd */

/* error codes */

#define ALFONT_OK                   0
#define ALFONT_ERROR                -1



/* structs */
//typedef struct ALFONT_FONT ALFONT_FONT;
/* structs */

struct _ALFONT_CACHED_GLYPH {
    char is_cached;
    int width, height, aawidth, aaheight;
    int left, top, aaleft, aatop;
    int advancex, advancey;
    int mono_available, aa_available;
    unsigned char *bmp;
    unsigned char *aabmp;
};

struct ALFONT_FONT {
  FT_Face face;           /* face */
  int face_h;             /* face height */
  int real_face_h;        /* real face height */
  int face_ascender;      /* face ascender */
  char *data;             /* if loaded from memory, the data chunk */
  int data_size;          /* and its size */
  int ch_spacing;         /* extra spacing */
  int num_fixed_sizes;    /* -1 if scalable, >=0 if fixed */
  _ALFONT_CACHED_GLYPH *cached_glyphs;       /* array to know which glyphs have been cached */
  int *fixed_sizes;       /* array with the fixed sizes */
  char *language;		  /* language */
  int type;				  /* Code Convert(Please Use TYPE_WIDECHAR for ASCII to UNICODE) */
  int outline_top;		  /* Font top outline width */
  int outline_bottom;	  /* Font bottom outline width */
  int outline_right;	  /* Font right outline width */
  int outline_left;		  /* Font left outline width */
  int outline_color;	  /* Font outline color */
  int outline_hollow;	  /* Font hollow(TRUE/FALSE) */
  int style;			  /* Font Style(STYLE_STANDARD/STYLE_ITALIC/STYLE_BOLD/STYLE_BOLDITALIC) */
  int underline;		  /* Font underline(TRUE/FALSE) */
  int underline_right;	  /* Extend right underline(TRUE/FALSE) */
  int underline_left;	  /* Extend left underline(TRUE/FALSE) */
  int background;		  /* Font Background Color(TRUE/FALSE) */
  int transparency;		  /* Font transparency(0-255) */
  int autofix;			  /* Font autofix(TRUE/FALSE) */
  int precedingchar;      /* preceding character for autofix*/
};



/* API */

int alfont_init(void);
void alfont_exit(void);

ALFONT_FONT *alfont_load_font(const char *filepathname);
ALFONT_FONT *alfont_load_font_from_mem(const char *data, int data_len);
void alfont_destroy_font(ALFONT_FONT *f);

int alfont_set_font_size(ALFONT_FONT *f, int h);
int alfont_get_font_height(ALFONT_FONT *f);

int alfont_text_mode(int mode);

void alfont_set_language(ALFONT_FONT *f, const char *language);				//Sets Language
char *alfont_get_language(ALFONT_FONT *f);									//Returns Language
void alfont_set_convert(ALFONT_FONT *f, int type);							//Sets Code Convert(Please Use TYPE_WIDECHAR for ASCII to UNICODE,TYPE_MULTIBYTE for UNICODE to ASCII)
int alfont_get_convert(ALFONT_FONT *f);										//Returns Code Convert(Return TYPE_WIDECHAR for ASCII to UNICODE,TYPE_MULTIBYTE for UNICODE to ASCII)
void alfont_set_font_style(ALFONT_FONT *f, int style);						//Sets Font Style(Please Use STYLE_STANDARD,STYLE_ITALIC,STYLE_BOLD or STYLE_BOLDITALIC)
int alfont_get_font_style(ALFONT_FONT *f);									//Returns Font Style(Please Use STYLE_STANDARD,STYLE_ITALIC,STYLE_BOLD or STYLE_BOLDITALIC)
void alfont_set_font_underline(ALFONT_FONT *f, int underline);				//Sets Font underline(TRUE/FALSE)
int alfont_get_font_underline(ALFONT_FONT *f);								//Returns Font underline(TRUE/FALSE)
void alfont_set_font_underline_right(ALFONT_FONT *f, int underline_right);	//Sets Extend right underline(TRUE/FALSE)
int alfont_get_font_underline_right(ALFONT_FONT *f);						//Returns Extend right underline(TRUE/FALSE)
void alfont_set_font_underline_left(ALFONT_FONT *f, int underline_left);	//Sets Extend left underline(TRUE/FALSE)
int alfont_get_font_underline_left(ALFONT_FONT *f);							//Returns Extend left underline(TRUE/FALSE)
void alfont_set_font_background(ALFONT_FONT *f, int background);			//Sets Font Background Color(TRUE/FALSE)
int alfont_get_font_background(ALFONT_FONT *f);								//Returns Font Background Color(TRUE/FALSE)
void alfont_set_font_transparency(ALFONT_FONT *f, int transparency);		//Sets Font transparency(0-255)
int alfont_get_font_transparency(ALFONT_FONT *f);							//Returns Font transparency(0-255)
void alfont_set_autofix(ALFONT_FONT *f, int autofix);						//Sets autofix(TRUE/FALSE)
int alfont_get_autofix(ALFONT_FONT *f);										//Returns autofix(TRUE/FALSE)
void alfont_set_precedingchar(ALFONT_FONT *f, int precedingchar);			//Sets precedingchar
int alfont_get_precedingchar(ALFONT_FONT *f);								//Returns precedingchar

int alfont_ugetc(ALFONT_FONT *f, const char *s);							//Returns the character pointered by `s' in the current encoding format

void alfont_set_font_outline_top(ALFONT_FONT *f, int w);					//Sets Font top outline width
int alfont_get_font_outline_top(ALFONT_FONT *f);							//Returns Font top outline width
void alfont_set_font_outline_bottom(ALFONT_FONT *f, int w);					//Sets Font bottom outline width
int alfont_get_font_outline_bottom(ALFONT_FONT *f);							//Returns Font bottom outline width
void alfont_set_font_outline_left(ALFONT_FONT *f, int w);					//Sets Font left outline width
int alfont_get_font_outline_left(ALFONT_FONT *f);							//Returns Font left outline width
void alfont_set_font_outline_right(ALFONT_FONT *f, int w);					//Sets Font right outline width
int alfont_get_font_outline_right(ALFONT_FONT *f);							//Returns Font right outline width
void alfont_set_font_outline_color(ALFONT_FONT *f, int c);					//Sets Font outline color
int alfont_get_font_outline_color(ALFONT_FONT *f);							//Returns Font outline color
void alfont_set_font_outline_hollow(ALFONT_FONT *f, int hollow);			//Sets Font hollow(TRUE/FALSE)
int alfont_get_font_outline_hollow(ALFONT_FONT *f);							//Returns Font hollow(TRUE/FALSE)

void alfont_textout_aa(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color);
void alfont_textout(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color);
void alfont_textout_aa_ex(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color, int bg);
void alfont_textout_ex(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color, int bg);

void alfont_textout_centre_aa(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color);
void alfont_textout_centre(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color);
void alfont_textout_centre_aa_ex(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color, int bg);
void alfont_textout_centre_ex(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color, int bg);

void alfont_textout_right_aa(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color);
void alfont_textout_right(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color);
void alfont_textout_right_aa_ex(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color, int bg);
void alfont_textout_right_ex(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color, int bg);

void alfont_textprintf(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, const char *format, ...);
void alfont_textprintf_aa(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, const char *format, ...);
void alfont_textprintf_ex(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, int bg, const char *format, ...);
void alfont_textprintf_aa_ex(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, int bg, const char *format, ...);

void alfont_textprintf_centre(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, const char *format, ...);
void alfont_textprintf_centre_aa(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, const char *format, ...);
void alfont_textprintf_centre_ex(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, int bg, const char *format, ...);
void alfont_textprintf_centre_aa_ex(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, int bg, const char *format, ...);

void alfont_textprintf_right(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, const char *format, ...);
void alfont_textprintf_right_aa(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, const char *format, ...);
void alfont_textprintf_right_ex(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, int bg, const char *format, ...);
void alfont_textprintf_right_aa_ex(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, int bg, const char *format, ...);

int alfont_text_height(ALFONT_FONT *f);
int alfont_text_length(ALFONT_FONT *f, const char *str);
int alfont_text_count(ALFONT_FONT *f, const char *str);		//Returns char Count(ASCII To Unicode)

int alfont_need_uconvert(ALFONT_FONT *f, const char *str);	//Returns TRUE if TYPE_WIDECHAR conversion is required or FALSE otherwise.

int alfont_is_fixed_font(ALFONT_FONT *f);
int alfont_is_scalable_font(ALFONT_FONT *f);

const int *alfont_get_available_fixed_sizes(ALFONT_FONT *f);
int alfont_get_nof_available_fixed_sizes(ALFONT_FONT *f);

int alfont_get_char_extra_spacing(ALFONT_FONT *f);
void alfont_set_char_extra_spacing(ALFONT_FONT *f, int spacing);

#ifdef __cplusplus
}
#endif

#endif
//
#define TYPE_NOSWITCH 0   //No Switch
#define TYPE_MULTIBYTE 1  //Local Code(convert Unicode Code To ASCII Code)
#define TYPE_WIDECHAR 2   //Unicode Code(convert ASCII Code To UNICODE Code)

//
#define STYLE_STANDARD 0     //Standard style font
#define STYLE_ITALIC 1	     //Italic style font
#define STYLE_BOLD 2         //Bold style font
#define STYLE_BOLDITALIC 3   //BoldItalic style font

/*Win32 Language Strings

Chinese "chinese"
Chinese(simplified) "chinese-simplified" or "chs"
Chinese(traditional) "chinese-traditional" or "cht"
Czech "csy" or "czech"
Danish "dan"or "danish"
Dutch(Belgian) "belgian", "dutch-belgian", or "nlb"
Dutch(default) "dutch" or "nld"
English(Australian) "australian", "ena", or "english-aus"
English(Canadian) "canadian", "enc", or "english-can"
English(default) "english"
English(New Zealand) "english-nz" or "enz"
English(UK) "eng", "english-uk", or "uk"
English(USA) "american", "american english", "american-english", "english-american", "english-us", "english-usa", "enu", "us", or "usa"
Finnish "fin" or "finnish"
French(Belgian) "frb" or "french-belgian"
French(Canadian) "frc" or "french-canadian"
French(default) "fra"or "french"
French(Swiss) "french-swiss" or "frs"
German(Austrian) "dea" or "german-austrian"
German(default) "deu" or "german"
German(Swiss) "des", "german-swiss", or "swiss"
Greek "ell" or "greek"
Hungarian "hun" or "hungarian"
Icelandic "icelandic" or "isl"
Italian(default) "ita" or "italian"
Italian(Swiss) "italian-swiss" or "its"
Japanese "japanese" or "jpn"
Korean "kor" or "korean"
Norwegian(Bokmal) "nor" or "norwegian-bokmal"
Norwegian(default) "norwegian"
Norwegian(Nynorsk) "non" or "norwegian-nynorsk"
Polish "plk" or "polish"
Portuguese(Brazilian) "portuguese-brazilian" or "ptb"
Portuguese(default) "portuguese" or "ptg"
Russian(default) "rus" or "russian"
Slovak "sky" or "slovak"
Spanish(default) "esp" or "spanish"
Spanish(Mexican) "esm" or "spanish-mexican"
Spanish(Modern) "esn" or "spanish-modern"
Swedish "sve" or "swedish"
Turkish "trk" or "turkish"

*/

/*Linux Language Strings
Afar                    aa
Abkhazian               ab
Afrikaans               af
Amharic                 am
Arabic                  ar
Assamese                as
Aymara                  ay
Azerbaijani             az

Bashkir                 ba
Byelorussian            be
Bulgarian               bg
Bihari                  bh
Bislama                 bi
Bengali; Bangla         bn
Tibetan                 bo
Breton                  br

                        C
Catalan                 ca
Corsican                co
Czech                   cs cs_CZ
Welsh                   cy

Danish                  da da_DK
German                  de de_AT de_DE
Bhutani                 dz

Greek                   el
English                 en en_AU en@boldquot en_CA en_GB en@IPA en@quot en_SE en_US
Esperanto               eo
Spanish                 es es_DO es_ES es_GT es_HN es_MX es_NI es_PA es_PE es_SV
Estonian                et et_EE
Basque                  eu
			eu_ES

Persian                 fa
Finnish                 fi fi_FI
Fiji                    fj
Faroese                 fo
French                  fr fr_FR
Frisian                 fy

Irish                   ga
Scots Gaelic            gd
Galician                gl
Guarani                 gn
Gujarati                gu

Hausa                   ha
Hebrew (formerly iw)    he
Hindi                   hi
Croatian                hr
Hungarian               hu
Armenian                hy

Interlingua             ia
Indonesian (formerly in)id
Interlingue             ie
Inupiak                 ik
Icelandic               is
Italian                 it
Inuktitut               iu

Japanese                ja ja_JP.eucJP ja_JP.SJIS
Javanese                jw

Georgian                ka
Kazakh                  kk
Greenlandic             kl
Cambodian               km
Kannada                 kn
Korean                  ko
Kashmiri                ks
Kurdish                 ku
Kirghiz                 ky

Latin                   la
Lingala                 ln
Laothian                lo
Lithuanian              lt
			lug
Latvian, Lettish        lv

Malagasy                mg
Maori                   mi
Macedonian              mk
Malayalam               ml
Mongolian               mn
Moldavian               mo
Marathi                 mr
Malay                   ms
Maltese                 mt
Burmese                 my

Nauru                   na
			nb_NO
Nepali                  ne
Dutch                   nl
			nn_NO
Norwegian               no no_NY

Occitan                 oc
(Afan) Oromo            om
Oriya                   or

Punjabi                 pa
Polish                  pl
Pashto, Pushto          ps
Portuguese              pt pt_BR pt_PT

Quechua                 qu

Rhaeto-Romance          rm
Kirundi                 rn
Romanian                ro
Russian                 ru ru_RU.cp1251 ru_RU.koi8r
Kinyarwanda             rw

Sanskrit                sa
Sindhi                  sd
Sangho                  sg
Serbo-Croatian          sh
Sinhalese               si
Slovak                  sk
Slovenian               sl
Samoan                  sm
Shona                   sn
Somali                  so
Albanian                sq
Serbian                 sr
Siswati                 ss
Sesotho                 st
Sundanese               su
Swedish                 sv
Swahili                 sw

Tamil                   ta
Telugu                  te
Tajik                   tg
Thai                    th
Tigrinya                ti
Turkmen                 tk
Tagalog                 tl
Setswana                tn
Tonga                   to
Turkish                 tr
Tsonga                  ts
Tatar                   tt
Twi                     tw

Uighur                  ug
Ukrainian               uk uk_UA.cp1251
Urdu                    ur
Uzbek                   uz

Vietnamese              vi
Volapuk                 vo

Wolof                   wo

Xhosa                   xh

Yiddish (formerly ji)   yi
Yoruba                  yo

Zhuang                  za
Chinese                 zh
Chinese(Hong kong)	zh_HK
Chinese(simple)		zh_CN zh_CN.GB2312
Chinese(traditional)	zh_TW zh_TW.Big5
Zulu                    zu
*/

/*DOS Language Strings
European languages
ASCII, ISO-8859-{1,2,3,4,5,7,9,10,13,14,15,16}, KOI8-R, KOI8-U, KOI8-RU, CP{1250,1251,1252,1253,1254,1257}, CP{850,866}, Mac{Roman,CentralEurope,Iceland,Croatian,Romania}, Mac{Cyrillic,Ukraine,Greek,Turkish}, Macintosh

Semitic languages
ISO-8859-{6,8}, CP{1255,1256}, CP862, Mac{Hebrew,Arabic}

Japanese
EUC-JP, SHIFT_JIS, CP932, ISO-2022-JP, ISO-2022-JP-2, ISO-2022-JP-1

Chinese
EUC-CN, HZ, GBK, GB18030, EUC-TW, BIG5, CP950, BIG5-HKSCS, ISO-2022-CN, ISO-2022-CN-EXT

ps. Chinese(traditional) : BIG5 , Chinese(simplified) : GBK

Korean
EUC-KR, CP949, ISO-2022-KR, JOHAB

Armenian
ARMSCII-8

Georgian
Georgian-Academy, Georgian-PS

Tajik
KOI8-T

Thai
TIS-620, CP874, MacThai

Laotian
MuleLao-1, CP1133

Vietnamese
VISCII, TCVN, CP1258

Platform specifics
HP-ROMAN8, NEXTSTEP

Full Unicode
UTF-8
UCS-2, UCS-2BE, UCS-2LE
UCS-4, UCS-4BE, UCS-4LE
UTF-16, UTF-16BE, UTF-16LE
UTF-32, UTF-32BE, UTF-32LE
UTF-7
C99, JAVA

Full Unicode, in terms of uint16_t or uint32_t (with machine dependent endianness and alignment)
UCS-2-INTERNAL, UCS-4-INTERNAL

European languages
CP{437,737,775,852,853,855,857,858,860,861,863,865,869,1125}

Semitic languages
CP864

Japanese
EUC-JISX0213, Shift_JISX0213, ISO-2022-JP-3

Turkmen
TDS565

Platform specifics
RISCOS-LATIN1
*/
