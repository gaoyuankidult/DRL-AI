#!/bin/bash

#Build everything
#also builds the user and dev .tar.gz
bash ./download-python.bash
bash ./prepare-windows-package.bash


#Upload Everything
bash ./upload-fromsource-user.bash
bash ./upload-fromsource-dev.bash
bash ./upload-winbinary.bash

#Figure out the wiki updates
bash ./update-wiki-partial-fromsource-dev.bash
bash ./update-wiki-partial-fromsource-user.bash
bash ./update-wiki-windows.bash

#Coalesce, copy, and commit the wikis
bash ./prepare-joint-wiki.bash
bash ./upload-wikis.bash
