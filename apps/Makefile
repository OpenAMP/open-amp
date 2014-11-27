
# Include commons make file to get platform and tool chain specific variables.


include ../Makefile.commons

all:			
ifeq ($(OS),baremetal) 
ifeq ($(ROLE),remote)
	make  -f samples/remote/baremetal/matrix_multiply/make
	make  -f tests/remote/baremetal/echo_test/make
	make  -f tests/remote/baremetal/func_test_suite/make
	make  -f samples/remote/baremetal/rpc_demo/make
endif
endif

linux_remote:
	make  -f samples/master/baremetal/matrix_multiply/make
	make  -f tests/master/baremetal/echo_test/make
	make  -f tests/master/baremetal/func_test_suite/make


clean:		
ifeq ($(OS),baremetal)
ifeq ($(ROLE),remote)
	make  -f samples/remote/baremetal/matrix_multiply/make clean
	make  -f tests/remote/baremetal/echo_test/make clean
	make  -f tests/remote/baremetal/func_test_suite/make clean
	make  -f samples/remote/baremetal/rpc_demo/make clean
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
