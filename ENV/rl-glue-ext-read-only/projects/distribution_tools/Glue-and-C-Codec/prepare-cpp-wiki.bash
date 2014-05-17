#!/bin/bash
#Prepare the C/CPP Codec wiki to be committed, coalesces, copies

python coalesce-cpp.py
cp wiki/current/CandCPP.wiki ../wiki/CandCPP.new
cd ../wiki
svn up
mv CandCPP.new CandCPP.wiki


