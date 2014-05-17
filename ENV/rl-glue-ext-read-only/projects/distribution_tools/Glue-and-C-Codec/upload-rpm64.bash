#!/bin/bash

#Upload the 642-bit rpms to Google Code
VERSION=`./install_root/usr/local/bin/rl_glue --pv`
FILENAME=rl-glue-${VERSION}-1.x86_64.rpm
THEFILE=./dist/$FILENAME
python ../googlecode_upload.py -s "RPM package with pre-compiled binaries and libraries (64 bit) $VERSION" -p rl-glue-ext --labels=Type-Package,OpSys-Linux,Language-C,Audience-User  $THEFILE
