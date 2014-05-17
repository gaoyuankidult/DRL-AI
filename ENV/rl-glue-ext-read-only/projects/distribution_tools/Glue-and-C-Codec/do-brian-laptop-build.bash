#!/bin/bash
#There are all the things that can be done in one big build on Brian's Laptop right now
#C/C++ Codec tarball
#GlueCore Tarball
#GlueCore + C/C++ Mac Binary Disk Image
#Windows .exe binary

#Build everything
bash ./download-build-glue.bash
bash ./download-build-codec.bash
bash ./create-glue-dist.bash
bash ./create-codec-dist.bash
bash ./prepare-mac-package.bash
bash ./build-glue-windows.bash
#Make sure we get rid of that windows build so we can get the version if we want it
bash ./build-glue-native.bash



#Upload Everything
bash ./upload-gluecore-fromsource.bash
bash ./upload-cpp-fromsource.bash
bash ./upload-macbinary.bash
bash ./upload-winbinary.bash

#Figure out the wiki updates
bash ./update-wiki-cpp-partial-fromsource.bash 
bash ./update-wiki-gluecore-partial-fromsource.bash
#Don't do debian, that will be in do-brian-linux-build.bash
bash ./update-wiki-gluecore-partial-windows.bash
bash ./update-wiki-partial-mac.bash

#Coalesce, copy, and commit the wikis
bash ./do-finalize-wikis.bash