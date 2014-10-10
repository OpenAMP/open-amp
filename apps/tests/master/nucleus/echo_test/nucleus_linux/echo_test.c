/* This is a sample demonstration application that showcases usage of remoteproc
and rpmsg APIs. This application is meant to run on the master CPU running Nucleus
and showcases booting of linux remote firmware using remoteproc and 
IPC with remote firmware using rpmsg; Nucleus on master core acts as a remoteproc master
but as an rpmsg remote;It brings up a remote Linux based 
firmware which acts as an rpmsg master and transmits data payloads to Nucleus.
Linux app sends paylaods of incremental sizes to Nucleus which echoes them back to Linux. 
Once Linux application is complete, it requests a shutdown from Nucleus. 
Nucleus acknowledges with a shutdown message which results in Linux starting a system halt. 
Nucleus shutsdown the remote core after a reasonable delay which allows
Linux to gracefully shutdown. */

/* Including required headers */
#include  <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "open_amp.h"
#include "nucleus.h"
#include "kernel/nu_kernel.h"

/* Define the main task's stack size */
#define STACK_SIZE      (NU_MIN_STACK_SIZE * 64)

/* Define the main task's priority */
#define TASK_PRIORITY   26

/* Define the main task's time slice */
#define TASK_SLICE      20

#define MAX_RPMSG_BUFF_SIZE 512
#define PAYLOAD_MIN_SIZE    1    
#define PAYLOAD_MAX_SIZE    (MAX_RPMSG_BUFF_SIZE - 24)
#define NUM_PAYLOADS        (PAYLOAD_MAX_SIZE/PAYLOAD_MIN_SIZE)

#define SHUTDOWN_MSG	0xEF56A55A

/* Prototypes */
static void Main_Task_Entry( UNSIGNED argc , VOID *argv );

/* Application provided callbacks */
void rpmsg_channel_created( struct rpmsg_channel *rp_chnl );
void rpmsg_channel_deleted( struct rpmsg_channel *rp_chnl );
void rpmsg_read_cb( struct rpmsg_channel *rp_chnl , void *data , int len , void * pric , unsigned long src );

/* Globals */
NU_TASK Task_Control_Block;
NU_SEMAPHORE App_Sem;
struct rpmsg_endpoint *rp_ept;
struct rpmsg_channel *app_rp_chnl;
char fw_name1 []= "firmware1";

int global_count;

/***********************************************************************
 * *
 * *   FUNCTION
 * *
 * *       Application_Initialize
 * *
 * *   DESCRIPTION
 * *
 * *       Demo application entry point
 *
 * ***********************************************************************/
VOID Application_Initialize(NU_MEMORY_POOL* mem_pool , NU_MEMORY_POOL* uncached_mem_pool )
{
    VOID *pointer;
    STATUS status = NU_SUCCESS;

    /* Reference unused parameters to avoid toolset warnings */
    NU_UNUSED_PARAM( uncached_mem_pool );

    if (status == NU_SUCCESS)
    {
        status = NU_Allocate_Memory(mem_pool, &pointer, STACK_SIZE, NU_NO_SUSPEND);

        /* Create the main task for matrix processing */
        if ( status == NU_SUCCESS )
        {
            status = NU_Create_Task( &Task_Control_Block , "MAIN" , Main_Task_Entry , 0 ,
                                    uncached_mem_pool , pointer , STACK_SIZE , 
                                    TASK_PRIORITY , TASK_SLICE , NU_PREEMPT , NU_START );
        }
        if(status == NU_SUCCESS)
        {
            status = NU_Create_Semaphore(&App_Sem ,"APP_SEM", 0, NU_FIFO);
        }
    }

    /* Check to see if previous operations were successful */
    if ( status != NU_SUCCESS )
    {
        /* Loop forever */
        while ( 1 );
    }
}

/***********************************************************************
 * *
 * *   FUNCTION
 * *
 * *       Main_Task_Entry
 * *
 * *   DESCRIPTION
 * *
 * *       Entry function for the main task. This task prints a hello world
 * *       message.
 * *
 * ***********************************************************************/
static VOID Main_Task_Entry( UNSIGNED argc , VOID *argv ) {

    struct remote_proc *proc;
    int                status;
	int shutdown_msg = SHUTDOWN_MSG;
	    
	printf("\n\n\r************************************");
	printf("*******************************************\r\n");
	printf("\r\n              Echo Test Application \r\n");
	printf("\r\n");
	printf("\r\nThis test application will start a linux kernel on remote core and will act\r\n");
	printf("\r\nas an RPMSG remote itself to echo back any data that it receives from Linux\r\n");

	printf("\r\n******************************************");
	printf("*************************************\n\r\n");

	printf("\r\n******************************************");
	printf("*************************************\n\r\n");
    printf("\r\nLoading remote context : %s \r\n" , fw_name1);
	printf("\r\n******************************************");
	printf("*************************************\n\r\n");
    
  
    status = remoteproc_init((void *) fw_name1, rpmsg_channel_created, rpmsg_channel_deleted, rpmsg_read_cb, &proc);
    
    if(!status && (proc))
    {
		printf("\r\n\n********************************************\r\n");
        printf("BOOTING LINUX REMOTE FIRMWARE");
	    printf("\r\n********************************************\r\n\n");
        status = remoteproc_boot(proc);
    }
    
    if(!status)
    {   
	    /* Wait for shutdown message. */
	    NU_Obtain_Semaphore(&App_Sem, NU_SUSPEND);
			
		/* Send shutdown message to remote */
		rpmsg_send(app_rp_chnl, &shutdown_msg, sizeof(int));
		
		/* The remote Linux kernel requires around ~15 seconds to shutdown itself. Wait. */
		NU_Sleep(100 * 18);
		
		remoteproc_shutdown(proc);
		
		remoteproc_deinit(proc);

    }
    else
    {
        printf("\r\n\nLoading remote context: %s failed \r\n" , fw_name1);
    }       

    printf("\r\nTest Completed \r\n");
}

/* This callback gets invoked when the remote chanl is created */
void rpmsg_channel_created(struct rpmsg_channel *rp_chnl) {

    app_rp_chnl = rp_chnl;
    
    rp_ept = rpmsg_create_ept(rp_chnl , rpmsg_read_cb ,RPMSG_NULL , RPMSG_ADDR_ANY);
}

/* This callback gets invoked when the remote channel is deleted */
void rpmsg_channel_deleted(struct rpmsg_channel *rp_chnl) {
    
    rpmsg_destroy_ept(rp_ept);

}

/* This is the read callback, note we are in a task context when this callback
is invoked, so kernel primitives can be used freely */
void rpmsg_read_cb(struct rpmsg_channel *rp_chnl, void *data, int len,
                void * priv, unsigned long src) {
   
    if ((*(int *) data) == SHUTDOWN_MSG)
    {
        NU_Release_Semaphore(&App_Sem);
    }
    else
    {
	    rpmsg_send(rp_chnl, data, len);
    }
}

