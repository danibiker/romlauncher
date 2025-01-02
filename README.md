# romlauncher
 A multisystem rom launcher primarily made to work in MSDOS

LINUX DEPENDENCIES FOR NATIVE COMPILING
---------------------------------------

* First, download allegro png from https://alpng.sourceforge.net and compile it:
```
cd alpng13
chmod 755 configure
./configure
make
```

* Install the dependencies:
```
pacman -S allegro4 freetype2
```

Go to the root of the project and type:
```
make gamemenu_unix
```

MSDOS DEPENDENCIES FOR CROSS COMPILING IN LINUX ENVIRONMENT
-----------------------------------------------------------

AUTOMATIC DOWNLOAD OF LIBRARIES
-------------------------------

Go to the dep directory and execute the file install.sh. All the required files specified in the
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
----------------------------

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



