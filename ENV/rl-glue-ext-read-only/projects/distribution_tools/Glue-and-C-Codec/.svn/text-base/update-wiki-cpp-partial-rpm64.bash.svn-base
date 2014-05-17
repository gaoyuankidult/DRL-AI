#!/bin/bash
CodecFileName=$(ls dist/c-codec*.gz)
VERSION=$(bash ./substr.bash $CodecFileName c-codec- .tar.gz)
FILENAME=rl-glue-c-codec-${VERSION}-1.x86_64.rpm
python filesubstitution.py $VERSION $FILENAME wiki/templates/CandCPP.wiki.rpm64.template wiki/current/CandCPP.wiki.rpm64


