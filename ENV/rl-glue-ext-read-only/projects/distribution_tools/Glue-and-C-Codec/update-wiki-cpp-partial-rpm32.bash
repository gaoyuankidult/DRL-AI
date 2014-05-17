#!/bin/bash
CodecFileName=$(ls dist/c-codec*.gz)
VERSION=$(bash ./substr.bash $CodecFileName c-codec- .tar.gz)
FILENAME=rl-glue-c-codec-${VERSION}-1.i386.rpm
python filesubstitution.py $VERSION $FILENAME wiki/templates/CandCPP.wiki.rpm32.template wiki/current/CandCPP.wiki.rpm32


