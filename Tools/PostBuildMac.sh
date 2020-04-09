#!/bin/bash

function CopyLibs()
{
   local libSrcDir=$1
   shift
   local libraryDir=$1
   shift
   local libs=("$@")

   for lib in "${libs[@]}"
   do
      srcLib="$libSrcDir/$lib"
      libName=$(basename "$lib")
      destLib="$libraryDir/$libName"

      echo
      if [[ "$srcLib" -nt "$destLib" ]]
      then
         echo "Copying $srcLib"

         # make sure we can overwrite the existing file
         test -f "$destLib" && chmod +uw "$destLib"

         cp "$srcLib" "$destLib"
      fi

      # Fix search paths
      chmod +uwr "$destLib"
      /usr/bin/install_name_tool -id "@rpath/$libName" "$destLib"

      echo "Change $srcLib to @rpath/$libName for $CONFIGURATION_BUILD_DIR/$EXECUTABLE_PATH"
      /usr/bin/install_name_tool -change "$srcLib" "@rpath/$libName" "$CONFIGURATION_BUILD_DIR/$EXECUTABLE_PATH"

   done
}


libraryDir=${TARGET_BUILD_DIR}/${CONTENTS_FOLDER_PATH}/Libraries
mkdir -p "${libraryDir}"

# For TF 1.14
tensorLibs=("libtensorflow_cc.so.1" "libtensorflow_framework.1.dylib")
CopyLibs "${PROJECT_DIR}/../../3rdParty/tensorflow/dist/lib" "${libraryDir}" "${tensorLibs[@]}"

# We need a later version of libarchive than MacOS provides
homebrewLibs=("libarchive/lib/libarchive.13.dylib" "libssh/lib/libssh.4.dylib" "openssl@1.1/lib/libcrypto.1.1.dylib")
CopyLibs "/usr/local/opt" "$libraryDir" "${homebrewLibs[@]}"

# Finally fix the link to libcrypto in libssh
/usr/bin/install_name_tool -change "/usr/local/opt/openssl@1.1/lib/libcrypto.1.1.dylib" "@rpath/libcrypto.1.1.dylib" "$libraryDir/libssh.4.dylib"
