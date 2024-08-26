
if [ $# -ne 1 ]
then
    echo "USAGE: setup.sh [TBVER]"
    echo "where TBVER is the version of the TB to be run"
    echo "2023_SPS or 2024_SPS"
    exit 1
fi

TBVER=$1
echo "Setting up environment for "${TBVER}



# Activate virtual environment after lsetup to avoid PYTHONPATH pollution
#. "${IDEADIR}"/ideadr-env/bin/activate 

# c.f. libtiff.so hack in init.sh
#export LD_LIBRARY_PATH=${IDEADIR}/ideadr-env/lib:$LD_LIBRARY_PATH 

export IDEARepo=${IDEADIR}/TBDataPreparation

if [ ! -d ${IDEARepo}/${TBVER} ]
then
    echo "TBVER "${TBVER}" does not exist"
    exit 1
fi

export PATH=${IDEARepo}/${TBVER}/scripts:${IDEARepo}/2023_SPS/SIPM/converter:${PATH}
export PYTHONPATH=${IDEARepo}/${TBVER}/SIPM/scripts:${IDEARepo}/${TBVER}/scripts:${IDEARepo}/DreamDaqMon:${PYTHONPATH}

export BATCHPATH=${PATH}

