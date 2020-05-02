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

# Run bazel build the first time
BUILD_CMD="bazel build --jobs=$(nproc) --verbose_failures --cxxopt='-std=c++11' -c opt //tensorflow:libtensorflow_cc.so"
echo $BUILD_CMD
$BUILD_CMD

# That's going to fail. Then fix the bazel-installed GRPC and try again
GRPC_DIR="$(ls -d ~/.cache/bazel/_bazel_${USER}/a*/external/grpc)"
echo $GRPC_DIR
(cd $GRPC_DIR; for f in src/core/lib/gpr/log_linux.cc src/core/lib/gpr/log_posix.cc src/core/lib/iomgr/ev_epollex_linux.cc; do sed -i 's/gettid/sys_gettid/g' "${f}"; done)

# Then run the build command a second time
echo $BUILD_CMD
$BUILD_CMD


if [ $? -ne 0 ]
then
    exit $?
fi

# Restore some headers that the build used but we will need to keep in sync (protobuf mainly)
tensorflow/contrib/makefile/download_dependencies.sh

../../Tools/Helpers/CopyTfLibs.sh
../../Tools/Helpers/CopyTfHeaders.sh

popd
