#!/bin/bash
#SBATCH --job-name=mpi_hello
#SBATCH --output=output%N.%j.out
#SBATCH --error=error%N.%j.err
#SBATCH --partition=development
#SBATCH --time=00:10:00
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=12
#module load openmpi
mpirun ./mpi_hello
echo "Done "
