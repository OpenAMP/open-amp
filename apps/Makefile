
# Include commons make file to get platform and tool chain specific variables.


include ../Makefile.commons

all:
ifeq ($(OS),nucleus)
ifeq ($(ROLE),master)
	make  -f samples/master/nucleus/matrix_multiply/nucleus_nucleusbm/make
	make  -f tests/master/nucleus/echo_test/nucleus_nucleusbm/make
	make  -f tests/master/nucleus/func_test_suite/nucleus_nucleusbm/make
else
	make  -f samples/remote/nucleus/matrix_multiply/make
	make  -f tests/remote/nucleus/echo_test/make
	make  -f tests/remote/nucleus/func_test_suite/make
	make  -f samples/remote/nucleus/rpc_demo/make
endif
endif			
ifeq ($(OS),baremetal) 
ifeq ($(ROLE),remote)
	make  -f samples/remote/baremetal/matrix_multiply/make
	make  -f tests/remote/baremetal/echo_test/make
	make  -f tests/remote/baremetal/func_test_suite/make
	make  -f samples/remote/baremetal/rpc_demo/make
endif
endif

benchmark:
	make  -f tests/master/nucleus/latency_test/nucleus_nucleusbm/make

linux_remote:
ifeq ($(OS),nucleus)
	make  -f samples/master/nucleus/matrix_multiply/nucleus_linux/make
	make  -f tests/master/nucleus/echo_test/nucleus_linux/make
	make  -f tests/master/nucleus/func_test_suite/nucleus_linux/make
else
	make  -f samples/master/baremetal/matrix_multiply/make
	make  -f tests/master/baremetal/echo_test/make
	make  -f tests/master/baremetal/func_test_suite/make
endif

linux_remote_benchmark:
ifeq ($(OS),nucleus)
	make  -f tests/master/nucleus/latency_test/nucleus_linux/make
endif

clean:
ifeq ($(OS),nucleus)
ifeq ($(ROLE),master)
	make  -f samples/master/nucleus/matrix_multiply/nucleus_nucleusbm/make clean
	make  -f tests/master/nucleus/echo_test/nucleus_nucleusbm/make clean
	make  -f tests/master/nucleus/func_test_suite/nucleus_nucleusbm/make clean
else
	make  -f samples/remote/nucleus/matrix_multiply/make clean
	make  -f tests/remote/nucleus/echo_test/make clean
	make  -f tests/remote/nucleus/func_test_suite/make clean
	make  -f samples/remote/nucleus/rpc_demo/make clean
endif
endif			
ifeq ($(OS),baremetal)
ifeq ($(ROLE),remote)
	make  -f samples/remote/baremetal/matrix_multiply/make clean
	make  -f tests/remote/baremetal/echo_test/make clean
	make  -f tests/remote/baremetal/func_test_suite/make clean
	make  -f samples/remote/baremetal/rpc_demo/make clean
endif
endif

cleanbenchmark:
	make  -f tests/master/nucleus/latency_test/nucleus_nucleusbm/make clean

clean_linux_remote:
ifeq ($(OS),nucleus)
	make  -f samples/master/nucleus/matrix_multiply/nucleus_linux/make clean
	make  -f tests/master/nucleus/echo_test/nucleus_linux/make clean
	make  -f tests/master/nucleus/func_test_suite/nucleus_linux/make clean
else
	make  -f samples/master/baremetal/matrix_multiply/make clean
	make  -f tests/master/baremetal/echo_test/make clean
	make  -f tests/master/baremetal/func_test_suite/make clean
endif

clean_linux_remote_benchmark:
ifeq ($(OS),nucleus)
	make  -f tests/master/nucleus/latency_test/nucleus_linux/make clean
endif

post-build:
	-@echo ' '

secondary-outputs:

PHONY: all clean dependents
.SECONDARY: post-build
