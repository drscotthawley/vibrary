#!/bin/bash

# Following https://www.tensorflow.org/install/source#setup_for_linux_and_macos

# Make sure we're in the correct directory, at the root of the source tree.
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
echo $SCRIPT_DIR
pushd ${SCRIPT_DIR}/../../3rdParty/tensorflow
pwd
pip install -U --user pip six numpy wheel setuptools mock 'future>=0.17.1'
pip install -U --user keras_applications --no-deps
pip install -U --user keras_preprocessing --no-deps
bazel build --jobs=$(nproc) --verbose_failures --cxxopt='-std=c++14' -c opt //tensorflow:libtensorflow_cc.so

if [ $? -ne 0 ]
then
    exit $?
fi

# Restore some headers that the build used but we will need to keep in sync (protobuf mainly)
tensorflow/contrib/makefile/download_dependencies.sh

../../Tools/Helpers/CopyTfLibs.sh
../../Tools/Helpers/CopyTfHeaders.sh

popd
