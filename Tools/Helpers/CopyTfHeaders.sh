#!/bin/bash


# TODO: Make more repeatable using rsync
# An rsync version of one copy below (-L is critical since bazel-genfiles is a link)
#rsync -Ltru tensorflow/core ${destRoot}

# TODO: Error handling


##
# Tensorflow builds the c++ libs in a temp directory. If the machine reboots, they are cleaned up.
# This script moves them into a permanent location where the builds can find them.
#
# Based on
#   https://itnext.io/how-to-use-your-c-muscle-using-tensorflow-2-0-and-xcode-without-using-bazel-builds-9dc82d5e7f80
#

##
# Make sure we're in the correct directory, at the root of the source tree.
#
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd ${SCRIPT_DIR}/../../3rdParty/tensorflow

destRoot=dist/include/tensorflow
mkdir -p ${destRoot}/{,tensorflow,eigen}

##
# Copy headers
#
cp -r bazel-genfiles ${destRoot}
cp -r tensorflow/cc ${destRoot}
cp -r tensorflow/core ${destRoot}


cp -r third_party ${destRoot}

cp -r tensorflow/contrib/makefile/downloads/absl/absl ${destRoot}

cp -r bazel-tensorflow/external/eigen_archive/{unsupported,Eigen} ${destRoot}/eigen

##
# Preserve the TensorFlow protobuf generated files
#
mkdir -p ${destRoot}/protobuf
cp -r tensorflow/contrib/makefile/gen/proto ${destRoot}

##
# Preserve the version of protobuf headers used to build the lib
#
mkdir -p dist/include/protobuf
cp -r tensorflow/contrib/makefile/downloads/protobuf/src dist/include/protobuf
