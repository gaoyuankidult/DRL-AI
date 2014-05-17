#!/bin/bash
#Update the Deb Wiki Template
VERSION=`./install_root/usr/local/bin/rl_glue --pv`
FILENAME=rl-glue_${VERSION}-1_i386.deb
python filesubstitution.py $VERSION $FILENAME wiki/templates/RLGlueCore.wiki.debian32.template wiki/current/RLGlueCore.wiki.debian32


