# Make file to create rpmsg library.

# Include commons make file to get platform and tool chain specific variables.
include ../Makefile.commons

LIB := librpmsg.a

# Include  the header and source files required by the virtio lib.
HEADERS += \
$(wildcard *.h) \
$(wildcard ../include/*.h) \
$(wildcard ../hil/*.h) \
$(wildcard ../env/*.h)

SRCFILES += \
$(wildcard *.c) \
$(wildcard ../hil/*/*.c) \
$(wildcard ../common/*.c) \
$(wildcard ../env/*.c)

OBJFILES := $(patsubst %.c, %.o, $(SRCFILES))

all: $(LIB)

$(LIB): $(OBJFILES)
	@echo AR $@
	@$(AR) -r $@ $(OBJFILES)

%.o:%.c $(HEADERS)
	@echo CC $(<:.c=.o)
	@$(CC) $(CFLAGS) $(ARCH_CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	-$(RM) $(LIB) $(OBJFILES)

PHONY: all clean