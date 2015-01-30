#!/bin/bash
#PBS -N halo_test
#PBS -l mppwidth=12288
#PBS -l walltime=01:00:00
#PBS -l advres=yadunandb.2763
#PBS -q batch

# Change to the directory that the job was submitted from
cd $PBS_O_WORKDIR

export MPICH_CPUMASK_DISPLAY=1

export MPICH_RANK_REORDER_METHOD=0

# Launch the parallel job to the allocated compute nodes
#datasize=(1 2) # 4 8 16 32 64 128 256 512 1024 2048 4096 8192 10240 16384 24576 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608)
datasize=(1 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 10240 16384 24576 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608)
for i in ${datasize[*]}
do
    aprun -S 4 -n 8192 -N 16 /lustre/beagle/yadunandb/HALO/mmps $i
done
echo "RANK ORDER METHOD 0 ===============================================" 1>&2

export MPICH_RANK_REORDER_METHOD=1
# Launch the parallel job to the allocated compute nodes
#datasize=(1 2) # 4 8 16 32 64 128 256 512 1024 2048 4096 8192 10240 16384 24576 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608)
datasize=(1 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 10240 16384 24576 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608)
for i in ${datasize[*]}
do
    aprun -S 4 -n 8192 -N 16 /lustre/beagle/yadunandb/HALO/mmps $i
done
echo "RANK ORDER METHOD 1 ===============================================" 1>&2

export MPICH_RANK_REORDER_METHOD=2
# Launch the parallel job to the allocated compute nodes
#datasize=(1 2) # 4 8 16 32 64 128 256 512 1024 2048 4096 8192 10240 16384 24576 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608)
datasize=(1 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 10240 16384 24576 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608)
for i in ${datasize[*]}
do
    aprun -S 4 -n 8192 -N 16 /lustre/beagle/yadunandb/HALO/mmps $i
done
echo "RANK ORDER METHOD 2 ===============================================" 1>&2

