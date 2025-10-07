#!/bin/bash
RUN=$1

OUTDIR=/afs/cern.ch/user/i/ideadr/scratch/TB2025_H8/mergedNtuples

cd /afs/cern.ch/user/i/ideadr/TB2025/
source setup.sh

# Crea una cartella temporanea locale per il job
WORKDIR=$(mktemp -d  /tmp/job_${RUN}_XXXX)
cd ${WORKDIR}

pwd

python3 ${IDEADIR}/TBDataPreparation/2025_SPS/scripts/DR_makeRootFiles.py --runNumber ${RUN}

# Copia l'output nella directory finale
cp output.root ${OUTDIR}/merged_sps2025_run${RUN}.root

# Rimuovi i file temporanei
cd ..
#rm -rf ${WORKDIR}

