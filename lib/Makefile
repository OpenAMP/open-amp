# Make file to create ipc stack library.

# Include commons make file to get platform and tool chain specific variables.
BUILD := $(BUILDROOT)/lib
BUILDLIB := $(BUILDROOT)/libs
SRCROOT := $(CURDIR)

ifeq ($(MACHINE),)
export MACHINE := zynq7
endif

ifeq ($(SYSTEM),)
export SYSTEM := generic
endif

ifeq ($(OHOME),)
export OHOME := $(CURDIR)
endif

INCLUDES :=

-include system/$(SYSTEM)/machine/$(MACHINE)/Makefile.platform
-include system/$(SYSTEM)/Makefile.include

ifeq ($(ROLE),master)
CFLAGS+=-D"MASTER=1"
else
CFLAGS+=-D"MASTER=0"
endif

ifeq ($(BENCHMARK),1)
CFLAGS+=-D"OPENAMP_BENCHMARK_ENABLE"
endif

ifeq ($(LINUXREMOTE),1)
CFLAGS+=-D"OPENAMP_REMOTE_LINUX_ENABLE"
endif

LIB := $(BUILDLIB)/libopen_amp.a

INCLUDES += -I"include"
ifneq ("$(wildcard include/openamp/system/$(SYSTEM))","")
INCLUDES += -I"include/openamp/system/$(SYSTEM)"
endif
ifneq ("$(wildcard include/openamp/system/$(SYSTEM)/machine/$(MACHINE))","")
INCLUDES += -I"include/openamp/system/$(SYSTEM)/machine/$(MACHINE)"
endif
CFLAGS += $(INCLUDES)

C_SRCFILES += \
$(wildcard remoteproc/*.c) \
$(wildcard virtio/*.c) \
$(wildcard rpmsg/*.c) \
$(wildcard common/*.c) \
$(wildcard proxy/*.c) \
$(wildcard system/$(SYSTEM)/*.c) \
$(wildcard system/$(SYSTEM)/machine/$(MACHINE)/*.c) \
$(wildcard machine/$(MACHINE)/*.c)

AS_SRCFILES += \
$(wildcard system/$(SYSTEM)/machine/$(MACHINE)/*.S)

OBJFILES := $(patsubst %.c, $(BUILD)/%.o, $(C_SRCFILES)) $(patsubst %.S, $(BUILD)/%.o, $(AS_SRCFILES))

DEPFILES := $(patsubst %.c, $(BUILD)/%.d, $(C_SRCFILES)) $(patsubst %.S, $(BUILD)/%.d, $(AS_SRCFILES))

all: $(LIB)

$(LIB): $(BUILDLIB) $(OBJFILES)
	@echo AR $@
	$(AR) -r $@ $(OBJFILES)

$(BUILD)/%.o:%.c
	@mkdir -p $(dir $@)
	@echo CC $(<:.c=.o)
	$(CC) $(CFLAGS) $(ARCH_CFLAGS) -c $< -o $@

$(BUILD)/%.o:%.S
	@echo AS $(<:.S=.o)
	$(AS) $(ARCH_ASFLAGS) $< -o $@

$(BUILD) $(BUILDLIB):
	mkdir -p $@

clean:
	rm -rf $(BUILD)

PHONY: all clean
