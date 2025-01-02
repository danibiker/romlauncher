#/bin/sh

currdir=$(pwd)

rm -fR dosdev
mkdir dosdev
echo "**********************************************"
echo "*** Downloading for LINUX NATIVE COMPILING ***"
echo "**********************************************"
mkdir -p dosdev/unix/alpng13
wget -O dosdev/unix/alpng13.zip -nc https://downloads.sourceforge.net/project/alpng/alpng/1.3/alpng13.zip
unzip -q dosdev/unix/alpng13.zip -d dosdev/unix/alpng13
cd dosdev/unix/alpng13
chmod 755 configure
./configure
make
cd $currdir

echo "***********************************************************************************"
echo "*** Downloading for MSDOS DEPENDENCIES FOR CROSS COMPILING IN LINUX ENVIRONMENT ***"
echo "***********************************************************************************"
wget -nc https://github.com/andrewwutw/build-djgpp/releases/download/v3.4/djgpp-linux64-gcc1220.tar.bz2
tar xf djgpp-linux64-gcc1220.tar.bz2 -C dosdev
rm -f $currdir/djgpp-linux64-gcc1220.tar.bz2

LINKMEDIAFIRE=$(curl https://www.mediafire.com/?4iuhtnvi61f7pn3/allegro-dev-djgpp-gcc-4.9.2.tar.bz2 | tr "\"" "\n" | grep -E 'https://download[0-9]{4}\.mediafire\.com*')
wget -nc $LINKMEDIAFIRE
mkdir -p dosdev/allegro-4.9.2
tar -xf allegro-dev-djgpp-gcc-4.9.2.tar.bz2 -C dosdev/allegro-4.9.2
rm -f $currdir/allegro-dev-djgpp-gcc-4.9.2.tar.bz2

wget -nc https://www.delorie.com/pub/djgpp/deleted/v2gnu/licv192b.zip
unzip -q licv192b.zip -d dosdev/djgpp
rm -f $currdir/licv192b.zip

wget -nc https://www.delorie.com/pub/djgpp/current/v2tk/png1640b.zip
unzip -q png1640b.zip -d dosdev/djgpp
rm -f $currdir/png1640b.zip

wget -nc https://www.delorie.com/pub/djgpp/current/v2tk/zlb13b.zip
unzip -q zlb13b.zip -d dosdev/djgpp
rm -f $currdir/zlb13b.zip

wget -O freetype-2.13.3.tar.gz -nc https://downloads.sourceforge.net/project/freetype/freetype2/2.13.3/freetype-2.13.3.tar.gz
tar xzvf freetype-2.13.3.tar.gz

cd freetype-2.13.3
export CC=$currdir/dosdev/djgpp/bin/i586-pc-msdosdjgpp-gcc
./configure --host=i586-pc-msdosdjgpp --without-harfbuzz --without-brotli --without-zlib --without-bzip2
make
cp objs/.libs/* $currdir/dosdev/djgpp/lib
cp -r include/freetype include/ft2build.h $currdir/dosdev/djgpp/include

rm -f $currdir/freetype-2.13.3.tar.gz
rm -fR $currdir/freetype-2.13.3



