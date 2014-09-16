BIN = bfs
OCL_UTILITIES = ${PPCG_PATH}/ocl_utilities.c

ORIG_BENCH_DIR = ${RODINIA_DIR}/opencl/bfs

ORIG_DATA = ${RODINIA_DIR}/data/bfs/inputGen

UPLOAD_FILES = pencil_kernel.cl ${BIN} run.sh
UPLOAD_FILES_ORIG = ${ORIG_BENCH_DIR}/Kernels.cl bfs-orig orig/run.sh

INPUT_SIZE = 4194304 # 2 ** 22
INPUT_SUFFIX= 4M
INPUT=graph${INPUT_SUFFIX}.txt

DATA_FILES = ${ORIG_DATA}/${INPUT}

include ../../config
include ../../scripts/default.mk
include ../../scripts/benchmark-run.mk

${DATA_FILES}:
	make -C ${ORIG_DATA}
	cd ${ORIG_DATA} && ./graphgen ${INPUT_SIZE} ${INPUT_SUFFIX}

ppcg: pencil.c
	${PPCG_PATH}/ppcg --opencl-pencil-runtime ${AUTOTUNER_PPCG_FLAGS} $^

bfs: pencil_host.c bfs.c ${OCL_UTILITIES}
	${CC} ${CFLAGS} ${LDFLAGS} $^ -o $@ -I$(PPCG_PATH) -I../../include -I$(PENCIL_RUNTIME) -I$(PENCIL_RUNTIME)/opencl-opt -L$(PENCIL_RUNTIME)/lib -locl_pencil

bfs-orig: ${ORIG_BENCH_DIR}/bfs.cpp
	${CXX} ${CXXFLAGS} ${LDFLAGS} -I `pwd`/../../include $< -o $@

run.sh:
	echo "#!/bin/bash" > $@
	echo "set -e" >> $@
	echo "set -o pipefail" >> $@
	echo "./bfs ${BOARD_DATA_DIR}/${INPUT} | grep TOTAL_TIME | sed -e 's/TOTAL_TIME: //'" >> $@
	chmod a+x $@

orig/run.sh:
	echo "#!/bin/bash" > $@
	echo "set -e" >> $@
	echo "set -o pipefail" >> $@
	echo "./bfs-orig ${BOARD_DATA_DIR}/${INPUT} | grep TOTAL_TIME | sed -e 's/TOTAL_TIME: //'" >> $@
	chmod a+x $@

build: bfs
