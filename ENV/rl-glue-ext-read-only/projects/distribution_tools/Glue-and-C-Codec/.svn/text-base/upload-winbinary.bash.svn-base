#!/bin/bash

#Upload the Windows binary to Google Code
VERSION=`./install_root/usr/local/bin/rl_glue --pv`
FILENAME=RL-Glue-Windows-Binary-$VERSION.zip
THEFILE=./dist/$FILENAME
python ../googlecode_upload.py -s "Windows pre-compiled binary rl_glue.exe $VERSION" -p rl-glue-ext --labels=Type-Executable,OpSys-Windows  $THEFILE
