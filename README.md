# Developer Startup

## Get the project

TODO: Expand startup for other platforms beyond MacOS

Clone the project and initialize the submodules in 3rdParty.

    $ git clone --recurse-submodules https://github.com/artandlogic/vibrary.git

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
        $ source bazel-0.24.1-installer-darwin-x86_64.sh
    
1. Install Java 8

        $ brew tap caskroom versions
        $ brew cask install homebrew/cask-versions/java8
    
1. Necessary Python support for Bazel builds
   
        $ pip install future
    
### Install other dependencies

Note that libarchive will not be linked by homebrew because macOS includes its own version. We need this specific build to handle the Linux server's zip archives. See the Projucer `other linker flags` for the location of this library.

*WARNING*: The PostBuild script(s) in the Tools directory and `Vibrary.jucer` file have references to specific versions of these libraries and may need to be udpated after the install or `brew update`.

    $ brew install libssh libarchive

### (Optional) Build Tensorflow

The PreBuild scripts will check if TensorFlow needs to be built, but to manually build it yourself:

    $ Tools/Helpers/BuildTensorFlowMac.sh


### (Optional) Build cnpy

The PreBuild scripts will check if Cnpy needs to be build, but to manually build it yourself:

    $ Tools/Helpers/BuildCnpy.sh

### Build Vibrary

The `Tools/BuildMac.sh` takes care of this, but manual steps are outlined here.

Currently there is only an Xcode exporter configured.

1. Open Incubator1.jucer with Projucer
1. Export the project files from Projucer (`command-shift-L`)
1. Build with Xcode.

Or to build and create an archive of the app, giving it a specific version

 `$ Tools/BuildMac.sh <version>` replacing version with the version number of the release. If no version is given the current version is bumped up one.



## Linux (still under construction)

### Create a virtual environment

These instructions assume a Python environment based on [pip & virtualenv](https://packaging.python.org/guides/installing-using-pip-and-virtual-environments/) on an Ubuntu Linux distribution.   Start a new environment via

```bash
$ cd vibrary
$ python3 -m venv env
$ source env/bin/activate
$ pip install --upgrade pip
```

### Install Tensorflow

We need the C++ bindings for tensorflow, which can only be obtained via building from source.

1. Install Bazel for TensorFlow build

   Binary builds are available [here](https://github.com/bazelbuild/bazel/releases/tag/0.24.1).  A basic Linux download & build can proceed as follows:

   ```bash
   $ curl -LO https://github.com/bazelbuild/bazel/releases/download/0.24.1/bazel-0.24.1-installer-linux-x86_64.sh
   $ chmod u+x bazel-0.24.1-installer-linux-x86_64.sh
   ```

   Then install either system-wide via 

   ```$ sudo ./bazel-0.24.1-installer-linux-x86_64.sh```

   or as a user-specific installation via 

   ```$ ./bazel-0.24.1-installer-linux-x86_64.sh --user```

1. Install Java 8. 

   ```bash
   $ sudo apt update
   $ sudo apt install openjdk-8-jre-headless
   ```

1. Run the helper script

   ```bash
   $ Tools/Helpers/BuildTensorFlowLinux.sh`
   ```

### Install other dependencies

```bash
$ sudo apt install libssh-4 libarchive-dev jackd2 libjack-jackd2-dev
```

**Build Cnpy** for reading numpy files:

```bash
$ Tools/Helpers/BuildCnpy.sh
```



### Build Vibrary

Run the `Projucer` JUCE app, and then choose "Open Existing Application" and select the `Vibrary.jucer` file in the `vibrary` directory.  

Then choose `File -> Save Project`.  Then you can make the Makefile:

```bash
$ cd Builds/LinuxMakefile
$ export CPPFLAGS="-I../../3rdParty/cnpy $CPPFLAGS"
$ make --trace -j 8 
```

**Problem:** the Makefile currently needs `-I../../3rdParty/cnpy/` added to it manually. 