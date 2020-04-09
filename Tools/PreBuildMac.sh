#!/bin/bash

##
# Make sure we're in the correct directory
#
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd ${SCRIPT_DIR}/../3rdParty

##
# Test for tensorflow build
#
if [[ ! -f tensorflow/dist/lib/libtensorflow_framework.1.dylib
   || ! -f tensorflow/dist/lib/libtensorflow_cc.so.1 ]]
then
    echo Building TensorFlow...
    ../Tools/Helpers/BuildTensorFlowMac.sh
fi

##
# Test for cnpy
#
if [[ ! -f cnpy/dist/include/cnpy.h
    || ! -f cnpy/dist/lib/libcnpy.a ]]
then
    echo Building Cnpy
    ../Tools/Helpers/BuildCnpy.sh
fi

# TODO: test for dependencies managed with homebrew

    
