#!/bin/bash
#Update the Glue tarball wiki

GLUEVERSION=`./install_root/usr/local/bin/rl_glue --pv`

GLUENAME=rlglue-$GLUEVERSION
GLUEFILE=$GLUENAME.tar.gz

#Upload to Google Code
#python ../googlecode_upload.py -s "RL-Glue Core Installation $GLUEVERSION" -p rl-glue-ext --labels=Type-Installer,OpSys-All,Language-C,Audience-User,Audience-Dev $GLUEZIP

# Update the Wiki
python filesubstitution.py $GLUEVERSION $GLUEFILE wiki/templates/RLGlueCore.wiki.fromsource.template wiki/current/RLGlueCore.wiki.fromsource
