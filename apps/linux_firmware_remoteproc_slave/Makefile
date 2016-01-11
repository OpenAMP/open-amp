
include $(SRCROOT)/Makefile.common

PBUILD := $(BUILD)
BUILD := $(PBUILD)/$(shell basename $(CURDIR))
BUILDSHARE := $(PBUILD)/share

BOOTSTRAP_OUT := $(BUILDSHARE)/lnx_firmware

BOOTSTRAP_SRCS := boot.c rsc_table.c

ifneq ($(MACHINE),zynq7)
$(warning "Machine $(MACHINE) is not supported by this app $(shell basename $(CURDIR))")
all:

clean:
else

ifeq ($(wildcard $(BUILDROOT)/libs/image.ub),)
all:

clean:
else

BOOTSTRAP_AS_SRCS := boot_wrapper_$(MACHINE).S

BOOTSTRAP_OBJS := $(patsubst %.c,$(BUILD)/%.o,$(BOOTSTRAP_SRCS)) $(patsubst %.S,$(BUILD)/%.o,$(BOOTSTRAP_AS_SRCS))

LINKERSCRIPT := link_remote.ld

ifeq ($(EXTRA_INCLUDES),)
EXTRA_INCLUDES += -I"$(OHOME)/obsolete/system/$(SYSTEM)/machine/$(MACHINE)/linux_firmware"
endif

CFLAGS += $(EXTRA_INCLUDES)

all: $(BOOTSTRAP_OUT)
	cp $(BOOTSTRAP_OUT) $(BUILD)/firmware1
	cd $(BUILD) && $(CROSS)objcopy -I binary -O elf32-littlearm -B arm firmware1 $(BUILDSHARE)/firmware1.o
	cp $(BOOTSTRAP_OUT) $(BUILD)/firmware2
	cd $(BUILD) && $(CROSS)objcopy -I binary -O elf32-littlearm -B arm firmware2 $(BUILDSHARE)/firmware2.o

$(BOOTSTRAP_OUT): $(BUILDSHARE) $(BOOTSTRAP_OBJS)

	@echo 'Building linux aggregate image : $@'

	$(LD)  -nostartfiles -T"$(LINKERSCRIPT)" $(LDFLAGS) -o "$(BOOTSTRAP_OUT)" -Wl,--start-group $(BOOTSTRAP_OBJS) -lc -lm -lstdc++ -lzlib -lfdt -Wl,--end-group

	@echo 'Finished building target: $@'

$(BUILD)/%.o: %.c
	mkdir -p $(dir $@)
	@echo CC $(<:.c=.o)
	$(CC) $(CFLAGS) $(ARCH_CFLAGS) $(INCLUDE) -c $< -o $@

$(BUILD)/%.o: $(CURDIR)/%.S $(BUILD)/image.ub
	mkdir -p $(dir $@)
	@echo AS $(<:.S=.o)
	cd $(BUILD); \
	$(AS) $(ASFLAGS) $(INCLUDE) $< -o $@

$(BUILD) $(BUILDSHARE):
	mkdir -p $@

$(BUILD)/image.ub:
	cp $(BUILDROOT)/libs/image.ub $@
clean:
	rm -rf $(BUILD) $(BOOTSTRAP_OUT)

PHONY: all clean

endif
endif
