#!/bin/bash

if [ -z $1 ]
then
  VERSION=UNKNOWN
  echo "You must pass a version!"
  exit 1
else
  VERSION=$1
fi

DIRBASE=lisp-codec
FULLDIR=${DIRBASE}-full
ASDFDIR=${DIRBASE}-asdf
DEVDIR=${DIRBASE}-dev
DISTDIR=dist

rm -Rf ${FULLDIR}
rm -Rf ${ASDFDIR}
rm -Rf ${DEVDIR}
rm -Rf ${DISTDIR}

# Making the development snapshot.

svn export http://rl-glue-ext.googlecode.com/svn/trunk/projects/codecs/Lisp ${DEVDIR}

# Preparing the *asdf* distribution.

mkdir ${ASDFDIR}
cp -R ${DEVDIR}/src/* ${ASDFDIR}
cp -R ${DEVDIR}/examples ${ASDFDIR}
mv ${ASDFDIR}/examples/ ${ASDFDIR}/rl-glue-examples/
mkdir ${ASDFDIR}/doc
cp ${DEVDIR}/doc/manual/lisp-codec.pdf ${ASDFDIR}/doc
cp -R ${DEVDIR}/doc/reference ${ASDFDIR}/doc

# Preparing the *full* distribution.

mkdir ${FULLDIR}
mkdir ${FULLDIR}/asdf
cp -R ${ASDFDIR}/* ${FULLDIR}/asdf
mv ${FULLDIR}/asdf/doc/ ${FULLDIR}/
cp setup.lisp ${FULLDIR}

cvs -f -d:pserver:anonymous:@cclan.cvs.sourceforge.net:/cvsroot/cclan login
cvs -fz3 -d:pserver:anonymous@cclan.cvs.sourceforge.net:/cvsroot/cclan export -D "1 day ago" asdf
cvs -f -d:pserver:anonymous@cclan.cvs.sourceforge.net:/cvsroot/cclan logout

mkdir ${FULLDIR}/asdf
mv asdf/asdf.lisp ${FULLDIR}/asdf/
rm -Rf asdf

SPLIT_SEQUENCE=split-sequence
USOCKET=usocket-0.4.1

wget -P ${FULLDIR}/asdf http://ftp.linux.org.uk/pub/lisp/experimental/cclan/${SPLIT_SEQUENCE}.tar.gz
wget -P ${FULLDIR}/asdf http://common-lisp.net/project/usocket/releases/${USOCKET}.tar.gz

for f in ${SPLIT_SEQUENCE} ${USOCKET}; do
    gzip -d ${FULLDIR}/asdf/${f}.tar.gz
    tar -xvf ${FULLDIR}/asdf/${f}.tar -C ${FULLDIR}/asdf
    rm -f ${FULLDIR}/asdf/${f}.tar
done

# Creating the distributions.

mkdir ${DISTDIR}

DEVFILEBASE=${DEVDIR}-${VERSION}
DEVTAR=${DEVFILEBASE}.tar
tar -cf ${DEVTAR} ${DEVDIR}
gzip ${DEVTAR}

FULLFILEBASE=${FULLDIR}-${VERSION}
FULLTAR=${FULLFILEBASE}.tar
tar -cf ${FULLTAR} ${FULLDIR}
gzip ${FULLTAR}

ASDFFILEBASE=${ASDFDIR}-${VERSION}
ASDFTAR=${ASDFFILEBASE}.tar
tar -cf ${ASDFTAR} ${ASDFDIR}
gzip ${ASDFTAR}

mv *.tar.gz ${DISTDIR}/

rm -Rf ${DEVDIR}
rm -Rf ${FULLDIR}
rm -Rf ${ASDFDIR}

# Uploading the distributions.

python ../googlecode_upload.py -s "RL-Glue Lisp Codec Full $VERSION" -p rl-glue-ext --labels=Type-Installer,OpSys-All,Language-Lisp,Audience-User ${DISTDIR}/${FULLTAR}.gz
python ../googlecode_upload.py -s "RL-Glue Lisp Codec ASDF $VERSION" -p rl-glue-ext --labels=Type-Installer,OpSys-All,Language-Lisp,Audience-User ${DISTDIR}/${ASDFTAR}.gz
python ../googlecode_upload.py -s "RL-Glue Lisp Codec Dev $VERSION" -p rl-glue-ext --labels=Type-Installer,OpSys-All,Language-Lisp,Audience-Dev ${DISTDIR}/${DEVTAR}.gz

# Updating the Wiki.

python substitute-lisp-strings.py $VERSION ${FULLFILEBASE} ${ASDFFILEBASE} ${DEVFILEBASE} 
cp Lisp.wiki ../wiki/lisp.new
cd ../wiki
svn up
mv lisp.new Lisp.wiki
svn commit Lisp.wiki -m "Automated update of Lisp wiki page."

