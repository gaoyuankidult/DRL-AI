#!/bin/bash

#Upload the 64-bit debs to Google Code
VERSION=`./install_root/usr/local/bin/rl_glue --pv`
FILENAME=rl-glue_${VERSION}-1_amd64.deb
THEFILE=./dist/$FILENAME
python ../googlecode_upload.py -s "Debian/Ubuntu pre-compiled binaries and libraries (64 bit) $VERSION" -p rl-glue-ext --labels=Type-Package,OpSys-Debian-Ubuntu,Language-C,Audience-User  $THEFILE
