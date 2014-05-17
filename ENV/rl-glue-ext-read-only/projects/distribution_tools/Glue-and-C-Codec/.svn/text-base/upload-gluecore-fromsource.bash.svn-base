#!/bin/bash
#Upload the GlueCore tarball to google code

GLUEVERSION=`./install_root/usr/local/bin/rl_glue --pv`
GLUENAME=rlglue-$GLUEVERSION
GLUEZIP=dist/$GLUENAME.tar.gz
#Upload to Google Code
python ../googlecode_upload.py -s "RL-Glue Core Installation $GLUEVERSION" -p rl-glue-ext --labels=Type-Installer,OpSys-All,Language-C,Audience-User,Audience-Dev $GLUEZIP
