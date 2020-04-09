# Server Configuration

Training occurs on a remote machine that has GPU capabilities to enable quick turnarounds and not require users to have expensive GPU hardware on their local machines.

The URL and account to use on the training machines is 

## Training process

When the user has created a data set and triggers training, the local application will

* connect to the remote training machine using SSH
* copy the training data to the home directory
* trigger the training
* monitor progress of the training by watching a log file
* copy back the trained Tensorflow SavedModel in a zip archive

## Example AWS GPU Instance configuration

During development we used an [AWS GPU Instance](https://docs.aws.amazon.com/dlami/latest/devguide/gpu.html) for training.

### System Requirements

Panotti uses Keras 2.2.4 which requires a later tensorflow-gpu (1.13.0) than the AWS default. That tensorflow requires CUDA 10.

### Training Stack

1. Setup CUDA 10.

    If `ll /usr/local/cuda` does not show a soft link to cuda-10.x, then recreate the softlink.

        $ sudo rm /usr/local/cuda
        $ sudo ln -s /usr/local/cuda.10.<x> /usr/local/cuda
        
    Replacing `<x>` with the minor version of the installed CUDA 10 


1. Install Dr. Hawley's Panotti library which manages the model and training.

    The app currently assumes panotti is stored in the user's home directory of the training user on the training machine.
    
        $ git clone https://github.com/drscotthawley/panotti
    
1. Install Keras

        $ mkdir -p 3rdParty/Keras
        $ git clone https://github.com/keras-team/keras.git 3rdParty
        $ sudo -H python3 -m pip install 3rdParty/keras --upgrade
        
    Alternative Keras install. It may be possible to install with pip through GitHub.

        $ sudo -H python3 -m pip install git+git://github.com/keras-team/keras.git

1. Reinstall tensorflow-gpu

        $ sudo -H python3 -m pip install tensorflow-gpu


1. The only dependencies needed from panotti/requirements.txt are

        $ pip install --user 'librosa>=0.5.1'
        $ pip install --user 'matplotlib>=2.0.0'
        $ pip install --user 'scikit-image>=0.14.1'
