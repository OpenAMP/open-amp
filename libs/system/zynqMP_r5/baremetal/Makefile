# Make file to create baremetal lib.


all: 
	make  -f make_xil_standalone_lib
	make  -f make_remote
	make  -f make_remote_rpc
clean:
	make  -f make_xil_standalone_lib
	make  -f make_remote clean
	make  -f make_remote_rpc clean
	rm -rf .build

PHONY: all clean
