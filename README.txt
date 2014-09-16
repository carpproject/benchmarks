CARP project validation cases to demonstrate PENCIL.

Requirements
------------
 * PENCIL toolchain: http://github.com/carpproject/pencil
 * PENCIL utils: http://github.com/carpproject/pencil-utils
 * PPCG toolchain: http://repo.or.cz/w/ppcg.git
 * SHOC benchmark suite: http://github.com/vetter/shoc
 * Rodinia 3.0 benchmark suite: https://www.cs.virginia.edu/~skadron/wiki/rodinia/index.php/Main_Page

Installation
------------
 * VALIDATION_CASES_DIR - directory where validation_cases repository is cloned
 * PENCIL_TOOLS_HOME - directory where PENCIL toolchain is cloned.
 * PENCIL_UTILS_HOME - directory where PENCIL utils repository is cloned.
 * PPCG_PATH - directory where PPCG is cloned
 * SHOC_DIR - directory where SHOC is cloned
 * RODINIA_DIR - directory where Rodinia is cloned

1. Clone and build PENCIL toolchain using build instructions from README.txt
   file in PENCIL repository.

1.1 Build the PENCIL runtime for your architecture:
   % make -C ${PENCIL_PATH}/etc/runtime OCL_UTIL=${PPCG_PATH}/ocl_utilities.c ARCH=${YOUR_ARCH}

2. Clone SHOC and checkout the required version
   % git clone git@github.com:vetter/shoc.git ${SHOC_DIR}
   % git checkout 4d476b46f7632752dd491238cd99158bc58ba3fb

3. Apply patches for SHOC and run configure
   % cd ${SHOC_DIR}
   % git apply ${VALIDATION_CASES_DIR}/patches/shoc/*.patch
   % ./configure

4. Apply patches for Rodinia
   % cd ${RODINIA_DIR}
   % patch -p1 < ${VALIDATION_CASES_DIR}/patches/rodinia/bfs.patch
   % patch -p1 < ${VALIDATION_CASES_DIR}/patches/rodinia/srad.patch
   % patch -p1 < ${VALIDATION_CASES_DIR}/patches/rodinia/gaussian.patch

5. Apply patch to PPCG to enable PENCIL runtime and build it
   % cd ${PPCG_PATH}
   % git checkout ppcg-0.03
   % patch -p1 < ${VALIDATION_CASES_DIR}/patches/ppcg/0001-PENCIL-runtime-support.patch
   % ./configure && make

6. Create configurations file for the validation cases
   % cd ${VALIDATION_CASES_DIR}
   % cp config.orig config
   % vim config

   The following variables should be specified:
   ARCH - target architecture (ARCH=arm-none-linux-gnueabi-)
   PENCIL_TOOLS_HOME - path to PENCIL repository clone (PENCIL_TOOLS_HOME=/path/to/pencil/clone)
   PENCIL_UTILS_HOME - path to PENCIL runtime (PENCIL_UTILS_HOME=/path/tp/pencil/utils)
   PPCG_PATH - path to PPCG repository clone (PPCG_PATH=/path/to/ppcg/clone)
   OPENCL_SDK - optional path to OpenCL SDK
   BOARD - address of the target board (BOARD=user@12.34.56.78)
   BOARD_DIR - directory on board used for execution (BOARD_DIR=/home/user/run)
   BOARD_DATA_DIR - directory on board used to store benchmark data (BOARD_DATA_DIR=/home/user/data). Should be different from BOARD_DIR.
   BOARD_LIB_DIR - directory on board used to install libraries (BOARD_LIB_DIR=/home/user/lib). Should be different from BOARD_DIR.
   SHOC_DIR - directory where SHOC is cloned (SHOC_DIR=/path/to/shoc/clone)
   RODINIA_DIR - directory where Rodinia is cloned (SHOC_DIR=/path/to/rodinia/clone)
   EXECUTION_TIME_LIMIT - execution time limit for benchmark in seconds (EXECUTION_TIME_LIMIT=180)

Running the validation cases:

The validation cases are designed to be used with autotuner program to
determine the best set of PPCG options for the given application.
Building with a single set of selected options is also possible.

Benchmarks will be run on the board specified by the $BOARD variable via ssh.
Running the benchmarks locally is not explicitly supported but can be done by
setting BOARD=localhost.

To run the benchmark use the following commands in the benchmark directory
(${VALIDATION_CASES_DIR}/src/spmv for example):

1. Running single configuration:
    % make ppcg AUTOTUNER_PPCG_FLAGS="${PPCG_OPTIONS}"
    % make build
    % make prepare #only once per benchmark to copy required data on board
    % make run

2. Running the original version of the benchmark
    % make prepare
    % make run-orig

3. Running the exploration. Validation cases repository contains simple script
   to explore the PPCG options. It can be launched in the benchmark directory:
   % python ../explore.py

   The set of params to explore is specified in the explore-params.py file.
