#!/bin/bash

#Downloads the glue and installs it to install_root
INSTALLDIR=`pwd`/install_root/usr/local
cd rl-glue-trunk
#Disable shared so that we can package them up and relocate the libraries.
./configure --prefix=$INSTALLDIR
make
make install

