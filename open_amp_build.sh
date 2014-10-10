
# This scripts performs all steps necessary to build Nucleus master apps

# Check if the user wants to clean everything    
if [ "$1" == "-c" ]; then
	make clean
	
	make -f libs/system/zc702evk/linux/make clean
		
	cd apps
	
	make OS=nucleus PLAT=zc702evk ROLE=remote clean
	
	make OS=baremetal PLAT=zc702evk ROLE=remote clean
	
	make OS=nucleus PLAT=zc702evk ROLE=master clean
	
	make OS=baremetal PLAT=zc702evk ROLE=master clean
		
	make cleanbenchmark OS=nucleus PLAT=zc702evk ROLE=master
	
	make clean_linux_remote OS=nucleus PLAT=zc702evk ROLE=master

	make clean_linux_remote OS=baremetal PLAT=zc702evk ROLE=master
	
	make clean_linux_remote_benchmark OS=nucleus PLAT=zc702evk ROLE=master
		
	cd firmware 
	
	find . -name "firmware" -delete
	    
	cd ../..

elif [ "$1" == "-p" ]; then
	
	# Build OPENAMP library for Nucleus master with benchmarking enabled

	make clean

   	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Building open AMP components..
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	make OS=nucleus PLAT=zc702evk ROLE=remote

    	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Build remote Nucleus applications
    	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	cd apps

    	echo ~~~~~~~~~~~~~~~~~~~~~~~
	echo Cleaning applications..
	echo ~~~~~~~~~~~~~~~~~~~~~~~

	make OS=nucleus PLAT=zc702evk ROLE=remote clean

    	echo ~~~~~~~~~~~~~~~~~~~~~~~
	echo Building applications..
	echo ~~~~~~~~~~~~~~~~~~~~~~~

	make OS=nucleus PLAT=zc702evk ROLE=remote

	cd ..

	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Build OPENAMP library for remote baremetal
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	make clean

	make OS=baremetal PLAT=zc702evk ROLE=remote

    	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Build remote baremetal applications
    	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	cd apps

	make OS=baremetal PLAT=zc702evk ROLE=remote clean

	make OS=baremetal PLAT=zc702evk ROLE=remote
	
	cd ..

	make clean

	make OS=nucleus PLAT=zc702evk ROLE=master BENCHMARK=1

	# Build nucleus master benchmark
	cd apps
    
	make cleanbenchmark OS=nucleus PLAT=zc702evk ROLE=master
    
	make benchmark OS=nucleus PLAT=zc702evk ROLE=master

	cd ..

	make -f libs/system/zc702evk/linux/make clean
	
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Building Linux Bootstrap
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	make -f libs/system/zc702evk/linux/make

	make clean

	make OS=nucleus PLAT=zc702evk ROLE=master BENCHMARK=1 LINUXREMOTE=1

	# Build nucleus master benchmark
	cd apps
    
	make clean_linux_remote_benchmark OS=nucleus PLAT=zc702evk ROLE=master
    
	make linux_remote_benchmark OS=nucleus PLAT=zc702evk ROLE=master

	cd ..
	
else
    	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Build OPENAMP library for remote Nucleus
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Cleaning open AMP components..
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	make clean

    	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Building open AMP components..
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	make OS=nucleus PLAT=zc702evk ROLE=remote

    	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Build remote Nucleus applications
    	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	cd apps

    	echo ~~~~~~~~~~~~~~~~~~~~~~~
	echo Cleaning applications..
	echo ~~~~~~~~~~~~~~~~~~~~~~~

	make OS=nucleus PLAT=zc702evk ROLE=remote clean

    	echo ~~~~~~~~~~~~~~~~~~~~~~~
	echo Building applications..
	echo ~~~~~~~~~~~~~~~~~~~~~~~

	make OS=nucleus PLAT=zc702evk ROLE=remote

	cd ..

	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Build OPENAMP library for remote baremetal
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Cleaning open AMP components..
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	make clean

    	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Building open AMP components..
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	make OS=baremetal PLAT=zc702evk ROLE=remote

    	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Build remote baremetal applications
    	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	cd apps

	echo ~~~~~~~~~~~~~~~~~~~~~~~
	echo Cleaning applications..
	echo ~~~~~~~~~~~~~~~~~~~~~~~

	make OS=baremetal PLAT=zc702evk ROLE=remote clean

    	echo ~~~~~~~~~~~~~~~~~~~~~~~
	echo Building applications..
	echo ~~~~~~~~~~~~~~~~~~~~~~~

	make OS=baremetal PLAT=zc702evk ROLE=remote

	cd ..

	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Cleaning Linux Bootstrap
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	make -f libs/system/zc702evk/linux/make clean
	
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Building Linux Bootstrap
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	make -f libs/system/zc702evk/linux/make
	
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Build OPENAMP library for master Nucleus
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Cleaning open AMP components..
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	make clean

    	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Building open AMP components..
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	make OS=nucleus PLAT=zc702evk ROLE=master

    	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Build master nucleus applications
    	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	cd apps

	echo ~~~~~~~~~~~~~~~~~~~~~~~
	echo Cleaning applications..
	echo ~~~~~~~~~~~~~~~~~~~~~~~

	make OS=nucleus PLAT=zc702evk ROLE=master clean

	echo ~~~~~~~~~~~~~~~~~~~~~~~
	echo Building applications..
	echo ~~~~~~~~~~~~~~~~~~~~~~~

	make OS=nucleus PLAT=zc702evk ROLE=master

	cd ..
	
	
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Build OPENAMP library for master Nucleus and remote Linux
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Cleaning open AMP components..
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	make clean

    echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Building open AMP components..
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	make OS=nucleus PLAT=zc702evk ROLE=master LINUXREMOTE=1

	# Build nucleus master with linux remote
	cd apps
    
    echo ~~~~~~~~~~~~~~~~~~~~~~~
	echo Cleaning applications..
	echo ~~~~~~~~~~~~~~~~~~~~~~~
	
	make clean_linux_remote OS=nucleus PLAT=zc702evk ROLE=master
    
    
    echo ~~~~~~~~~~~~~~~~~~~~~~~
	echo Building applications..
	echo ~~~~~~~~~~~~~~~~~~~~~~~
	make linux_remote OS=nucleus PLAT=zc702evk ROLE=master

	cd ..
	

	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Build OPENAMP library for master baremetal and remote Linux
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Cleaning open AMP components..
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	make clean

    echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Building open AMP components..
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	make OS=baremetal PLAT=zc702evk ROLE=master LINUXREMOTE=1

	# Build nucleus master with linux remote
	cd apps
    
    echo ~~~~~~~~~~~~~~~~~~~~~~~
	echo Cleaning applications..
	echo ~~~~~~~~~~~~~~~~~~~~~~~
	
	make clean_linux_remote OS=baremetal PLAT=zc702evk ROLE=master
    
    
    echo ~~~~~~~~~~~~~~~~~~~~~~~
	echo Building applications..
	echo ~~~~~~~~~~~~~~~~~~~~~~~
	make linux_remote OS=baremetal PLAT=zc702evk ROLE=master

	cd ..
	
fi
