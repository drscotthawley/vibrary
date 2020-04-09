#!/bin/sh


##
# Make sure we're in the correct directory, at the root of the source tree.
#
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd ${SCRIPT_DIR}/../../3rdParty/tensorflow/contrib/makefile/downloads/protobuf/

./autogen.sh
./configure

make && make install
if [ $? ]
then
    err=$?
    echo "Protobuf make step failed."
    exit $err
fi

