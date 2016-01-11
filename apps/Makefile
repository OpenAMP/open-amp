
# Include commons make file to get platform and tool chain specific variables.

export SRCROOT := $(CURDIR)
export OHOME := $(SRCROOT)/../
export BUILD := $(BUILDROOT)/apps

all:
ifeq ($(REMOTEPROC_MASTER),y)
	make -C linux_firmware_remoteproc_slave all
endif
	make -C matrix_multiply all
	make -C echo_test all
ifneq ($(REMOTEPROC_MASTER),y)
	make -C rpc_demo all
endif

clean:
	rm -rf $(BUILD)


