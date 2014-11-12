AR=$(ARCH)ar
CC=$(ARCH)gcc
CXX=$(ARCH)g++
FC=$(ARCH)gcc
LD=$(ARCH)ld
ifdef ARCH
	CFLAGS=-Wall -std=c99 -I$(OPENCL_SDK)/include -g -O2 -mfloat-abi=hard
	CXXFLAGS=-Wall -I$(OPENCL_SDK)/include -g -O2 -mfloat-abi=hard
else
	CFLAGS=-Wall -std=c99 -I$(OPENCL_SDK)/include -g -O2
	CXXFLAGS=-Wall -I$(OPENCL_SDK)/include -g -O2
endif
LDFLAGS=-L$(OPENCL_SDK)/lib -lOpenCL
PPCG=$(PPCG_PATH)/ppcg

PENCIL_RUNTIME=${PENCIL_UTIL_HOME}/runtime
PENCIL_INCLUDE=${PENCIL_UTIL_HOME}/include
