ifeq ($(SYSTEM),)
SYSTEM := generic
endif

ifeq ($(MACHINE),)
MACHINE := zynq7
endif

OHOME := $(CURDIR)
ifeq ($(BUILDROOT),)
BUILDROOT := $(CURDIR)/.build
endif

export SYSTEM MACHINE OHOME BUILDROOT

.PHONY: all lib obsolete apps clean clean_lib clean_obsolete clean_apps

all: lib

clean:
	rm -rf $(BUILDROOT)

ifeq ($(WITH_OBSOLETE),y)
all: obsolete
endif
ifeq ($(WITH_APPS),y)
all: apps
endif

lib:
	make -C lib all

obsolete:
	make -C obsolete

apps:
	make -C apps all

clean_lib:
	make -C lib clean

clean_obsolete:
	make -C obsolete clean

clean_apps:
	make -C apps clean
