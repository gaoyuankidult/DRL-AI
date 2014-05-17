#! /bin/sh

# deb=>rpm FROM SCRATCH (tar.gz required only) Script for RL-Glue
# Version: v1.1 - modified 10-3-2009
# Author: Iordanis Daroglou <idaroglo@csd.auth.gr>
# Supported build arches: i386, amd64
# Virtually any arch on which Debian is available could be supported..

# base dir
BASEDIR=$(pwd)

# get timestamp (string + numerical format to append to filenames)
TSTAMP=$(date -R)
TSTAMPNUM=$(date +"%Y%m%d_%H%M")

# the tar.gz sources will be copied and extracted here
# these dirs have to be created in the svn-repo, the old ones can be removed
DEBBASE_DIR=$BASEDIR/debian/buildbase
DEB_CFG=$BASEDIR/debian/debcfg/glue

# target dir, where the debs & rpms end up - also the place where we get the source tar.gz from
DIST=$BASEDIR/dist
rm $DIST/*.rpm
rm $DIST/*.deb

# variables for the filenames of the lintian logs - (un)comment according to arch
# if we want no logs, we can ommit them
THE_ARCH=$(uname -m)
if [ $THE_ARCH = x86_64 ]; then
	echo "Building 64 Bit Packages..."
#	LINTLOG=lintian-glue-64_$TSTAMPNUM.log
else
	echo "Building 32 Bit Packages..."
#	LINTLOG=lintian-glue-32_$TSTAMPNUM.log
fi

# prepare the updated package debian sources (section 'identical' for both arches)
#==================================================================================
cp $DIST/rlglue*.tar.gz $DEBBASE_DIR
cd $DEBBASE_DIR
tar -xzf *.tar.gz

# this one renames the rlglue-XX folder to rl-glue-XX, no need to know about version numbers etc.
# since we cannot do find -name bla -exec cd {} \;, we have to do some tricks to get the versioned dir
DIR_NAME=$(find -maxdepth 1 -type d -name "rlg*")
echo $DIR_NAME > tmpdir
sed -i "s/.\/rl/rl-/" tmpdir
sed -e "s/rl-glue-//" tmpdir > tmpver
DASHED=$(cat tmpdir)
VER=$(cat tmpver)

DASHED_DIR=$DEBBASE_DIR/$DASHED
mv $DIR_NAME $DASHED
rm tmp*

# cd to the rl-glue-XX dir and build the package
cd $DASHED_DIR
mkdir debian
cd $DASHED_DIR/debian
cp $DEB_CFG/* .

# before we start building, we change the timestamps and version no. in copyright/changelog
#===========================================================================================
sed -i "s/X/$VER/" changelog
sed -i "s/Wed, 11 Feb 2009 03:20:37 +0200/$TSTAMP/" changelog
sed -i "s/Wed Feb 25 18:43:29 EET 2009/$TSTAMP/" copyright

#  actual building takes place here
#====================================
cd ..
tar -zcf examples.tar.gz examples
dpkg-buildpackage -rfakeroot -uc -us


# check with lintian ==> build rpm with alien ==> cp rmp+deb to dist
# WARNING: deb check temporarily disabled due to some lintian issues (v2.2.7)
#============================================================================
#cd ..
#lintian rl-glue*.deb > $DIST/$LINTLOG
cd $DASHED_DIR
fakeroot alien -r -k ../rl-glue*.deb
cd ..
cp rl-glue*.deb $DIST
mv $DASHED_DIR/rl-glue*.rpm $DIST

#cleanup to prepare for next build :)
cd $DEBBASE_DIR
rm -r *
