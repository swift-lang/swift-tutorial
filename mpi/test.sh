#!/bin/bash
#PBS -N halo_test
#PBS -l mppwidth=64
#PBS -l mppnppn=16
#PBS -l walltime=00:30:00

# Change to the directory that the job was submitted from
cd $PBS_O_WORKDIR

export MPICH_CPUMASK_DISPLAY=1
export MPICH_RANK_REORDER_METHOD=0
# Launch the parallel job to the allocated compute nodes
aprun -n 64 -N 16 /lustre/beagle/yadunandb/HALO/mmps 10

export MPICH_RANK_REORDER_METHOD=1
# Launch the parallel job to the allocated compute nodes
aprun -n 64 -N 16 /lustre/beagle/yadunandb/HALO/mmps 10

export MPICH_RANK_REORDER_METHOD=2
# Launch the parallel job to the allocated compute nodes
aprun -n 64 -N 16 /lustre/beagle/yadunandb/HALO/mmps 10
