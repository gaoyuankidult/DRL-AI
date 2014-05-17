#!/bin/bash
#Update the Mac Wiki template

GLUEVERSION=`./install_root/usr/local/bin/rl_glue --pv`
GLUENAME=rlglue-$GLUEVERSION
MACNAME=RL-Glue-$GLUEVERSION-and-C-Codec.dmg

MACDMG=dist/$MACNAME

#python ../googlecode_upload.py -s "Mac OS X RL-Glue Core Project and C/C++ Codec Disk Image $GLUEVERSION" -p rl-glue-ext --labels=Type-Installer,OpSys-OSX  $MACDMG

# Update the Wiki
python filesubstitution.py $GLUEVERSION $MACNAME wiki/templates/RLGlueCore.wiki.macbinary.template wiki/current/RLGlueCore.wiki.macbinary
#python substitute-codec-mac-strings.py $GLUEVERSION $GLUENAME $MACNAME
#cp CandCPP.wiki ../wiki/candcpp.new
#cd ../wiki
#svn up
#mv core.new RLGlueCore.wiki
#mv candcpp.new CandCPP.wiki
#svn commit RLGlueCore.wiki CandCPP.wiki -m "Automated update of RLGlueCore and CandCPP wiki pages."
