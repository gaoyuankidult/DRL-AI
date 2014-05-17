#!/bin/bash
#This is a do-script because its not meant to be called by any others.  It's top level (for this dir at least)
#This will download and build RL-Glue and the Codec, build the Mac disk image, and upload it.
bash ./download-build-glue.bash
bash ./download-build-codec.bash

bash ./create-glue-dist.bash
bash ./create-codec-dist.bash

#This could be 32 or 64 
sh ./create-debian-package.sh 
bash ./update-wiki-gluecore-partial-deb64.bash
bash ./update-wiki-gluecore-partial-rpm64.bash

bash ./create-c-codec-deb.bash
bash ./update-wiki-cpp-partial-deb64.bash
bash ./update-wiki-cpp-partial-rpm64.bash

bash ./upload-deb64.bash
bash ./upload-rpm64.bash
bash ./upload-cpp-deb64.bash
bash ./upload-cpp-rpm64.bash
bash ./do-finalize-wikis.bash
