#!/bin/bash
USERDIR=java-codec
DEVDIR=$USERDIR-dev
DISTDIR=dist
rm -Rf $USERDIR
rm -Rf $DEVDIR
rm -Rf $DISTDIR
svn export http://rl-glue-ext.googlecode.com/svn/trunk/projects/codecs/Java $DEVDIR

mkdir $DISTDIR
mkdir $USERDIR
cp $DEVDIR/products/JavaRLGlueCodec.jar $USERDIR
cp $DEVDIR/docs/JavaCodec.pdf $USERDIR
mv $DEVDIR/USER-README.txt $USERDIR/README.txt
mv $DEVDIR/DEV-README.txt $DEVDIR/README.txt
cp -R $DEVDIR/examples $USERDIR/examples
java org.rlcommunity.rlglue.codec.RLGlueCore --uninstall
VERSION=`java -jar $USERDIR/JavaRLGlueCodec.jar --version`

DEVTAR=$DEVDIR-$VERSION.tar
USERTAR=$USERDIR-$VERSION.tar

tar -cf $DEVTAR $DEVDIR
tar -cf $USERTAR $USERDIR
gzip $DEVTAR
gzip $USERTAR

DEVZIP=$DISTDIR/$DEVTAR.gz
USERZIP=$DISTDIR/$USERTAR.gz

mv *.gz $DISTDIR

rm -Rf $USERDIR
rm -Rf $DEVDIR

#Upload to Google Code
python ../googlecode_upload.py -s "Developer version of the RL-Glue Java Codec $VERSION" -p rl-glue-ext --labels=Type-Installer,OpSys-All,Language-Java,Audience-Dev $DEVZIP
python ../googlecode_upload.py -s "End-User version of the RL-Glue Java Codec $VERSION" -p rl-glue-ext --labels=Type-Installer,OpSys-All,Language-Java,Audience-User $USERZIP

# Update the Wiki
python substitute-java-strings.py $VERSION $DEVDIR-$VERSION $USERDIR-$VERSION
cp Java.wiki ../wiki/java.new
cd ../wiki
svn up
mv java.new Java.wiki
svn commit Java.wiki -m "Automated update of Java wiki page."
