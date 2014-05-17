#!/bin/bash
bash ./download-build-glue.bash
bash ./download-build-codec.bash
bash ./create-glue-dist.bash
bash ./create-codec-dist.bash
bash ./prepare-mac-package.bash
bash ./cleanup.bash