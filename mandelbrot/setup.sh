#!/bin/bash

export PATH=$PWD/bin:$PATH
module load openmpi
module load ffmpeg
cd bin; make; cd ..
