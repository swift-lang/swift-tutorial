#!/bin/bash


SWIFT=$PWD/swift-trunk/bin
export PATH=$SWIFT:$PWD/bin:$PATH
module load openmpi
module load ffmpeg
cd bin; make; cd ..
