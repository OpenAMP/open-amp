include config.make
AS=armr5-none-eabi-as
CC=armr5-none-eabi-gcc
AR=armr5-none-eabi-ar
CP=cp
ARCHIVER=armr5-none-eabi-ar
COMPILER_FLAGS= -O2 -c
EXTRA_COMPILER_FLAGS= -mcpu=cortex-r5 -DUSEAMP=1
LIB=libxil.a

LIB=libxil.a

CC_FLAGS = $(subst -pg, -DPROFILING, $(COMPILER_FLAGS))
ECC_FLAGS = $(subst -pg, -DPROFILING, $(EXTRA_COMPILER_FLAGS))

ifeq ($(COMPILER) , arm-eabi-gcc)
 ECC_FLAGS	=	+= -nostartfiles
endif

#The following flags are required for PEEP. We can remove them later
ECC_FLAGS	+= -mcpu=cortex-r5 \
		  -mfloat-abi=soft \
		  -DUSEAMP=1

#RELEASEDIR=../../../lib
RELEASEDIR=./.
#INCLUDEDIR=../../../include
#INCLUDES=-I./. -I${INCLUDEDIR}
INCLUDES=-I./.

OUTS = *.o

INCLUDEFILES=*.h

libs: $(LIBS)

all: libs

standalone_libs: $(LIBSOURCES)
	@echo "Compiling standalone"
	$(CC) $(CC_FLAGS) $(ECC_FLAGS) $(INCLUDES) $^
	@echo "AR standalone"
	$(ARCHIVER) -r ${RELEASEDIR}/${LIB} ${OUTS}


.PHONY: include
include: standalone_includes

standalone_includes:
	${CP} ${INCLUDEFILES} ${INCLUDEDIR}


clean:
	rm -rf ${OUTS} $(LIBS)
