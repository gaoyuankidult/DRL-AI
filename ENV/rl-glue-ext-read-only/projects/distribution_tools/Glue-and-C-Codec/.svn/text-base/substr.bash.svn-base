#!/bin/bash
if [ $# == 3 ]
then
	ppref="${1%${2}*}";
	ssuff="${1#*${3}}";
	nopref="${1#${ppref}${2}}";
	echo "${nopref%${3}${ssuff}}";
else
	echo "Usage: substr string prefix suffix"
fi