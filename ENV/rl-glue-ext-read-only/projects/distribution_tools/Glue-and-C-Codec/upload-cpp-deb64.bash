#!/bin/bash

CodecFileName=$(ls dist/c-codec*.gz)
VERSION=$(bash ./substr.bash $CodecFileName c-codec- .tar.gz)
FILENAME=rl-glue-c-codec_${VERSION}-1_amd64.deb
THEFILE=./dist/$FILENAME
python ../googlecode_upload.py -s "Debian/Ubuntu pre-compiled C/C++ Codec (64 bit) $VERSION" -p rl-glue-ext --labels=Type-Package,OpSys-Debian-Ubuntu,Language-C,Audience-User  $THEFILE
