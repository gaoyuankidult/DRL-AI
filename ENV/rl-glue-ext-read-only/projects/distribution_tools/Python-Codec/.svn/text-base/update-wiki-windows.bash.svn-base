#!/bin/bash
USERDIR=python-codec
VERSION=`PYTHONPATH=$USERDIR/src/ python get_version.py`
echo $VERSION
USERZIP=$USERDIR-$VERSION-win32.zip



# Update the Wiki
python filesubstitution.py $VERSION $USERZIP wiki/templates/Python.wiki.windows.template wiki/current/Python.wiki.windows

