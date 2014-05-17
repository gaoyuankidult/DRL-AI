#!/bin/bash

#Upload the Mac DMG to Google Code
GLUEVERSION=`./install_root/usr/local/bin/rl_glue --pv`
MACNAME=RL-Glue-$GLUEVERSION-and-C-Codec.dmg
MACDMG=dist/$MACNAME

python ../googlecode_upload.py -s "Mac OS X RL-Glue Core Project and C/C++ Codec Disk Image $GLUEVERSION" -p rl-glue-ext --labels=Type-Installer,OpSys-OSX  $MACDMG
