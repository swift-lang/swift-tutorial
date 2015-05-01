#!/bin/bash

gcc -I/opt/cray/mpt/default/gni/mpich2-gnu/49/include -L/opt/cray/mpt/default/gni/mpich2-gnu/49/lib -lmpich pi.c -o pi
