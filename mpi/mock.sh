#!/bin/bash
echo "hi"
module load openmpi
mpirun -n 32 /scratch/midway/yadunand/swift-tutorial/mpi2/mpi_sum

