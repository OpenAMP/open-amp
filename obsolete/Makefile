BUILD := $(BUILDROOT)/obsolete
BUILDLIB := $(BUILDROOT)/libs
export BUILDROOT OHOME BUILDLIB

all:
	if [ -f "system/$(SYSTEM)/machine/$(MACHINE)/Makefile" ]; then \
		make -C system/$(SYSTEM)/machine/$(MACHINE) all || exit 255; \
	fi; \
	if [ -f "system/$(SYSTEM)/Makefile" ]; then \
		make -C system/$(SYSTEM) all || exit 255; \
	fi; \

clean:
	rm -rf $(BUILDROOT)
