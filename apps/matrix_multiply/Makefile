
include $(SRCROOT)/Makefile.common

PBUILD := $(BUILD)
BUILD := $(PBUILD)/$(shell basename $(CURDIR))

EXES := $(BUILD)/matrix_multiplyd.out

ifeq ($(REMOTEPROC_MASTER),y)
MATRIX_MUL_D_SRCS := matrix_multiplyd_remoteproc_master.c
else
MATRIX_MUL_D_SRCS := matrix_multiplyd.c
endif

MATRIX_MUL_D_OBJS := $(patsubst %.c,$(BUILD)/%.o,$(MATRIX_MUL_D_SRCS))

C_COMMON_OBJS := $(foreach f,$(C_COMMON_SRCS),$(BUILD)/$(patsubst %c,%o,$(shell basename $(f))))

# Create obj files dependencies
$(foreach f,$(MATRIX_MUL_D_SRCS) $(C_COMMON_SRCS),$(eval $(patsubst %.c,%.o,$(BUILD)/$(shell basename $(f)))_dep := $(f)))

MATRIX_MUL_D_MAP := $(patsubst %.out,%.map, $(EXES))

LIBS := -lopen_amp $(DEPS)

.PHONY: all clean

all: $(EXES)

$(BUILD)/matrix_multiplyd.out: $(MATRIX_MUL_D_OBJS) $(C_COMMON_OBJS)

	@echo 'Building echo test for baremetal : $@'

	$(LD)  -Wl,-Map=$(MATRIX_MUL_D_MAP) -Wl,--gc-sections -T"$(LINKERSCRIPT)" $(LDFLAGS) -o "$@"  $^ -Wl,--start-group $(LIBS) -Wl,--end-group

clean:
	rm -rf $(BUILD)

.SECONDEXPANSION:
%.o: $$($$@_dep)
	mkdir -p $(dir $@);
	$(CC) $(CFLAGS) $(ARCH_CFLAGS) $(INCLUDE) -c $< -o $@

# This is to avoid the warning of non-existing Makefile
%/Makefile.include:

