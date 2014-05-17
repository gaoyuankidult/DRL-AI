#!/bin/bash
MATLAB_VERSION=1.04
C_CODEC_VERSION=2.0
LISP_VERSION=1.0
cd Glue-and-C-Codec
bash do-brian-laptop-build.bash $C_CODEC_VERSION
cd ..

cd Java-Codec
bash download-java.bash
cd ..

cd Matlab-Codec
bash download-matlab.bash $MATLAB_VERSION
cd ..

cd Python-Codec
do-python-release.bash
cd ..

cd Lisp-Codec
bash download-lisp.bash $LISP_VERSION
cd ..
