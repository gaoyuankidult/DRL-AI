#!/bin/bash

#Upload the 32-bit debs to Google Code
CodecFileName=$(ls dist/c-codec*.gz)
VERSION=$(bash ./substr.bash $CodecFileName c-codec- .tar.gz)
FILENAME=rl-glue-c-codec-${VERSION}-1.i386.rpm
THEFILE=./dist/$FILENAME
python ../googlecode_upload.py -s "RPM pre-compiled C/C++ Codec (32 bit) $VERSION" -p rl-glue-ext --labels=Type-Package,OpSys-Linux,Language-C,Audience-User  $THEFILE
