#Paths to the djgpp compiler
COMPILER_DIR=./dep/dosdev
DJBIN=$(COMPILER_DIR)/djgpp/bin
DJDOSBIN=$(COMPILER_DIR)/djgpp/i586-pc-msdosdjgpp/bin
#Compiler of c and c++
CC=$(DJBIN)/i586-pc-msdosdjgpp-gcc
CCPLUS=$(DJBIN)/i586-pc-msdosdjgpp-g++
UNIXCCPLUS=/usr/bin/g++
MINGWCC=/mingw64/bin/g++
#To generate a stubbed exe for DOS with PMODE
EXE2COFF=$(DJDOSBIN)/exe2coff
#To generate the main gamemenu program
INCLUDES_DIR=-I src -I$(COMPILER_DIR)/allegro-4.9.2/include -I$(COMPILER_DIR)/djgpp/i586-pc-msdosdjgpp/sys-include  -I$(COMPILER_DIR)/djgpp/include
LIBS_DIR=-L$(COMPILER_DIR)/djgpp/lib -L$(COMPILER_DIR)/allegro-4.9.2/lib
LIBS=-lalpng -lalmp3 -lalleg -liconv -lfreetype -lpng -lz
FLAGS=-O2 -s -Wall
#Includes and lib for UNIX
INCLUDES_DIR_UNIX=-I src -I$(COMPILER_DIR)/unix/alpng13/src -I$(COMPILER_DIR)/unix/almp3-2.0.5/include -I/usr/include/freetype2
LIBS_DIR_UNIX=-L$(COMPILER_DIR)/unix/alpng13 -L$(COMPILER_DIR)/unix/almp3-2.0.5/lib/linux -L/usr/lib
LIBS_UNIX=-lalpng -lalmp3 -lalleg -lfreetype
#Includes and lib for MINGW
INCLUDES_DIR_MINGW=-I src -I$(COMPILER_DIR)/unix/alpng13/src -I$(COMPILER_DIR)/unix/almp3-2.0.5/include -I$(COMPILER_DIR)/allegro/bin/include -I/mingw64/include/freetype2
LIBS_DIR_MINGW=-L$(COMPILER_DIR)/unix/alpng13 -L$(COMPILER_DIR)/unix/almp3-2.0.5/lib/mingw32 -L$(COMPILER_DIR)/allegro/bin/lib
LIBS_MINGW=-mwindows -lalmp3 -lalpng -lalleg -lfreetype

all: clean dosmenu gamemenu_dos gamemenu_unix

dosmenu:
	@echo "=================================================="
	@echo "===      Generating the main dosmenu.exe       ==="
	@echo "=================================================="
	$(CC) -O2 -s src/dosmenu.cpp -o gmenu/dosmenu.exe
	@echo "=========================================================================="
	@echo "=== Stubbing with PMODE instead of the default CWSDPMI that djgpp uses ==="
	@echo "=========================================================================="
	$(EXE2COFF) gmenu/dosmenu.exe
	rm gmenu/dosmenu.exe
	dd if=pmode/PMODSTUB.EXE of=gmenu/dosmenu.exe conv=notrunc bs=1
	dd if=gmenu/dosmenu >> gmenu/dosmenu.exe
	rm gmenu/dosmenu

gamemenu_dos:
	@echo "=================================================="
	@echo "=== Generating the main gamemenu.exe for MSDOS ==="
	@echo "=================================================="
	$(CCPLUS) $(FLAGS) -DDOS $(INCLUDES_DIR) $(LIBS_DIR) \
	src/utils/font/alfont.cpp main.cpp \
	-o gmenu/gamemenu.exe $(LIBS)

gamemenu_unix:
	@echo "=================================================="
	@echo "=== Generating the main gamemenu.exe for LINUX ==="
	@echo "=================================================="
	$(CXX) $(FLAGS) -DUNIX $(INCLUDES_DIR_UNIX) $(LIBS_DIR_UNIX) \
	src/utils/font/alfont.cpp main.cpp \
	-o gmenu/gamemenu $(LIBS_UNIX)
	
gamemenu_win:
	@echo "=================================================="
	@echo "=== Generating the main gamemenu.exe for MINGW ==="
	@echo "=================================================="
	$(MINGWCC) $(FLAGS) -DWIN $(INCLUDES_DIR_MINGW) $(LIBS_DIR_MINGW) \
	src/utils/font/alfont.cpp main.cpp \
	-o gmenu/gamemenu.exe $(LIBS_MINGW)

.PHONY: clean
clean:
	rm -f gmenu/dosmenu.exe gmenu/gamemenu.exe gmenu/gamemenu
