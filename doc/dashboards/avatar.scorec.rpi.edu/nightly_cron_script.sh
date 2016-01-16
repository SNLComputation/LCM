#!/bin/bash

# Crontab entry
#
# Run at midnight every day
#
# 00 00 * * * /lore/ghansen/nightly/nightly_cron_script.sh

source /usr/local/etc/bash_profile

export PATH=/users/ambrad/Trilinos/MPI_REL/bin:/users/ambrad/bin:/usr/local/bin:/usr/bin:/bin:/usr/X11R6/bin:${PATH}

export LD_LIBRARY_PATH=/usr/local/intel/11.1/069/mkl/lib/em64t:/users/ambrad/ompi-clang/lib:/users/ambrad/lib:/users/ambrad/lib64:/usr/local/parasolid/25.1.181/shared_object:/lore/ambrad/nightly/build/TrilinosInstallC11/lib:/usr/lib/:/usr/lib64:${LD_LIBRARY_PATH}

cd /lore/ambrad/nightly

LOG_FILE=/lore/ambrad/nightly/nightly_log.txt
if [ -f $LOG_FILE ]; then
  rm $LOG_FILE
fi

eval "env  TEST_DIRECTORY=/lore/ambrad/nightly SCRIPT_DIRECTORY=/users/ambrad/Albany/doc/dashboards/avatar.scorec.rpi.edu ctest -VV -S /users/ambrad/Albany/doc/dashboards/avatar.scorec.rpi.edu/ctest_nightly_mpi_opt_avatar.cmake" >> $LOG_FILE 2>&1


