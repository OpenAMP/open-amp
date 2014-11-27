open-amp
========
This repository is a place holder for Open Asymmetric Multi Processing (OpenAMP) framework project. The OpenAMP framework provides software components that enable development of software applications for Asymmetric Multiprocessing (AMP) systems. OpenAMP provides following features:

1. Fully functional remoteproc and rpmsg components usable in following configurations;
	a. Linux master/Bare Metal remote configuration
	b. Bare Metal master/Linux remote configuration
2. Proxy infrastructure and supplied demos showcase ability of proxy on master to handle printf, scanf, open, close, read, write calls from Bare metal based remote contexts.
3. Documentation - OpenAMP User Manual, OpenAMP Getting Started Guide and Performance Test Measurements.
4. Performance tests have been conducted and OpenAMP latency measurements have been quantified and documented.

Following are the known limitations:

1. In rpc_demo.c(the remote demonstration application that showcases usage of rpmsg retargetting infrastructure),  the bindings for the flag input parameter in open() system call has been redefined. The GCC tool library bindings for this input argument is different between arm-xilinx/none-eabi, and arm-linux-eabi toolchains. For this reason, redefinition is required for compatibility with proxy on Linux master.


For using the framework please refer to the documents present in the /docs folder.
