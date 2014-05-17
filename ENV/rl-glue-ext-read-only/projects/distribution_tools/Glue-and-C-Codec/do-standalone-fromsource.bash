#!/bin/bash
#C/C++ Codec tarball
#GlueCore Tarball

#Build everything
bash ./download-build-glue.bash
bash ./download-build-codec.bash
bash ./create-glue-dist.bash
bash ./create-codec-dist.bash
bash ./build-glue-native.bash



#Upload Everything
bash ./upload-gluecore-fromsource.bash
bash ./upload-cpp-fromsource.bash

#Figure out the wiki updates
bash ./update-wiki-cpp-partial-fromsource.bash 
bash ./update-wiki-gluecore-partial-fromsource.bash

#Coalesce, copy, and commit the wikis
bash ./do-finalize-wikis.bash
