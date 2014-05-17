#!/bin/bash
#This is a do-script because its not meant to be called by any others.  It's top level (for this dir at least)
#This will download and build RL-Glue and the Codec, build the Mac disk image, and upload it.
bash ./download-build-glue.bash
bash ./build-glue-windows.bash
bash ./update-wiki-gluecore-partial-windows.bash
bash ./upload-winbinary.bash
bash ./do-finalize-wikis.bash
