# .bashrc

# Source global definitions
if [ -f /etc/bashrc ]; then
	. /etc/bashrc
fi

# Uncomment the following line if you don't like systemctl's auto-paging feature:
# export SYSTEMD_PAGER=

# User specific aliases and functions
export TERM=xterm-256color
module use /tools/acfl/24.10/modulefiles/
module use /tools/acfl/24.04/modulefiles/
module load acfl/24.04 binutils/13.2.0 gnu/13.2.0 
module load armpl/24.04.0
export PATH=$PATH:/tools/openblas/acfl/24.04/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/tools/openblas/acfl/24.04/lib
#export PATH=$PATH:/tools/openblas/gnu/13.2.0/bin
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/tools/openblas/gnu/13.2.0/lib
export PATH=$PATH:/tools/openmpi/4.1.7/acfl/24.04/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/tools/openmpi/4.1.7/acfl/24.04/lib
export PATH=$PATH:/tools/forge/bin/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/tools/forge/lib
# Use OpenMPI compiler wrappers
export CC=$MPI_DIR/bin/mpicc
export CXX=$MPI_DIR/bin/mpicxx
export FC=$MPI_DIR/bin/mpif90

# Set base compilers for OpenMPI to use
export OMPI_CC=/tools/acfl/24.04/arm-linux-compiler-24.04_AmazonLinux-2/bin/armclang
export OMPI_CXX=/tools/acfl/24.04/arm-linux-compiler-24.04_AmazonLinux-2/bin/armclang++
export OMPI_FC=/tools/acfl/24.04/arm-linux-compiler-24.04_AmazonLinux-2/bin/armflang
#export OMPI_CC=/tools/acfl/24.04/gcc-13.2.0_AmazonLinux-2/bin/gcc
#export OMPI_CXX=/tools/acfl/24.04/gcc-13.2.0_AmazonLinux-2/bin/g++
#export OMPI_FC=/tools/acfl/24.04/gcc-13.2.0_AmazonLinux-2/bin/gfortran


# Boost configuration
export BOOST_ROOT=/fsx/home/etud14-2/codeaster-prerequisites-20240327-oss/bin/boost
export BOOST_INCLUDEDIR=$BOOST_ROOT/include
export BOOST_LIBRARYDIR=$BOOST_ROOT/stage/lib
export LD_LIBRARY_PATH=$BOOST_LIBRARYDIR:$LD_LIBRARY_PATH


# Omp setup
export OMP_PROC_BIND=true
export OMP_NUM_THREADS=$(lscpu | grep '^Core(s) per socket:' | awk '{print $4}' | xargs)
