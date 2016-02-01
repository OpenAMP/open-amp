BUILD := $(BUILDROOT)/$(subst $(OHOME)/,,$(CURDIR))
BUILDLIBS := $(BUILDROOT)/libs

CROSS            := arm-xilinx-eabi-
CFLAGS           := -Wall -ffunction-sections -fdata-sections -O0 -g3 -MMD
ARCH_CFLAGS      := -gdwarf-2 -mthumb-interwork -mcpu=cortex-a9
CC               = $(CROSS)gcc
CXX              = $(CROSS)g++
AS               = $(CROSS)as
AR               = $(CROSS)ar
LD               = $(CROSS)gcc

LIB := $(BUILDLIBS)/libbaremetal.a

C_SRCFILES += \
$(wildcard *.c)

OBJFILES := $(patsubst %.c, $(BUILD)/%.o, $(C_SRCFILES))

DEPFILES := $(patsubst %.c, $(BUILD)/%.d, $(C_SRCFILES))

all: $(LIB)

$(LIB): $(OBJFILES)

	@echo AR $@
	$(AR) -r $@ $(OBJFILES)

$(BUILD)/%.o:%.c $(BUILD)
	@echo CC $(<:.c=.o)
	$(CC) $(CFLAGS) $(ARCH_CFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $@

clean:
	@rm -rf $(LIB)
	@rm -rf $(BUILD)

PHONY: all clean
