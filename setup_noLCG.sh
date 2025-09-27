#! /bin/bash


if [ $# -ne 1 ]
then
    echo "USAGE: source setup_noLCG.sh [TBVER]"
    echo "where TBVER is the version of the TB to be run"
    echo "2023_SPS or 2024_SPS"
    return 
fi

if [ `basename $PWD` == TBDataPreparation ]
then
    echo 'This script should be run from one directory above TBDataPreparation'
    return
fi

TBVER=$1
echo "Setting up environment for "${TBVER}

IDEADIR=${PWD}

# Activate virtual environment after lsetup to avoid PYTHONPATH pollution
#. "${IDEADIR}"/ideadr-env/bin/activate 

# c.f. libtiff.so hack in init.sh
#export LD_LIBRARY_PATH=${IDEADIR}/ideadr-env/lib:$LD_LIBRARY_PATH 

export IDEARepo=${IDEADIR}/TBDataPreparation

if [ ! -d ${IDEARepo}/${TBVER} ]
then
    echo "TBVER "${IDEARepo}/${TBVER}" does not exist"
    return 
fi

export PATH=${IDEARepo}/${TBVER}/scripts:${IDEARepo}/2023_SPS/SIPM/converter:${PATH}
export PYTHONPATH=${IDEARepo}/${TBVER}/SIPM/scripts:${IDEARepo}/${TBVER}/scripts:${IDEARepo}/DreamDaqMon:${PYTHONPATH}

export BATCHPATH=${PATH}

