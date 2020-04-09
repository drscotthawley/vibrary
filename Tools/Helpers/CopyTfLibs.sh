#!/bin/bash

##
# Tensorflow builds the c++ libs in a temp directory. If the machine reboots, they are cleaned up.
# This script moves them into a permanent location where the builds can link to them.
#
# Based on
#   https://itnext.io/how-to-use-your-c-muscle-using-tensorflow-2-0-and-xcode-without-using-bazel-builds-9dc82d5e7f80
#

##
# Make sure we're in the correct directory, at the root of the source tree.
#
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd ${SCRIPT_DIR}/../../3rdParty/tensorflow

mkdir -p dist/lib
cp bazel-bin/tensorflow/libtensorflow_cc.so* dist/lib
cp bazel-bin/tensorflow/libtensorflow_framework.*.dylib dist/lib/
