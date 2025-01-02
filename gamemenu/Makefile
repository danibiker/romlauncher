#Paths to the djgpp compiler
COMPILER_DIR=./dep/dosdev
DJBIN=$(COMPILER_DIR)/djgpp/bin
DJDOSBIN=$(COMPILER_DIR)/djgpp/i586-pc-msdosdjgpp/bin
#Compiler of c and c++
CC=$(DJBIN)/i586-pc-msdosdjgpp-gcc
CCPLUS=$(DJBIN)/i586-pc-msdosdjgpp-g++
UNIXCCPLUS=/usr/bin/g++
#To generate a stubbed exe for DOS with PMODE
EXE2COFF=$(DJDOSBIN)/exe2coff
#To generate the main gamemenu program
INCLUDES_DIR=-I src -I$(COMPILER_DIR)/allegro-4.9.2/include -I$(COMPILER_DIR)/djgpp/i586-pc-msdosdjgpp/sys-include  -I$(COMPILER_DIR)/djgpp/include
LIBS_DIR=-L$(COMPILER_DIR)/djgpp/lib -L$(COMPILER_DIR)/allegro-4.9.2/lib
LIBS=-lalpng -lalleg -liconv -lfreetype -lpng -lz
FLAGS=-O2 -s -Wall
#Includes and lib for UNIX
INCLUDES_DIR_UNIX=-I src -I$(COMPILER_DIR)/unix/alpng13/src -I/usr/include/freetype2
LIBS_DIR_UNIX=-L$(COMPILER_DIR)/unix/alpng13 -L/usr/lib
LIBS_UNIX=-lalpng -lalleg -lfreetype

all: clean dosmenu gamemenu_dos gamemenu_unix

dosmenu:
	@echo "=== Generating the main dosmenu.exe ==="
	$(CC) -O2 -s src/dosmenu.cpp -o gmenu/dosmenu.exe
	@echo "=== Stubbing with PMODE instead of the default CWSDPMI that djgpp uses ==="
	$(EXE2COFF) gmenu/dosmenu.exe
	rm gmenu/dosmenu.exe
	dd if=pmode/PMODSTUB.EXE of=gmenu/dosmenu.exe conv=notrunc bs=1
	dd if=gmenu/dosmenu >> gmenu/dosmenu.exe
	rm gmenu/dosmenu

gamemenu_dos:
	@echo "=== Generating the main gamemenu.exe for MSDOS ==="
	$(CCPLUS) $(FLAGS) -DDOS $(INCLUDES_DIR) $(LIBS_DIR) \
	src/utils/font/alfont.cpp main.cpp \
	-o gmenu/gamemenu.exe $(LIBS)

gamemenu_unix:
	@echo "=== Generating the main gamemenu.exe for LINUX ==="
	$(UNIXCCPLUS) $(FLAGS) -DUNIX $(INCLUDES_DIR_UNIX) $(LIBS_DIR_UNIX) \
	src/utils/font/alfont.cpp main.cpp \
	-o gmenu/gamemenu $(LIBS_UNIX)

.PHONY: clean
clean:
	rm -f gmenu/dosmenu.exe gmenu/gamemenu.exe gmenu/gamemenu
