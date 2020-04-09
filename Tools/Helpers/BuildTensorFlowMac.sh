#!/bin/bash

# Make sure we're in the correct directory, at the root of the source tree.
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
echo $SCRIPT_DIR
pushd ${SCRIPT_DIR}/../../3rdParty/tensorflow
pwd
bazel build --jobs=$(sysctl -n hw.logicalcpu) --verbose_failures --cxxopt='-std=c++14' -c opt //tensorflow:libtensorflow_cc.so

if [ $? -ne 0 ]
then
    exit $?
fi

# Restore some headers that the build used but we will need to keep in sync (protobuf mainly)
tensorflow/contrib/makefile/download_dependencies.sh

../../Tools/Helpers/CopyTfLibs.sh
../../Tools/Helpers/CopyTfHeaders.sh

popd
