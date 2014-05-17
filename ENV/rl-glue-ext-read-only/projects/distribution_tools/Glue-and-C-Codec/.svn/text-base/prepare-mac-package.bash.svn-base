#!/bin/bash
#This file assumes that glue and codec have been built, and now it will make a mac package out of the install directory.
MACDIR=OSX-Package
DISTDIR=dist
VERSION=`./install_root/usr/local/bin/rl_glue --pv`
DISTNAME=RL-Glue-$VERSION-and-C-Codec
IMAGEFOLDER=$MACDIR/$DISTNAME
PACKAGENAME=$DISTNAME.pkg
IMAGENAME=$DISTNAME.dmg
mkdir $DISTDIR

rm -Rf $IMAGEFOLDER
mkdir $IMAGEFOLDER
mkdir $IMAGEFOLDER/codec
mkdir $IMAGEFOLDER/rl-glue

#Copy the Docs into the root of the package
cp codec-trunk/docs/*.pdf $IMAGEFOLDER/codec/
cp -R codec-trunk/examples $IMAGEFOLDER/codec/

cp rl-glue-trunk/docs/*.pdf $IMAGEFOLDER/rl-glue/
cp -R rl-glue-trunk/examples $IMAGEFOLDER/rl-glue/

#Make the uninstall script
python build-rlglue-uninstall-script.py

#Copy the uninstall command into the DMG
cp "Uninstall RL-Glue (Double Click Me)" $IMAGEFOLDER/
#Copy the uninstall python script too
cp -R uninstall-resources $IMAGEFOLDER/

rm -Rf $MACDIR/*.pkg
rm $MACDIR/*.dmg
#Create the package file
/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker --root install_root --id org.rlcommunity.rlglue --version 3.0 --title "RL-Glue 3.0 and C/C++ Codec" --resources $MACDIR/resources --out $MACDIR/$PACKAGENAME --root-volume-only

mv $MACDIR/$PACKAGENAME $IMAGEFOLDER
#Create the disk image
rm $DISTDIR/$IMAGENAME
hdiutil create -srcfolder $IMAGEFOLDER $DISTDIR/$IMAGENAME
rm -Rf $IMAGEFOLDER
