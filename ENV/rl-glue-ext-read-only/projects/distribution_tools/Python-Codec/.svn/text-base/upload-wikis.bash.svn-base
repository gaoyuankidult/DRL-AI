#!/bin/bash
#Called by do-finalize-wikis
cd wiki/current
svn up
svn commit -m "Automated update of the wiki partials."
cd ../../../wiki
svn up
svn commit -m "Automated update of wiki pages."

