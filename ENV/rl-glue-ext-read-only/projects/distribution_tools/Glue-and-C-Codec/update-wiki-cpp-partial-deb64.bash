#!/bin/bash
CodecFileName=$(ls dist/c-codec*.gz)
VERSION=$(bash ./substr.bash $CodecFileName c-codec- .tar.gz)
FILENAME=rl-glue-c-codec_${VERSION}-1_amd64.deb
python filesubstitution.py $VERSION $FILENAME wiki/templates/CandCPP.wiki.debian64.template wiki/current/CandCPP.wiki.debian64


