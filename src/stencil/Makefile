BENCH_SRC = ${SHOC_DIR}/src/opencl/level1/stencil2d/
BIN = ${SHOC_DIR}/bin/Serial/OpenCL/Stencil2D
OCL_UTILITIES_C = ${PPCG_PATH}/ocl_utilities.c
OCL_UTILITIES_H = ${PPCG_PATH}/ocl_utilities.h

UPLOAD_FILES = stencil_kernel.cl ${BIN} run.sh
UPLOAD_FILES_ORIG = ${BIN} orig/run.sh

SIZE=4

include ../../config
include ../../scripts/default.mk
include ../../scripts/benchmark-run.mk

ppcg: stencil.pencil
	${PPCG_PATH}/ppcg --opencl-pencil-runtime ${AUTOTUNER_PPCG_FLAGS} $^

${BIN}: build

build: stencil_host.c
	cp stencil_host.c ${BENCH_SRC}
	cp ${OCL_UTILITIES_C} ${BENCH_SRC}
	cp ${OCL_UTILITIES_H} ${BENCH_SRC}
	make -C ${SHOC_DIR}/src/common
	make -C ${SHOC_DIR}/src/opencl/common
	make -C ${BENCH_SRC} clean
	make -C ${BENCH_SRC} PENCIL_RUNTIME=${PENCIL_RUNTIME}
	make -C ${BENCH_SRC} PENCIL_RUNTIME=${PENCIL_RUNTIME} install

stencil.pencil: stencil.pencil.c stencil.pencil.h
	cpp -P $< -o $@

.PHONY: run.sh
run.sh:
	echo "#!/bin/bash" > $@
	echo "set -e" >> $@
	echo "set -o pipefail" >> $@
	echo "./Stencil2D --num-iters 100 --passes 1 --pencil --size  ${SIZE} | grep TOTAL_TIME | sed -e '1 s/^/0.0/' -e 's/TOTAL_TIME: /+/' | tr -d '\\\n' | sed -e 's/$$/\\\n/' | bc" >> $@
	chmod a+x $@

.PHONY: oirg/run.sh
orig/run.sh:
	echo "#!/bin/bash" > $@
	echo "set -e" >> $@
	echo "set -o pipefail" >> $@
	echo "./Stencil2D --num-iters 100 --passes 1 --lsize \"8,8\" --size  ${SIZE} | grep TOTAL_TIME | sed -e '1 s/^/0.0/' -e 's/TOTAL_TIME: /+/' | tr -d '\\\n' | sed -e 's/$$/\\\n/' | bc" >> $@
	chmod a+x $@
