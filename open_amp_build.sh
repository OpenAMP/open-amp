if [ "$1" == "-c" ]; then
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Cleanup OPENAMP library for baremetal and Linux
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	make clean
	
	make -f libs/system/zc702evk/linux/make clean
	make -C libs/system/zc702evk/baremetal clean
		
	cd apps

	make SYSTEM=baremetal MACHINE=zc702evk ROLE=remote clean
	
	make SYSTEM=baremetal MACHINE=zc702evk ROLE=master clean

	make clean_linux_remote SYSTEM=baremetal MACHINE=zc702evk ROLE=master
		
	cd firmware 
	
	find . -name "firmware" -delete
	    
	cd ../..
	
else

	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Rebuild baremetal library
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	make -C libs/system/zc702evk/baremetal clean all

	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Build OPENAMP library for remote baremetal and Master Linux
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Cleaning open AMP components..
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	make clean

    	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Building open AMP components..
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	make SYSTEM=baremetal MACHINE=zc702evk ROLE=remote

    	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Build remote baremetal applications
    	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	cd apps

	echo ~~~~~~~~~~~~~~~~~~~~~~~
	echo Cleaning applications..
	echo ~~~~~~~~~~~~~~~~~~~~~~~

	make SYSTEM=baremetal MACHINE=zc702evk ROLE=remote clean

    	echo ~~~~~~~~~~~~~~~~~~~~~~~
	echo Building applications..
	echo ~~~~~~~~~~~~~~~~~~~~~~~

	make SYSTEM=baremetal MACHINE=zc702evk ROLE=remote

	cd ..

	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Cleaning Linux Bootstrap
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	make -f libs/system/zc702evk/linux/make clean
	
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	echo Building Linux Bootstrap
	echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	make -f libs/system/zc702evk/linux/make
	
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
	make SYSTEM=baremetal MACHINE=zc702evk ROLE=master LINUXREMOTE=1

	# Build baremetal master with linux remote
	cd apps
    
    echo ~~~~~~~~~~~~~~~~~~~~~~~
	echo Cleaning applications..
	echo ~~~~~~~~~~~~~~~~~~~~~~~
	
	make clean_linux_remote SYSTEM=baremetal MACHINE=zc702evk ROLE=master
    
    
    echo ~~~~~~~~~~~~~~~~~~~~~~~
	echo Building applications..
	echo ~~~~~~~~~~~~~~~~~~~~~~~
	make linux_remote SYSTEM=baremetal MACHINE=zc702evk ROLE=master

	cd ..
	
fi

