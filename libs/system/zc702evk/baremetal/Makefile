# Make file to create baremetal lib.


all: 
	make  -f make_remote
	make  -f make_remote clean
	make  -f make_master
	make  -f make_master clean
	make  -f make_remote_rpc
clean:
	make  -f make_remote clean
	make  -f make_master clean
	make  -f make_remote_rpc clean

PHONY: all clean
