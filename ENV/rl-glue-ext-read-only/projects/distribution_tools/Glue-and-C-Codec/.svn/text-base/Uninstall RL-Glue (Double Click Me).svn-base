#!/bin/bash
CURRENTDIR=$(dirname "$0")

echo "--------------------------------------"
echo "RL-Glue and C/C++ Codec Uninstaller"
echo "--------------------------------------"
echo "This program will ask for your administrator password, and then "
echo " -- Delete the RL-Glue executable socket server"
echo " -- Remove all RL-Glue headers from /usr/local/include/rlglue"
echo " -- Remove the /usr/local/include/rlglue directory"
echo " -- Remove all RL-Glue libraries from /usr/local/lib"
echo "Do you wish to do this  (enter the number that corresponds to your choice)?"

OPTIONS="Yes No"
select opt in $OPTIONS; do
    if [ "$opt" = "Yes" ]; then
       RESULT=`sudo /usr/bin/python $CURRENTDIR/uninstall-resources/uninstall-rlglue.py`
       echo "...RL-Glue has been uninstalled."
       exit
    elif [ "$opt" = "No" ]; then
       echo "Uninstall cancelled by user input." 
       exit
    else
       echo "Didn't know what $opt was"
    fi
done