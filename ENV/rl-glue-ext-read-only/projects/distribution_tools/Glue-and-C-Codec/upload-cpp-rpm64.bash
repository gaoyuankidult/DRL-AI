#!/bin/bash

CodecFileName=$(ls dist/c-codec*.gz)
VERSION=$(bash ./substr.bash $CodecFileName c-codec- .tar.gz)
FILENAME=rl-glue-c-codec-${VERSION}-1.x86_64.rpm
THEFILE=./dist/$FILENAME
python ../googlecode_upload.py -s "RPM pre-compiled C/C++ Codec (64 bit) $VERSION" -p rl-glue-ext --labels=Type-Package,OpSys-Linux,Language-C,Audience-User  $THEFILE
