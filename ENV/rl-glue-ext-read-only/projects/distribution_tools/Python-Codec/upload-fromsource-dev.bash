#!/bin/bash

DEVDIR=python-codec-dev
VERSION=`PYTHONPATH=$DEVDIR/src/ python get_version.py`
DEVZIP=dist/$DEVDIR-$VERSION.tar.gz

python ../googlecode_upload.py -s "Developer version of the RL-Glue Python Codec $VERSION" -p rl-glue-ext --labels=Type-Installer,OpSys-All,Language-Python,Audience-Dev $DEVZIP
