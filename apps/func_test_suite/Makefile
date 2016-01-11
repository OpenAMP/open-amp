
include $(SRCROOT)/Makefile.common

PBUILD := $(BUILD)
BUILD := $(PBUILD)/$(shell basename $(CURDIR))

EXES := $(BUILD)/func_test_suite.out

ifeq ($(REMOTEPROC_MASTER),y)
FUNC_TEST_SUITE_SRCS := func_test_suite_remoteproc_master.c
EXES := $(BUILD)/func_test_suite.bin
BUILDSHARE := $(PBUILD)/share
else
FUNC_TEST_SUITE_SRCS := func_test_suite.c
endif

FUNC_TEST_SUITE_OBJS := $(patsubst %.c,$(BUILD)/%.o,$(FUNC_TEST_SUITE_SRCS))

C_COMMON_OBJS := $(foreach f,$(C_COMMON_SRCS),$(BUILD)/$(patsubst %c,%o,$(shell basename $(f))))

# Create obj files dependencies
$(foreach f,$(FUNC_TEST_SUITE_SRCS) $(C_COMMON_SRCS),$(eval $(patsubst %.c,%.o,$(BUILD)/$(shell basename $(f)))_dep := $(f)))

FUNC_TEST_SUITE_MAP := $(patsubst %.out,%.map, $(EXES))

LIBS := -lopen_amp $(DEPS)

.PHONY: all clean

all: $(EXES)

ifeq ($(REMOTEPROC_MASTER),y)
$(BUILD)/func_test_suite.out: $(FUNC_TEST_SUITE_OBJS) $(C_COMMON_OBJS)
	@echo 'Building function test stuie for baremetal : $@'
	$(LD)  -Wl,-Map=$(FUNC_TEST_SUITE_MAP) -Wl,--gc-sections -T"$(LINKERSCRIPT)" $(LDFLAGS) -o "$@" $^ -Wl,--start-group $(BUILDSHARE)/firmware1.o $(BUILDSHARE)/firmware2.o -lstdc++ $(LIBS) -Wl,--end-group

$(BUILD)/func_test_suite.bin: $(BUILD)/func_test_suite.out
	$(CROSS)objcopy -O binary $(BUILD)/func_test_suite.out $(BUILD)/func_test_suite.bin

else
$(BUILD)/func_test_suite.out: $(FUNC_TEST_SUITE_OBJS) $(C_COMMON_OBJS)

	@echo 'Building function test suite for baremetal : $@'

	$(LD)  -Wl,-Map=$(FUNC_TEST_SUITE_MAP) -Wl,--gc-sections -T"$(LINKERSCRIPT)" $(LDFLAGS) -o "$@"  $^ -Wl,--start-group $(LIBS) -Wl,--end-group
endif

clean:
	rm -rf $(BUILD)

.SECONDEXPANSION:
%.o: $$($$@_dep)
	mkdir -p $(dir $@);
	$(CC) $(CFLAGS) $(ARCH_CFLAGS) $(INCLUDE) -c $< -o $@

# This is to avoid the warning of non-existing Makefile
%/Makefile.include:

