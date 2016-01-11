
include $(SRCROOT)/Makefile.common

PBUILD := $(BUILD)
BUILD := $(PBUILD)/$(shell basename $(CURDIR))

EXES := $(BUILD)/echo_testd.out

ifeq ($(REMOTEPROC_MASTER),y)
ECHO_TEST_D_SRCS := echo_testd_remoteproc_master.c
EXES := $(BUILD)/echo_testd.bin
BUILDSHARE := $(PBUILD)/share
else
ECHO_TEST_D_SRCS := echo_testd.c
endif

ECHO_TEST_D_OBJS := $(patsubst %.c,$(BUILD)/%.o,$(ECHO_TEST_D_SRCS))

C_COMMON_OBJS := $(foreach f,$(C_COMMON_SRCS),$(BUILD)/$(patsubst %c,%o,$(shell basename $(f))))

# Create obj files dependencies
$(foreach f,$(ECHO_TEST_D_SRCS) $(C_COMMON_SRCS),$(eval $(patsubst %.c,%.o,$(BUILD)/$(shell basename $(f)))_dep := $(f)))

ECHO_TEST_D_MAP := $(patsubst %.out,%.map,$(BUILD)/echo_testd.out)

LIBS := -lopen_amp $(DEPS)

.PHONY: all clean

all: $(EXES)

ifeq ($(REMOTEPROC_MASTER),y)
$(BUILD)/echo_testd.out: $(ECHO_TEST_D_OBJS) $(C_COMMON_OBJS)
	@echo 'Building echo test for baremetal : $@'
	$(LD)  -Wl,-Map=$(ECHO_TEST_D_MAP) -Wl,--gc-sections -T"$(LINKERSCRIPT)" $(LDFLAGS) -o "$@" $^ -Wl,--start-group $(BUILDSHARE)/firmware1.o $(BUILDSHARE)/firmware2.o -lstdc++ $(LIBS) -Wl,--end-group

$(BUILD)/echo_testd.bin: $(BUILD)/echo_testd.out
	$(CROSS)objcopy -O binary $(BUILD)/echo_testd.out $(BUILD)/echo_testd.bin

else
$(BUILD)/echo_testd.out: $(ECHO_TEST_D_OBJS) $(C_COMMON_OBJS)
	@echo 'Building echo test for baremetal : $@'
	$(LD)  -Wl,-Map=$(ECHO_TEST_D_MAP) -Wl,--gc-sections -T"$(LINKERSCRIPT)" $(LDFLAGS) -o "$@"  $^ -Wl,--start-group $(LIBS) -Wl,--end-group
endif

clean:
	rm -rf $(BUILD)

.SECONDEXPANSION:
%.o: $$($$@_dep)
	mkdir -p $(dir $@);
	$(CC) $(CFLAGS) $(ARCH_CFLAGS) $(INCLUDE) -c $< -o $@

# This is to avoid the warning of non-existing Makefile
%/Makefile.include:

