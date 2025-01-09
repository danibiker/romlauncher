MSDOS DEPENDENCIES FOR CROSS COMPILING IN LINUX ENVIRONMENT
-----------------------------------------------------------

  AUTOMATIC DOWNLOAD OF LIBRARIES
---------------------------------

* First install dependencies

```
sudo pacman -S curl wget base-devel unzip allegro4 freetype2
or 
sudo apt install curl wget build-essential unzip liballegro4-dev allegro4-doc libfreetype-dev
```

* Next, Go to the dep directory and execute the file install.sh. All the required files specified in the
next section "MANUAL DOWNLOAD OF LIBRARIES" will be downloaded.

```
cd dep
./install.sh
```

Go to the root of the project and type:
```
make dosmenu gamemenu_dos
```

  MANUAL DOWNLOAD OF LIBRARIES
------------------------------

If you don't want to execute the script install.sh mentioned on the section above, execute the following steps, prior to executing 'make'

* First Download DJGPP toolchain from the releases section of https://github.com/andrewwutw/build-djgpp

    - Download from the latest release -> Prebuilt binary files (GCC 12.2.0)

    - Extract djgpp-linux64-gcc1220.tar.bz2 into a directory (in this case into the home/dosdev directory)
```
mkdir ~/dosdev
tar xf djgpp-linux64-gcc1220.tar.bz2 -C ~/dosdev
```

* Next download allegro toolchain from https://www.bttr-software.de/forum/board_entry.php?id=14983

    - Download from the link "For DJGPP GCC 4.9"
        http://www.mediafire.com/file/4iuhtnvi61f7pn3/allegro-dev-djgpp-gcc-4.9.2.tar.bz2

    - Extract allegro-dev-djgpp-gcc-4.9.2.tar.bz2 into a directory
```
mkdir -p ~/dosdev/allegro-4.9.2
tar -xf allegro-dev-djgpp-gcc-4.9.2.tar.bz2 -C ~/dosdev/allegro-4.9.2
```

* Download iconv, png, zlib from https://www.delorie.com/djgpp
    - Extract iconv -> Using link: https://www.delorie.com/pub/djgpp/deleted/v2gnu/licv192b.zip
        ```
      unzip licv192b.zip -d ~/dosdev/djgpp
        ```
    - Extract png -> Using link: https://www.delorie.com/pub/djgpp/current/v2tk/png1640b.zip
        ```
      unzip png1640b.zip -d ~/dosdev/djgpp
        ```
    - Extract zlib -> Using link: https://www.delorie.com/pub/djgpp/current/v2tk/zlb13b.zip
        ```
      unzip zlb13b.zip -d ~/dosdev/djgpp
        ```

* Download FreeType 2.13.3 from https://freetype.org/download.html

    - Save file using the link https://sourceforge.net/projects/freetype/files/freetype2/2.13.3/freetype-2.13.3.tar.gz/download

    - Extract freetype-2.13.3.tar.gz, compile and copy files to djgpp directories
```
        tar xzvf freetype-2.13.3.tar.gz
        cd freetype-2.13.3
        export CC=~/dosdev/djgpp/bin/i586-pc-msdosdjgpp-gcc
        ./configure --host=i586-pc-msdosdjgpp --without-harfbuzz --without-brotli --without-zlib --without-bzip2
        make
        cp objs/.libs/* ~/dosdev/djgpp/lib
        cp -r include/freetype include/ft2build.h ~/dosdev/djgpp/include
```

Finally, go to the root of the project and type:
```
make dosmenu gamemenu_dos
```

LINUX DEPENDENCIES FOR NATIVE COMPILING
---------------------------------------

* Install the dependencies:

```
sudo pacman -S curl wget base-devel unzip allegro4 freetype2
or 
sudo apt install curl wget build-essential unzip liballegro4-dev allegro4-doc libfreetype-dev
```

* Next, download allegro png from https://alpng.sourceforge.net and compile it:

```
cd alpng13
chmod 755 configure
./configure
make
```

* Lastly, download allegro mp3 from https://sourceforge.net/projects/almp3/files/2.0.5/almp3-2.0.5.tar.xz/download and compile it:

```
cd almp3-2.0.5
chmod 755 fixunix.sh
./fixunix.sh
#comment line 6 of the makefile representing the line: TARGET=DJGPP_STATIC
sed -i '6s/^/#/' Makefile
#uncomment line 9 of the makefile representing the line: TARGET=LINUX_STATIC
sed -i '9s/^#//' Makefile
#remove the text -march=pentium of the line 52
sed -i '52s/-march=pentium//' Makefile
make
```

Go to the root of the project and type:
```
make gamemenu_unix
```

WINDOWS DEPENDENCIES FOR MINGW COMPILING
---------------------------------------

* First, install msys2 from https://www.msys2.org/

Go to the msys2 instalation dir, start mingw64.exe and type the following to update needed dependencies

```
pacman -Syu
pacman -S mingw-w64-x86_64-gcc-base make curl wget unzip mingw-w64-x86_64-freetype mingw-w64-x86_64-ccmake mingw-w64-x86_64-cmake-gui mingw-w64-x86_64-gdb mingw-w64-i686-gcc mingw-w64-i686-freetype mingw-w64-i686-cmake mingw-w64-i686-gdb mingw-w64-i686-libvorbis mingw-w64-x86_64-libvorbis
```

  AUTOMATIC DOWNLOAD OF LIBRARIES
---------------------------------

* Go to the dep directory and execute the file install_win.sh while being inside mingw environment. All the required files specified in the
next section "MANUAL DOWNLOAD OF LIBRARIES" will be downloaded.

```
cd dep
./install_win.sh
```

* Go to the root of the project and type:
```
make gamemenu_win
```

  MANUAL DOWNLOAD OF LIBRARIES
------------------------------

* First, download allegro:

```
cd dosdev
wget https://github.com/liballeg/allegro5/releases/download/4.4.3/allegro-4.4.3.zip
unzip allegro-4.4.3.zip
cd allegro
mkdir build
mkdir bin
cd build
cmake-gui ..
```

On the cmake-gui frontend:
```
	1. Press configure and select 'MSYS Makefiles' on the combo
	2. Set MAKE_BUILD_TYPE to Release
	3. Uncheck WANT_TESTS
	4. Set the CMAKE_INSTALL_PREFIX to dosdev/allegro/bin
	5. Press configure and Generate
	6. Exit and let the script continue
```

```
make
make install
cp dosdev/allegro/bin/lib/liballeg44.dll.a dosdev/allegro/bin/lib/liballeg.dll.a
cp -fR dosdev/allegro/bin/* /mingw64
```

* Next, download allegro png from https://alpng.sourceforge.net and compile it:

```
mkdir -p dosdev/unix/alpng13
wget -O dosdev/unix/alpng13.zip -nc https://downloads.sourceforge.net/project/alpng/alpng/1.3/alpng13.zip
unzip -q dosdev/unix/alpng13.zip -d dosdev/unix/alpng13
cd dosdev/unix/alpng13
chmod 755 configure.bat
./configure.bat mingw
make
```

* Lastly, download allegro mp3 from https://sourceforge.net/projects/almp3/files/2.0.5/almp3-2.0.5.tar.xz/download and compile it:

```
cd almp3-2.0.5
#comment line 6 of the makefile representing the line: TARGET=DJGPP_STATIC
sed -i '6s/^/#/' Makefile
#uncomment line 9 of the makefile representing the line: TARGET=MINGW32_STATIC
sed -i '7s/^#//' Makefile
#remove the text -march=pentium of the line 52
sed -i '31s/-march=pentium//' Makefile
make
```
