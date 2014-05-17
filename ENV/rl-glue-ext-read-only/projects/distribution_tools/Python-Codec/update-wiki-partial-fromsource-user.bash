#!/bin/bash
USERDIR=python-codec

VERSION=`PYTHONPATH=$USERDIR/src/ python get_version.py`
echo $VERSION
USERZIP=$USERDIR-$VERSION.tar.gz



# Update the Wiki
python filesubstitution.py $VERSION $USERZIP wiki/templates/Python.wiki.fromsource.user.template wiki/current/Python.wiki.fromsource.user

