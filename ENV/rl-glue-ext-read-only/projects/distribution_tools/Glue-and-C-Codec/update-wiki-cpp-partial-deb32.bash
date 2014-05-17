#!/bin/bash
#Update the Deb Wiki Template
CodecFileName=$(ls dist/c-codec*.gz)
VERSION=$(bash ./substr.bash $CodecFileName c-codec- .tar.gz)
FILENAME=rl-glue-c-codec_${VERSION}-1_i386.deb
python filesubstitution.py $VERSION $FILENAME wiki/templates/CandCPP.wiki.debian32.template wiki/current/CandCPP.wiki.debian32


