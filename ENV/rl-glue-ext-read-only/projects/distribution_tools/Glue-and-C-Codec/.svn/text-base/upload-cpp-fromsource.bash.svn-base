#!/bin/bash
#Upload the C/C++ codec tarball to Google Code

CODECFILE=$(ls dist/ | grep c-codec)
CODECVERSION=$(bash substr.bash $CODECFILE c-codec- .tar.gz)
CODECZIP=dist/$CODECFILE
python ../googlecode_upload.py -s "RL-Glue C/C++ Codec $CODECVERSION" -p rl-glue-ext --labels=Type-Installer,OpSys-All,Language-C,Audience-User,Audience-Dev $CODECZIP
