
# Include commons make file to get platform and tool chain specific variables.

export SRCROOT := $(CURDIR)
export OHOME := $(SRCROOT)/../
export BUILD := $(BUILDROOT)/apps

all:
	make -C matrix_multiply all
	make -C echo_test all
	make  -C rpc_demo all

clean:
	rm -rf $(BUILD)


