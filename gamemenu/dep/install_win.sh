#/bin/sh

currdir=$(pwd)

rm -fR dosdev
mkdir dosdev
echo "************************************************"
echo "*** Downloading for WINDOWS NATIVE COMPILING ***"
echo "************************************************"

wget https://github.com/liballeg/allegro5/releases/download/4.4.3/allegro-4.4.3.zip
unzip -q allegro-4.4.3.zip -d dosdev
cd dosdev/allegro
mkdir build
mkdir bin
cd build
echo "-----------------------------------------------------------"
echo "1. Press configure and select 'MSYS Makefiles' on the combo" 
echo "2. Set MAKE_BUILD_TYPE to Release"
echo "3. Uncheck WANT_TESTS"
echo "4. Set the CMAKE_INSTALL_PREFIX to dosdev/allegro/bin"
echo "5. Press configure and Generate"
echo "6. Exit and let the script continue"
echo "-----------------------------------------------------------"
cmake-gui ..
make
make install
cd $currdir
cp dosdev/allegro/bin/lib/liballeg44.dll.a dosdev/allegro/bin/lib/liballeg.dll.a
cp -fR dosdev/allegro/bin/* /mingw64

mkdir -p dosdev/unix/alpng13
wget -O alpng13.zip -nc https://downloads.sourceforge.net/project/alpng/alpng/1.3/alpng13.zip
unzip -q alpng13.zip -d dosdev/unix/alpng13
cd dosdev/unix/alpng13
chmod 755 configure.bat
./configure.bat mingw
make

cd $currdir
wget -O almp3-2.0.5.tar.xz https://sourceforge.net/projects/almp3/files/2.0.5/almp3-2.0.5.tar.xz/download
tar -xJf almp3-2.0.5.tar.xz -C dosdev/unix
cd dosdev/unix/almp3-2.0.5
#comment line 6 of the makefile representing the line: TARGET=DJGPP_STATIC
sed -i '6s/^/#/' Makefile
#uncomment line 9 of the makefile representing the line: TARGET=MINGW32_STATIC
sed -i '7s/^#//' Makefile
#remove the text -march=pentium of the line 52
sed -i '31s/-march=pentium//' Makefile
make