#!/bin/bash

if [[ "$HOSTNAME" == *stampede* ]]
then
    echo "STAMPEDE"
    module load mvapich2
    cd bin/
    make clean; make
    cd ..
else
    SWIFT=$PWD/swift-trunk/bin
    export PATH=$SWIFT:$PWD/bin:$PATH
    module load openmpi
    module load ffmpeg
    cd bin; make; cd ..
fi



