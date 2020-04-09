 #!/bin/bash

##
# Make sure we're in the correct directory, at the root of the source tree.
#
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
echo $SCRIPT_DIR
pushd ${SCRIPT_DIR}/../../3rdParty/cnpy

mkdir build
cd build

if ! cmake -DCMAKE_INSTALL_PREFIX='../dist' ..
then
    err=$?
    echo "Cnpy cmake build step failed."
    exit $err
fi

if ! make
then
    err=$?
    echo "Cnpy make build steps failed."
    exit $err
fi

if ! make install
then
    err=$?
    echo "Cnpy make install build step failed."
    exit $err
fi

popd
