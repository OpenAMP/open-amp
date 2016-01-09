# Make file to create baremetal lib.

BUILD := $(BUILDROOT)/$(subst $(OHOME)/,,$(CURDIR))

export BUILD

all:
	make  -f make_xil_standalone_lib
clean:
	make  -f make_xil_standalone_lib clean

PHONY: all clean
