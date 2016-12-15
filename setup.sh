######################### CONFIGS ###########################

export BEAGLE_USERNAME=""
export BEAGLE_PROJECT=""
export MIDWAY_USERNAME=""
export AWS_CREDENTIALS_FILE=""
export URL_OF_AD_HOC_MACHINE_1=""
export AD_HOC_1_USERNAME=""
export AD_HOC_N_USERNAME=""
export OSG_USERNAME=""
export OSG_PROJECT=""
export BLUES_USERNAME=""
export CLOUD_HEADNODE="http://HEADNODE_ADDRESS:50010"
#############################################################


# ensure that this script is being sourced
if [ ${BASH_VERSINFO[0]} -gt 2 -a "${BASH_SOURCE[0]}" = "${0}" ] ; then
  echo ERROR: script ${BASH_SOURCE[0]} must be executed as: source ${BASH_SOURCE[0]}
  exit 1
fi


# Setting scripts folder to the PATH env var.
TUTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [ _$(which cleanup 2>/dev/null) != _$TUTDIR/bin/cleanup ]; then
  echo Adding $TUTDIR/bin:$TUTDIR/app: to front of PATH
  PATH=$TUTDIR/bin:$TUTDIR/app:$PATH
else
  echo Assuming $TUTDIR/bin:$TUTDIR/app: is already at front of PATH
fi


# Set PATH for specific hosts

if hostname | grep bridges; then

    module load java # Get Oracle Java 1.8.x
    SWIFT=/home/wilde/swift/rev/swift-0.96.2
    PATH=$SWIFT/bin:$JAVA:$PATH

    module unload mpi/intel_mpi
    module load mpi/gcc_mvapich
    export CC="mpicc"
    echo "modules adjusted: unloaded mpi/intel_mpi; loaded mpi/gcc_mvapich"
    echo "This list should should show mpi/gcc_mvapich as only MPI:"
    module list

elif hostname -f | grep "edison" ; then
    echo "Configuring for Edison"
    module swap PrgEnv-intel PrgEnv-gnu
    export PATH=$PWD/part07:$PATH
    export CC="cc"

    pushd .
    cd part07/ ;
    make clean ; make
    popd

    pushd .
    cd part08/ ; rm hipi
    make clean ; make
    popd


elif hostname -f | grep "ncsa.illinois.edu" ; then
    # BW does not have a clearly identifiable hostname on the login nodes

    module load java # Oracle java java/jdk1.8.0_51
    module unload PrgEnv-cray/5.2.82
    module load PrgEnv-gnu/5.2.82
    SWIFT=~wilde/scratch/swift/rev/swift-0.96.2
    PATH=$SWIFT/bin:$PATH
    export CC="cc"

elif hostname | grep comet; then

    JAVA=/oasis/scratch/comet/xdtr1/temp_project/jdk1.8.0_91/bin
    SWIFT=/oasis/scratch/comet/xdtr1/temp_project/swift/swift-0.96.2/bin
    PATH=$SWIFT:$JAVA:$PATH

elif hostname | grep workflow.iu; then

    SWIFT=/opt/swift/swift-0.96.2/bin
    JAVA=$(echo /opt/swift/jdk1.*/bin)
    PATH=$SWIFT:$JAVA:$PATH
    export X509_USER_PROXY=/tmp/x509.$USER.$RANDOM

elif [ -d /usr/local/bin/swift-trunk ] && [ -d /usr/local/bin/jdk1.7.0_51 ]; then

    JAVA=/usr/local/bin/jdk1.7.0_51/bin
    SWIFT=/usr/local/bin/swift-trunk/bin
    PATH=$JAVA:$SWIFT:$PATH

elif [ -d /opt/swift/swift-0.96.1 ] && [ -d /opt/swift/jdk1.7.0_51 ]
then
    export SWIFT=/opt/swift/swift-0.96.1/bin
    export JAVA=/opt/swift/jdk1.7.0_51/bin
    export PATH=$SWIFT:$JAVA:$PATH
    export X509_USER_PROXY=/tmp/x509.$USER.$RANDOM
fi

echo "Swift version is $(swift -version)"

return
