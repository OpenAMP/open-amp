# Make file to create ipc stack library.

# Include commons make file to get platform and tool chain specific variables.
include Makefile.commons

LIB := libs/open_amp/libopen_amp.a

INCLUDES := -I"include" -I"include/openamp/porting/system/$(SYSTEM)/machine/$(MACHINE)"
INCLUDES += -I"libs/system/$(MACHINE)/$(SYSTEM)"
CFLAGS += $(INCLUDES)

C_SRCFILES += \
$(wildcard remoteproc/*.c) \
$(wildcard virtio/*.c) \
$(wildcard rpmsg/*.c) \
$(wildcard common/hil/*.c) \
$(wildcard common/llist/*.c) \
$(wildcard common/shm/*.c) \
$(wildcard common/firmware/*.c) \
$(wildcard proxy/*.c) \
$(wildcard porting/system/$(SYSTEM)/*.c) \
$(wildcard porting/system/$(SYSTEM)/machine/$(MACHINE)/*.c) \
$(wildcard porting/machine/$(MACHINE)/*.c)

AS_SRCFILES += \
$(wildcard porting/system/$(SYSTEM)/machine/$(MACHINE)/*.S)

OBJFILES := $(patsubst %.c, %.o, $(C_SRCFILES)) $(patsubst %.S, %.o, $(AS_SRCFILES))

DEPFILES := $(patsubst %.c, %.d, $(C_SRCFILES)) $(patsubst %.S, %.d, $(AS_SRCFILES))

all: $(LIB)

$(LIB): $(OBJFILES)
	@echo AR $@
	$(AR) -r $@ $(OBJFILES)

%.o:%.c $(HEADERS)
	@echo CC $(<:.c=.o)
	$(CC) $(CFLAGS) $(ARCH_CFLAGS) $(INCLUDE) -c $< -o $@

%.o:%.S
	@echo AS $(<:.S=.o)
	$(AS) $(ARCH_ASFLAGS) $(INCLUDE) $< -o $@

clean:
	-$(RM) $(LIB) $(OBJFILES) $(DEPFILES)

PHONY: all clean
