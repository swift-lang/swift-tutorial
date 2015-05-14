#!/bin/bash

if [[ "$HOSTNAME" == *stampede* ]]
then
    echo "XSEDE/Stampede"
    module load mvapich2
    cd bin/
    make clean; make
    cd ..

elif [[ "$HOSTNAME" == *blacklight* ]]
then
    echo "XSEDE/Blacklight"
    module load ImageMagick/6.7.3-10

else
    SWIFT=$PWD/swift-trunk/bin
    export PATH=$SWIFT:$PWD/bin:$PATH
    module load openmpi
    module load ffmpeg
    cd bin; make; cd ..
fi



