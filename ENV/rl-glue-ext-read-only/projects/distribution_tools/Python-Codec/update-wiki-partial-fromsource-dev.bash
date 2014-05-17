#!/bin/bash
USERDIR=python-codec
DEVDIR=$USERDIR-dev

VERSION=`PYTHONPATH=$DEVDIR/src/ python get_version.py`
echo $VERSION
DEVZIP=$DEVDIR-$VERSION.tar.gz



# Update the Wiki
python filesubstitution.py $VERSION $DEVZIP wiki/templates/Python.wiki.fromsource.dev.template wiki/current/Python.wiki.fromsource.dev

