open-amp
========
This repository is the home for the Open Asymmetric Multi Processing (OpenAMP)
framework project. The OpenAMP framework provides software components that
enable development of software applications for Asymmetric Multiprocessing
(AMP) systems. The framework provides the following key capabilities.

1. Provides Life Cycle Management, and Inter Processor Communication
   capabilities for management of remote compute resources and their associated
   software contexts
2. Provides a stand alone library usable with RTOS and Baremetal software
   environments
3. Compatibility with upstream Linux remoteproc and rpmsg components
4. Following AMP configurations supported
	a. Linux master/Baremetal remote
	b. Baremetal master/Linux remote
5. Proxy infrastructure and supplied demos showcase ability of proxy on master
   to handle printf, scanf, open, close, read, write calls from Bare metal
   based remote contexts.

Following are the known limitations:

1. In rpc_demo.c(the remote demonstration application that showcases usage of
   rpmsg retargetting infrastructure),  the bindings for the flag input
   parameter in open() system call has been redefined. The GCC tool library
   bindings for this input argument is different between arm-xilinx/none-eabi, and
   arm-linux-eabi toolchains. For this reason, redefinition is required for
   compatibility with proxy on Linux master.

For using the framework please refer to the documents present in the /docs folder.
Subscribe to the open-amp mailing list at https://groups.google.com/group/open-amp.
