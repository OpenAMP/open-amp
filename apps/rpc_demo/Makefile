
include $(SRCROOT)/Makefile.common

PBUILD := $(BUILD)
BUILD := $(PBUILD)/$(shell basename $(CURDIR))

EXES := $(BUILD)/rpc_demo.out

RPC_DEMO_SRCS := rpc_demo.c

RPC_DEMO_OBJS := $(patsubst %.c,$(BUILD)/%.o,$(RPC_DEMO_SRCS))

C_COMMON_OBJS := $(foreach f,$(C_COMMON_SRCS),$(BUILD)/$(patsubst %c,%o,$(shell basename $(f))))

# Create obj files dependencies
$(foreach f,$(RPC_DEMO_SRCS) $(C_COMMON_SRCS),$(eval $(patsubst %.c,%.o,$(BUILD)/$(shell basename $(f)))_dep := $(f)))

RPC_DEMO_MAP := $(patsubst %.out,%.map, $(EXES))

LIBS := -lopen_amp $(DEPS)

.PHONY: all clean

all: $(EXES)

$(BUILD)/rpc_demo.out: $(RPC_DEMO_OBJS) $(C_COMMON_OBJS)

	@echo 'Building echo test for baremetal : $@'

	$(LD)  -Wl,-Map=$(RPC_DEMO_MAP) -Wl,--gc-sections -T"$(LINKERSCRIPT)" $(LDFLAGS) -o "$@"  $^ -Wl,--start-group $(LIBS) -Wl,--end-group

clean:
	rm -rf $(BUILD)

.SECONDEXPANSION:
%.o: $$($$@_dep)
	mkdir -p $(dir $@);
	$(CC) $(CFLAGS) $(ARCH_CFLAGS) $(INCLUDE) -c $< -o $@

# This is to avoid the warning of non-existing Makefile
%/Makefile.include:

