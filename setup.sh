#! /bin/bash


if [ $# -ne 1 ]
then
    echo "USAGE: setup.sh [TBVER]"
    echo "where TBVER is the version of the TB to be run"
    echo "2023_SPS, 2024_SPS or 2025_SPS"
    exit 1
fi

TBVER=$1
echo "Setting up environment for "${TBVER}




export IDEARepo=${IDEADIR}/TBDataPreparation

if [ ! -d ${IDEARepo}/${TBVER} ]
then
    echo "TBVER "${TBVER}" does not exist"
    exit 1
fi

if [ ! -d ${IDEADIR}/build ]
then
    mkdir ${IDEADIR}/build
    cd ${IDEADIR}/build
    cmake ${IDEARepo}/${TBVER}/SIPM
    make
fi

export LD_LIBRARY_PATH=${IDEADIR}/build:${LD_LIBRARY_PATH}
export PATH=${IDEARepo}/${TBVER}/scripts:${IDEARepo}/${TBVER}/SIPM/converter:${PATH}
export PYTHONPATH=${IDEARepo}/${TBVER}/SIPM/scripts:${IDEARepo}/${TBVER}/scripts:${IDEARepo}/DreamDaqMon:${PYTHONPATH}
export BATCHPATH=${PATH}

