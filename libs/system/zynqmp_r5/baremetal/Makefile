# Make file to create baremetal lib.


all: 
	make  -f make_xil_standalone_lib
	make  -f make_remote
clean:
	make  -f make_xil_standalone_lib clean
	make  -f make_remote clean
	rm -rf .build

PHONY: all clean
