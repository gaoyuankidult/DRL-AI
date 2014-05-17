#!/bin/bash
#Prepare the GlueCore wiki to be committed, coalesces, copies
GLUEVERSION=`./install_root/usr/local/bin/rl_glue --pv`

python coalesce-gluecore.py
cp wiki/current/RLGlueCore.wiki ../wiki/core.new
cd ../wiki
svn up
mv core.new RLGlueCore.wiki
