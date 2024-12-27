#/bin/sh
/home/daniel/Apps/Develop/dos/djgpp/i586-pc-msdosdjgpp/bin/exe2coff ../gmenu/dosmenu.exe
rm ../gmenu/dosmenu.exe
dd if=PMODSTUB.EXE of=../gmenu/dosmenu.exe conv=notrunc bs=1
dd if=../gmenu/dosmenu >> ../gmenu/dosmenu.exe
rm ../gmenu/dosmenu
