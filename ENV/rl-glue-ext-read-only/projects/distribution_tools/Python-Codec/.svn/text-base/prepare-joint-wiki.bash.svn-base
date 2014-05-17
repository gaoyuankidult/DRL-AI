#!/bin/bash
#Prepare the Python wiki to be committed, coalesces, copies
USERDIR=python-codec
VERSION=`PYTHONPATH=$USERDIR/src/ python get_version.py`

python coalesce-python-wiki.py
cp wiki/current/Python.wiki ../wiki/Python.new
cd ../wiki
svn up
mv Python.new Python.wiki
