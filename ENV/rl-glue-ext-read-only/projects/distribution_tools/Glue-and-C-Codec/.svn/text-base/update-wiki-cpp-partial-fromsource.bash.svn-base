#!/bin/bash
CODECFILE=$(ls dist/ | grep c-codec)
CODECVERSION=$(bash substr.bash $CODECFILE c-codec- .tar.gz)

CODECNAME=c-codec-$CODECVERSION
CODECZIP=dist/$CODECFILE

python filesubstitution.py $CODECVERSION $CODECFILE wiki/templates/CandCPP.wiki.fromsource.template wiki/current/CandCPP.wiki.fromsource

