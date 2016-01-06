
# Include commons make file to get platform and tool chain specific variables.


include ../Makefile.commons

all:
ifeq ($(SYSTEM),baremetal)
ifeq ($(ROLE),remote)
	make  -C samples/remote/baremetal/matrix_multiply -f make
	make  -C tests/remote/baremetal/echo_test -f make
	#make  -f tests/remote/baremetal/func_test_suite/make
	make  -C samples/remote/baremetal/rpc_demo -f make
endif
endif

linux_remote:
	make  -f samples/master/baremetal/matrix_multiply/make
	make  -f tests/master/baremetal/echo_test/make
	make  -f tests/master/baremetal/func_test_suite/make

clean:
ifeq ($(SYSTEM),baremetal)
ifeq ($(ROLE),remote)
	make  -C samples/remote/baremetal/matrix_multiply -f make clean
	make  -C tests/remote/baremetal/echo_test -f make clean
	#make  -f tests/remote/baremetal/func_test_suite/make clean
	make  -C samples/remote/baremetal/rpc_demo -f make clean
endif
endif

clean_linux_remote:
	make  -f samples/master/baremetal/matrix_multiply/make clean
	make  -f tests/master/baremetal/echo_test/make clean
	make  -f tests/master/baremetal/func_test_suite/make clean

post-build:
	-@echo ' '

secondary-outputs:

PHONY: all clean dependents
.SECONDARY: post-build
