#!/bin/bash
#PBS -q normal
#PBS -l nodes=1:ppn=16:native
#PBS -l walltime=1:00:00
#PBS -N sedov2a0002
#PBS -o out.stdout
#PBS -e out.stderr
#PBS -M jobordner@ucsd.edu
#PBS -m abe
#PBS -V
# Start of user commands - comments start with a hash sign (#)

P=0002
t=2a

source include.sh
