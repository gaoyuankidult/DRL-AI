#!/bin/bash
#Assume the glue is downloaded to rl-glue-trunk and installed to install_root
INSTALLDIR=`pwd`/install_root/usr/local
VERSION=`$INSTALLDIR/bin/rl_glue --pv`
#Created by download-build-glue
cd rl-glue-trunk

#configure for cross compile on Ubuntu/Debian
#./configure --host i586-mingw32msvc --enable-mingw

#configure for cross compile on Mac OS with tools installed from:
#http://crossgcc.rts-software.org/doku.php
PATH=$PATH:/usr/local/i386-mingw32-3.4.5/bin ./configure --host i386-mingw32 --enable-mingw


#Linux
#make

#Mac OS
PATH=$PATH:/usr/local/i386-mingw32-3.4.5/bin make

BinaryName=RL-Glue-Windows-Binary-$VERSION.zip
mkdir tmp
cp ../windows-binaries/README.txt ./tmp/
unix2dos ./tmp/README.txt
cp docs/Glue-Overview.pdf ./tmp/
cp docs/TechnicalManual.pdf ./tmp/
cp src/.libs/rl_glue.exe ./tmp/
cd tmp
zip $BinaryName rl_glue.exe Glue-Overview.pdf TechnicalManual.pdf README.txt
cp $BinaryName ../../dist/
cd ..
rm -Rf tmp



