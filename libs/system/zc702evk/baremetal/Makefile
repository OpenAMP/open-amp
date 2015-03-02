# Make file to create baremetal lib.


all:
	make  -f make_remote
	make  -f make_master
clean:
	make  -f make_remote clean
	make  -f make_master clean
	rm -rf .build

PHONY: all clean
