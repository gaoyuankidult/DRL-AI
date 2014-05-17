#!/bin/bash
USERDIR=python-codec
DEVDIR=$USERDIR-dev
DISTDIR=dist
rm -Rf $USERDIR
rm -Rf $DEVDIR
rm -Rf $DISTDIR
svn export http://rl-glue-ext.googlecode.com/svn/trunk/projects/codecs/Python $DEVDIR
VERSION=`PYTHONPATH=$DEVDIR/src/ python get_version.py`

rm -f $DEVDIR/WINDOWS-README.txt
cp -R $DEVDIR $USERDIR
mkdir $DISTDIR

mv $USERDIR/docs/PythonCodec.pdf $USERDIR/
rm -Rf $USERDIR/docs
mv $USERDIR/USER-README.txt $USERDIR/README.txt
rm $USERDIR/DEV-README.txt
mv $DEVDIR/DEV-README.txt $DEVDIR/README.txt
rm $DEVDIR/USER-README.txt

DEVTAR=$DEVDIR-$VERSION.tar
USERTAR=$USERDIR-$VERSION.tar
 
tar -cf $DEVTAR $DEVDIR
tar -cf $USERTAR $USERDIR
gzip $DEVTAR
gzip $USERTAR

rm $DISTDIR/*.gz
mv *.gz $DISTDIR
DEVZIP=$DISTDIR/$DEVTAR.gz
USERZIP=$DISTDIR/$USERTAR.gz


#Upload to Google Code
# python ../googlecode_upload.py -s "Developer version of the RL-Glue Python Codec $VERSION" -p rl-glue-ext --labels=Type-Installer,OpSys-All,Language-Python,Audience-Dev $DEVZIP
# python ../googlecode_upload.py -s "End-User version of the RL-Glue Python Codec $VERSION" -p rl-glue-ext --labels=Type-Installer,OpSys-All,Language-Python,Audience-User $USERZIP
# 
# # Update the Wiki
# python substitute-python-strings.py $VERSION $DEVDIR-$VERSION $USERDIR-$VERSION
# cp Python.wiki ../wiki/python.new
# cd ../wiki
# svn up
# mv python.new Python.wiki
# svn commit Python.wiki -m "Automated update of Python wiki page."
