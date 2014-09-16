include ../../config
include ../../scripts/default.mk

#C compiler
HOSTCC=gcc
C_FLAGS = -std=c99 -O3
C_EXE = c_gaussian

PENCIL_SRC = pencil.c

INPUT_FILE = matrix1024.txt
ORIG_DATA = ${RODINIA_DIR}/data/gaussian/${INPUT_FILE}
ORIG_BENCH_DIR = ${RODINIA_DIR}/opencl/gaussian
C_SRC = ${ORIG_BENCH_DIR}/gaussianElim.cpp
OCL_SRC = pencil_host.c ${C_SRC} $(PPCG_PATH)/ocl_utilities.c

ORIG_EXE = ${ORIG_BENCH_DIR}/gaussian

OCL_EXE = ocl_gaussian

build: $(OCL_EXE)

ppcg: pencil.c
	$(PPCG) --opencl-pencil-runtime $(AUTOTUNER_PPCG_FLAGS) $^

%_host.c %_kernel.cl: $(PENCIL_SRC)
	$(PPCG) --target=opencl --opencl-pencil-runtime $(PPCGFLAGS) $^

$(OCL_EXE): pencil_host.c $(C_SRC)
	$(CC) $(CFLAGS) -x c $(OCL_SRC) -I../../include -I$(OPENCL_SDK)/include/ -I$(PPCG_PATH) -I$(PENCIL_RUNTIME) -I$(PENCIL_RUNTIME)/opencl-opt -L$(OPENCL_SDK)/lib/ -lOpenCL -L$(PENCIL_RUNTIME)/lib -locl_pencil -lm -o $@ -DPENCIL_DRIVER -I..

${ORIG_EXE}:
	make -C ${ORIG_BENCH_DIR} CC_FLAGS+="-I`pwd`/../../include -L${OPENCL_SDK}/lib -I${OPENCL_SDK}/include" CC=${CXX}

UPLOAD_FILES=pencil_kernel.cl $(OCL_EXE) run.sh ${ORIG_DATA}
UPLOAD_FILES_ORIG=${ORIG_EXE} ${ORIG_BENCH_DIR}/gaussianElim_kernels.cl orig/run.sh ${ORIG_DATA}


clean:
	rm -f $(C_EXE) $(OCL_EXE) pencil_kernel.h pencil_kernel.cl pencil_host.c

.PHONY: run.sh
run.sh:
	echo "#!/bin/bash" > $@
	echo "set -e" >> $@
	echo "set -o pipefail" >> $@
	echo "./ocl_gaussian -f ${INPUT_FILE} | grep TOTAL_TIME | sed -e 's/TOTAL_TIME: //'" >> $@
	chmod a+x $@

.PHONY: orig/run.sh
orig/run.sh:
	echo "#!/bin/bash" > $@
	echo "set -e" >> $@
	echo "set -o pipefail" >> $@
	echo "./gaussian -f ${INPUT_FILE} -p 0 -d 0 | grep TOTAL_TIME | sed -e 's/TOTAL_TIME: //'" >> $@
	chmod a+x $@

include ../../scripts/benchmark-run.mk

.PHONY: build ppcg clean
