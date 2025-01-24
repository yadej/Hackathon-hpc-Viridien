#!/bin/bash
#SBATCH --job-name=Base_mpi_mc         # Nom du travail
#SBATCH --output=output/Base_mpi_job.out         # Fichier de sortie
#SBATCH --error=output/Base_mpi_job.err          # Fichier d'erreur
#SBATCH --ntasks=90                  # Nombre total de tâches MPI (64 processus)
#SBATCH --nodes=1                    # Nombre de nœuds (1 nœud)
#SBATCH --cpus-per-task=1            # Nombre de cœurs par tâche (1 cœur par processus)
#SBATCH --time=01:00:00              # Temps limite (hh:mm:ss)

echo "=========== Job Information =========="
echo "Node List : "$SLURM_NODELIST
echo "my jobID : "$SLURM_JOB_ID
echo " Partition : " $SLURM_JOB_PARTITION
echo " submit directory : " $SLURM_SUBMIT_DIR
echo " submit host : " $SLURM_SUBMIT_HOST
echo " In the directory : " $PWD
echo "As the user : " $USER
echo "=========== Job Information =========="

module use /tools/acfl/24.04/modulefiles/
module load acfl/24.04 binutils/13.2.0 gnu/13.2.0 
export PATH=$PATH:/tools/openblas/acfl/24.04/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/tools/openblas/acfl/24.04/lib
#export PATH=$PATH:/tools/openblas/gnu/13.2.0/bin
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/tools/openblas/gnu/13.2.0/lib
export PATH=$PATH:/tools/openmpi/4.1.7/acfl/24.04/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/tools/openmpi/4.1.7/acfl/24.04/lib
export PATH=$PATH:/tools/forge/bin/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/tools/forge/lib
# Omp setup
export OMP_PROC_BIND=true
export OMP_NUM_THREADS=$(lscpu | grep '^Core(s) per socket:' | awk '{print $4}' | xargs)

nodelist=$(scontrol show hostname $SLURM_NODELIST)
printf "%s\n " "${nodelist[@]}" > output/nodefile

map --profile --output=profile.txt  mpirun --hostfile output/nodefile  ./BSMwithopt 100000    1000000
#mpirun --hostfile output/nodefile  ./BSMwithopt 10000000  1000000
#mpirun --hostfile output/nodefile  ./BSMwithopt 100000000 1000000
