#!/bin/bash

USERDIR=python-codec
VERSION=`PYTHONPATH=$USERDIR/src/ python get_version.py`
USERZIP=dist/$USERDIR-$VERSION.tar.gz

python ../googlecode_upload.py -s "End-User version of the RL-Glue Python Codec $VERSION" -p rl-glue-ext --labels=Type-Installer,OpSys-All,Language-Python,Audience-User $USERZIP
