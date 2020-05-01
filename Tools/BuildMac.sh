#! /bin/bash

projectName=Vibrary
juceProject=$projectName.jucer

echo juce project $juceProject
if [[ ! -f $juceProject ]]; then
    echo "Run from the root directory of the project, i.e. where $projectName.jucer file is."
    exit 1;
fi


#--------------------------------------------------------------------------------
echo Preparing platform projects...

# I symlink my projucer build into /Applications
projucer=/Applications/Projucer.app/Contents/MacOS/Projucer
if [ ! -f "$projucer" ]; then
    echo Could not find Projucer in expected location: $projucer
    echo Try creating a symlink to your build there.
    exit 1
fi

if [[ $# -eq 1 ]]; then
    echo Setting version to $1.
    versionCmd="--set-version $1"
else
    echo Bumping version.
    versionCmd="--bump-version"
fi

$projucer $versionCmd $juceProject
if [[ ! $? ]]; then
    echo "Failed to set version with $versionCmd"
    exit 1
fi

$projucer --resave $juceProject
if [[ ! $? ]]; then
    echo "Failed to save and export $juceProject"
    exit 1
fi


#--------------------------------------------------------------------------------
echo Building...

macProjDir=Builds/MacOSX
targetDir=$macProjDir/build/Release

cd $macProjDir

/usr/bin/xcodebuild -configuration Release 2>&1 > ../../build.log
if [[ ! $? ]]; then
    echo "Failed to build. See build.log"
    exit 1
fi

cd -


#--------------------------------------------------------------------------------
echo Archiving...

if [[ ! -d $targetDir ]]; then
    echo "macOS target build folder not found: $targetDir"
    exit 1
fi

version=$($projucer --get-version $proj)
archive=Vibrary-$($projucer --get-version $juceProject).zip

cd $targetDir
/usr/bin/zip -r $archive Vibrary.app
cd -

mv $targetDir/$archive .

# Mac only
if [[ $OSTYPE == "darwim"* ]]
then
    open .
fi
