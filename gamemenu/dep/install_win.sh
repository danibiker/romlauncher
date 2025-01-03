#/bin/sh

currdir=$(pwd)

rm -fR dosdev
mkdir dosdev
echo "************************************************"
echo "*** Downloading for WINDOWS NATIVE COMPILING ***"
echo "************************************************"

cd dosdev
wget https://github.com/liballeg/allegro5/releases/download/4.4.3/allegro-4.4.3.zip
unzip allegro-4.4.3.zip
cd allegro
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
wget -O dosdev/unix/alpng13.zip -nc https://downloads.sourceforge.net/project/alpng/alpng/1.3/alpng13.zip
unzip -q dosdev/unix/alpng13.zip -d dosdev/unix/alpng13
cd dosdev/unix/alpng13
chmod 755 configure.bat
./configure.bat mingw
make