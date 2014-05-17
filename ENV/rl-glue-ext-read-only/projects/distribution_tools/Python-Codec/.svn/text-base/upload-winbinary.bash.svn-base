#!/bin/bash

#Upload the Python ZIP to Google Code
USERDIR=python-codec
VERSION=`PYTHONPATH=$USERDIR/src/ python get_version.py`
echo $VERSION
USERZIP=dist/$USERDIR-$VERSION-win32.zip

python ../googlecode_upload.py -s "Windows Python Codec installer $VERSION" -p rl-glue-ext --labels=Type-Installer,OpSys-Windows,Language-Python  $USERZIP
