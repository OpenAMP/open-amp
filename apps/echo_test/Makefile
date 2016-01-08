
include $(SRCROOT)/Makefile.common

PBUILD := $(BUILD)
BUILD := $(PBUILD)/$(shell basename $(CURDIR))

EXES := $(BUILD)/echo_testd.out

ifeq ($(REMOTEPROC_MASTER),y)
ECHO_TEST_D_SRCS := echo_testd_remoteproc_master.c
else
ECHO_TEST_D_SRCS := echo_testd.c
endif

ECHO_TEST_D_OBJS := $(patsubst %.c,$(BUILD)/%.o,$(ECHO_TEST_D_SRCS))

C_COMMON_OBJS := $(foreach f,$(C_COMMON_SRCS),$(BUILD)/$(patsubst %c,%o,$(shell basename $(f))))

# Create obj files dependencies
$(foreach f,$(ECHO_TEST_D_SRCS) $(C_COMMON_SRCS),$(eval $(patsubst %.c,%.o,$(BUILD)/$(shell basename $(f)))_dep := $(f)))

ECHO_TEST_D_MAP := $(patsubst %.out,%.map, $(EXES))

LIBS := -lopen_amp $(DEPS)

.PHONY: all clean

all: $(EXES)

$(BUILD)/echo_testd.out: $(ECHO_TEST_D_OBJS) $(C_COMMON_OBJS)

	@echo 'Building echo test for baremetal : $@'

	$(LD)  -Wl,-Map=$(ECHO_TEST_D_MAP) -Wl,--gc-sections -T"$(LINKERSCRIPT)" $(LDFLAGS) -o "$@"  $^ -Wl,--start-group $(LIBS) -Wl,--end-group

clean:
	rm -rf $(BUILD)

.SECONDEXPANSION:
%.o: $$($$@_dep)
	mkdir -p $(dir $@);
	$(CC) $(CFLAGS) $(ARCH_CFLAGS) $(INCLUDE) -c $< -o $@

# This is to avoid the warning of non-existing Makefile
%/Makefile.include:

