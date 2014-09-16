#!/bin/bash

. `dirname $0`/../config

LOCK_FILE=/tmp/pencil_validation.lock
LOCK_FID=200
UPLOAD_FILES="$*"

(
  flock -e 200
  ssh ${BOARD} "rm -rf ${BOARD_DIR}; mkdir -p ${BOARD_DIR}"
  scp -r ${UPLOAD_FILES} ${BOARD}:${BOARD_DIR}
  ssh ${BOARD} "ulimit -t ${EXECUTION_TIME_LIMIT}; cd ${BOARD_DIR} && LD_LIBRARY_PATH=${BOARD_LIB_DIR}:\${LD_LIBRARY_PATH} ./run.sh"
) 200>${LOCK_FILE}
