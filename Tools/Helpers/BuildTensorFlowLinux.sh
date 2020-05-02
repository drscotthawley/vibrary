#!/bin/bash

# Following https://www.tensorflow.org/install/source#setup_for_linux_and_macos

# Make sure we're in the correct directory, at the root of the source tree.
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
echo $SCRIPT_DIR
pushd ${SCRIPT_DIR}/../../3rdParty/tensorflow
pwd
# dependencies
pip install pip six numpy wheel setuptools mock 'future>=0.17.1'
pip install keras_applications --no-deps
pip install keras_preprocessing --no-deps

# patch grpc to avoid errors with gettid
(cd third_party; curl -O https://gist.githubusercontent.com/drscotthawley/8eb51af1b4c92c4f18432cb045698af7/raw/a4aaa020f0434c58b08e453e6d501553ceafbc81/grpc.patch; mv grpc.patch grpc_gettid_fix.patch)

bazel build --jobs=$(nproc) --verbose_failures --cxxopt='-std=c++11' -c opt //tensorflow:libtensorflow_cc.so

if [ $? -ne 0 ]
then
    exit $?
fi

# Restore some headers that the build used but we will need to keep in sync (protobuf mainly)
tensorflow/contrib/makefile/download_dependencies.sh

../../Tools/Helpers/CopyTfLibs.sh
../../Tools/Helpers/CopyTfHeaders.sh

popd
