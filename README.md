# Developer Startup

## Get the project

TODO: Expand startup for other platforms beyond MacOS

Clone the project and initialize the submodules in 3rdParty.

!!! Fix path 

    $ git clone --recurse-submodules git@gitlab.artlogic.com:ArtLogic/Incubator1.git`
        
Or if you already have cloned the project
        
    $ git submodule update --init


## Get the JUCE framework

The JUCE repository is not kept as a submodule so that you can use your own installation if you want. If you are not already set up, you can follow the instructions [here](https://github.com/WeAreROLI/JUCE).

After installation, build the Projucer, set its global paths, and you should be able to export the project files.

## Configure the Server

Instructions [here](./README-Server.md)

## MacOS

### Prepare to build Tensorflow C++ libraries

1. Install homebrew if you don't already have it. Instructions are [here](https://brew.sh)

1. Install Bazel for TensorFlow build

    The version of Tensorflow used requires Bazel 0.24.1, a version homebrew no longer has available. Install it manually.

    Binary builds are available [here](https://github.com/bazelbuild/bazel/releases/tag/0.24.1)
    
    macOS 10.15 will prevent `bazel-real` from opening because it isn't signed. You can work around this by downloading with `curl` instead of the browser. The following downloads the installer version. 
    
        $ curl -L -O https://github.com/bazelbuild/bazel/releases/download/0.24.1/bazel-0.24.1-installer-darwin-x86_64.sh
        $ . bazel-0.24.1-installer-darwin-x86_64.sh
       

1. Install Java 8

        $ brew tap caskroom versions
        $ brew cask install homebrew/cask-versions/java8
       
1. Necessary Python support for Bazel builds
    
        $ pip install future
    
### Install other dependencies

Note that libarchive will not be linked by homebrew because macOS includes its own version. We need this specific build to handle the Linux server's zip archives. See the Projucer `other linker flags` for the location of this library.

*WARNING*: The PostBuild script(s) in the Tools directory and `Vibrary.jucer` file have references to specific versions of these libraries and may need to be udpated after the install or `brew update`.

    $ brew install libssh libarchive
    
### Build Tensorflow

1. Configure Tensorflow.

    This is only necessary once.

         $ cd 3rdParty/tensorflow; ./configure
    
    The configure process will present options for the build.
     
    I chose the default python install and path, default for everything else, and gave `-mavx -msse4.2` for optimization options. 
     
    Your CPU may differ, and you may want CUDA support if you have an NVIDIA GPU. The prediction done locally isn't very taxing and doesn't require a GPU.
     
     When you are running the app, Tensorflow will log in the console output any optimizations for your CPU not being used. Keep in mind that CPUs without that optimization option will not be able to run your Tensorflow code should you be distributing your app.

1. Build TensorFlow (last built version is 1.14)

    The PreBuild scripts will check if TensorFlow needs to be built, but to manually build it yourself:

        $ Tools/Helpers/BuildTensorFlowMac.sh


### Build cnpy

The PreBuild scripts will check if Cnpy needs to be build, but to manually build it yourself:

    $ Tools/Helpers/BuildCnpy.sh

### Build Vibrary

Currently there is only an Xcode exporter configured.

1. Open Incubator1.jucer with Projucer
1. Export the project files from Projucer (`command-shift-L`)
1. Build with Xcode.

Or to build and create an archive of the app, giving it a specific version

1. `Tools/BuildMac.sh <version>` replacing version with the version number of the release. If no version is given the current version is bumped up one.
