#!/bin/bash

# copied from @Brian Petersen
if [ "$0" != "TBDataPreparation/init.sh" ]
then
    echo "init.sh must be run as 'TBDataPreparation/init.sh'"
    exit 1
fi

if [ $# != 1 ]
then
    echo "USAGE: TBDataPreparation/init.sh [TBVER]"
    echo "where [TBVER] is the version of the test beam to be run"
    echo "Currently available are 2023_SPS, 2024_SPS or 2025_SPS"
    exit 1
fi

TBVER=$1
shift 1

#LCGVER=/cvmfs/sw.hsf.org/key4hep/setup.sh
LCGVER=/cvmfs/sft.cern.ch/lcg/views/LCG_108/x86_64-el9-gcc15-opt/setup.sh


if [ -f ${LCGVER} ]
then 
    source ${LCGVER}
else
    echo 'Cannot find LCG setup script ${LCGVER}. If this is expected, try using the setup_noLCG,sh scriipt'
fi

# Use cvmfs-venv to hack back against PYTHONPATH pollution
# c.f. https://github.com/matthewfeickert/cvmfs-venv for more information and examples
# download and run directly to avoid temporary files or adding a Git submodule
#bash <(curl -sL https://raw.githubusercontent.com/matthewfeickert/cvmfs-venv/v0.0.3/cvmfs-venv.sh) ideadr-env
#source ideadr-env/bin/activate

# FIXME: hack due to unversioned library in LCG
#ln -sf /lib64/libtiff.so "ideadr-env/lib/"
#ln -sf /lib64/libtiff.so.5 "ideadr-env/lib/"
export LD_LIBRARY_PATH="ideadr-env/lib:${LD_LIBRARY_PATH}"

# Generate a Env setup script for every log-in
echo "Generating settings file: 'setup.sh'"
if [ -e setup.sh ]
then 
    echo "  Moving existing 'setup.sh' to 'setup.sh.old'"
    mv setup.sh setup.sh.old
fi

cp TBDataPreparation/setup_noLCG.sh .

cat <<EOF > setup.sh

#source LCG

source ${LCGVER}

#set working directory

export IDEADIR=$PWD
export SamplePath=$PWD

#standard setup
source $PWD/TBDataPreparation/setup.sh ${TBVER}

EOF
