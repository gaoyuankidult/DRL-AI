#!/bin/bash
#Update the Windows Wiki Template

VERSION=`./install_root/usr/local/bin/rl_glue --pv`
FILENAME=RL-Glue-Windows-Binary-$VERSION.zip
python filesubstitution.py $VERSION $FILENAME wiki/templates/RLGlueCore.wiki.windows.template wiki/current/RLGlueCore.wiki.windows
