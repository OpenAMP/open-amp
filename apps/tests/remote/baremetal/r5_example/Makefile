
include Makefile.commons

LDFLAGS += -L"$(OHOME)/libs/open_amp" -L"$(OHOME)/libs/system/$(PLAT)/baremetal"

SAMPLE_OUT := echo_test.out
SAMPLE_SRC := echo_test.c
SAMPLE_SRC += rsc_table_zynqMP_r5.c

SAMPLEOBJFILES := $(patsubst %.c, %.o, $(SAMPLE_SRC))

SAMPLE_DEPS   := $(patsubst %.c, %.d, $(SAMPLE_SRC))

SAMPLE_MAP := $(patsubst %.out, %.map, $(SAMPLE_OUT))

all: $(SAMPLE_OUT)

$(SAMPLE_OUT): $(SAMPLEOBJFILES) 

	@echo 'Building echo test for baremetal : $@'

	$(LD) -v -Wl,-Map=echo_test.map -Wl,--gc-sections -T"$(OHOME)/libs/system/$(PLAT)/baremetal/linker_remote.ld" $(LDFLAGS) -o "$(SAMPLE_OUT)" -Wl,--start-group $(SAMPLEOBJFILES) -lxil -lbaremetal_remote -lopen_amp -lgcc -lc -lm -Wl,--end-group
	
	@echo 'Finished building target: $@'
	
%.o:%.c
	@echo CC $(<:.c=.o)
	@$(CC) $(CFLAGS) $(ARCH_CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	-$(RM) $(SAMPLEOBJFILES) $(SAMPLE_DEPS) $(SAMPLE_OUT) $(SAMPLE_MAP)
	
post-build:
	-@echo ' '

secondary-outputs:

PHONY: all clean dependents
.SECONDARY: post-build
