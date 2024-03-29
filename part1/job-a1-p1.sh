#!/bin/bash
#SBATCH --nodes=1
#SBATCH --cpus-per-task=8
#SBATCH --time=0:10:00
#SBATCH --job-name a1_p1
#SBATCH --output=a1_p1_%j.out

module load anaconda3/5.2.0
./part1
make run
