/* This is a test demonstration application that showcases usage of remoteproc
and rpmsg APIs. This application is meant to run on the master CPU running Nucleus
and showcases booting of two sub-sequent remote firmware cycles using remoteproc and 
IPC with remote firmware using rpmsg; 1. It brings up a remote Nucleus based remote 
firmware which echoes back payload data, 2. It brings up a baremetal based 
remote firmware which echoes back payload data. Master app transmists paylaods of
varying sizes to the remote core which echoes them back. The master core validates
the data integrity and shutsdown the core once the test has been completed.*/

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

struct _payload {
    unsigned long    num;
    unsigned long    size;
    char             data[];
};

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
char fw_name2 []= "firmware2";

struct _payload* p_payload = NULL;
struct _payload* r_payload = NULL;

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
    int                idx ,i, size, status;
    NU_MEMORY_POOL     *sys_pool_ptr;
    int shutdown_msg = SHUTDOWN_MSG;

    NU_System_Memory_Get(&sys_pool_ptr, NU_NULL);
	
	printf("\n\n\r************************************");
	printf("*******************************************\r\n");
	printf("\r\n              Echo Test Application \r\n");
	printf("\r\n");
	printf("\r\nThis test application will send variable length data \r\n");
	printf("\r\npackets to remote cores\r\n");

	printf("\r\n******************************************");
	printf("*************************************\n\r\n");

	printf("\r\n******************************************");
	printf("*************************************\n\r\n");
    printf("\r\nLoading remote context : %s \r\n" , fw_name1);
	printf("\r\n******************************************");
	printf("*************************************\n\r\n");
    
    NU_Allocate_Memory(sys_pool_ptr, (VOID **)&r_payload, (2 * sizeof(unsigned long) + PAYLOAD_MAX_SIZE), NU_NO_SUSPEND);
            
    status = remoteproc_init((void *) fw_name1, rpmsg_channel_created, rpmsg_channel_deleted, rpmsg_read_cb, &proc);
    
    if(!status && (proc))
    {
		printf("\r\n\n********************************************\r\n");
        printf("BOOTING NUCLEUS REMOTE FIRMWARE");
	    printf("\r\n********************************************\r\n\n");
        status = remoteproc_boot(proc);
    }
    
    if(!status)
    {   
        /* Wait for channel creation complete  callback. */
        NU_Obtain_Semaphore(&App_Sem, NU_SUSPEND);
        
		printf("\r\n******************************************");
		printf("*************************************\n\r\n");
        printf("\r\nStarting echo test on : %s \r\n" , fw_name1);
		printf("\r\n******************************************");
		printf("*************************************\n\r\n");
        
        for(i = 0, size=PAYLOAD_MIN_SIZE; i < NUM_PAYLOADS; i++, size++)
        {     
            NU_Allocate_Memory(sys_pool_ptr, (VOID **)&p_payload, (2 * sizeof(unsigned long) + size), NU_NO_SUSPEND);
                    
            p_payload->num = i;
            
            p_payload->size = size;
            
            /* Setup the buffer with a pattern*/    
            memset(&(p_payload->data[0]), 0xA5, size);  
         
            printf("\r\nSending data packet of size: %d to remote firmware : %s \r\n" , size, fw_name1);
                    
            /* Send data to remote side. */
            rpmsg_send(app_rp_chnl, p_payload, (2 * sizeof(unsigned long)) + size);
            
            /* Wait for echo. */
            NU_Obtain_Semaphore(&App_Sem , NU_SUSPEND);
            
            /* Validate packet number*/
            if(p_payload->num != r_payload->num)
            {
                printf("\r\nError Echo packet number does not match with transmitted packet number: %d \r\n", (int)p_payload->num);
            }
                  
            /* Validate the data integrity. */
            for(idx = 0; idx < r_payload->size; idx++)
            {
                if(p_payload->data[idx] != r_payload->data[idx])
                {
                    printf("\r\nError receiving data packet of size: %d at data index %d \r\n", size, idx);
                    break;
                }
            }
            
            if(idx == size)
            {
                printf("\r\nReceived data packet of size: %d from remote successfully \r\n", size);
            }
            
            NU_Deallocate_Memory(p_payload);
        }

		printf("\r\n******************************************");
		printf("*************************************\n\r\n");  
        printf("\r\nRemoving remote context : %s \r\n" , fw_name1);
		printf("\r\n******************************************");
		printf("*************************************\n\r\n");  

        /* Send termination message to remote */
		rpmsg_send(app_rp_chnl, &shutdown_msg, sizeof(int));

        /* Wait for channel deletion callback. */
        NU_Obtain_Semaphore(&App_Sem , NU_SUSPEND);

        remoteproc_shutdown(proc);
        
        remoteproc_deinit(proc);

    }
    else
    {
        printf("\r\n\nLoading remote context: %s failed \r\n" , fw_name1);
    }       

	printf("\r\n******************************************");
	printf("*************************************\n\r\n"); 
    printf("\r\nLoading remote context : %s \r\n" , fw_name2);
	printf("\r\n******************************************");
	printf("*************************************\n\r\n"); 
	 
    status = remoteproc_init((void *) fw_name2, rpmsg_channel_created, rpmsg_channel_deleted, rpmsg_read_cb, &proc);
    
    if(!status && (proc))
    {
		printf("\r\n\n********************************************\r\n");
        printf("BOOTING BAREMETAL REMOTE FIRMWARE");
	    printf("\r\n********************************************\r\n\n");
        status = remoteproc_boot(proc);
    }
    
    if(!status) 
    {
        NU_Obtain_Semaphore(&App_Sem, NU_SUSPEND);

		printf("\r\n******************************************");
		printf("*************************************\n\r\n"); 
        printf("\r\nStarting echo test on : %s \r\n" , fw_name2);
		printf("\r\n******************************************");
		printf("*************************************\n\r\n"); 
        
        for(i = 0, size=PAYLOAD_MIN_SIZE; i < NUM_PAYLOADS; i++, size++)
        {     
            NU_Allocate_Memory(sys_pool_ptr, (VOID **)&p_payload, (2 * sizeof(unsigned long) + size), NU_NO_SUSPEND);
                                 
            p_payload->num = i;
            
            p_payload->size = size;
            
            /* Setup the buffer with a pattern*/    
            memset(&(p_payload->data[0]), 0xA5, size);  
         
            printf("\r\nSending data packet of size: %d to remote firmware : %s \r\n" , size, fw_name2);
                    
            /* Send data to remote side. */
            rpmsg_send(app_rp_chnl, p_payload, (2 * sizeof(unsigned long)) + size);
            
            /* Wait for echo. */
            NU_Obtain_Semaphore(&App_Sem , NU_SUSPEND);
            
            /* Validate packet number*/
            if(p_payload->num != r_payload->num)
            {
                printf("\r\nError Echo packet number does not match with transmitted packet number %d \r\n", (int)p_payload->num);
            }
                
            /* Validate the data integrity. */
            for(idx = 0; idx < size; idx++)
            {
                if(p_payload->data[idx] != r_payload->data[idx])
                {
                    printf("\r\nError receiving data packet of size: %d at data index %d \r\n", size, idx);
                    break;
                }
            }
            
            if(idx == size)
            {
                printf("\r\nReceived data packet of size: %d from remote successfully \r\n", size);
            }
            
            NU_Deallocate_Memory(p_payload);
        }
        
        printf("\r\nRemoving remote context : %s \r\n" , fw_name2);
        
        remoteproc_shutdown(proc);
        
        remoteproc_deinit(proc);
    }
    else
    {
        printf("\r\n\nLoading remote context: %s failed \r\n" , fw_name2);
    }
      
    printf("\r\nTest Completed \r\n");
}

/* This callback gets invoked when the remote chanl is created */
void rpmsg_channel_created(struct rpmsg_channel *rp_chnl) {

    app_rp_chnl = rp_chnl;
    
    rp_ept = rpmsg_create_ept(rp_chnl , rpmsg_read_cb ,RPMSG_NULL , RPMSG_ADDR_ANY);
    
    NU_Release_Semaphore(&App_Sem);
}

/* This callback gets invoked when the remote channel is deleted */
void rpmsg_channel_deleted(struct rpmsg_channel *rp_chnl) {
    
    rpmsg_destroy_ept(rp_ept);
    NU_Release_Semaphore(&App_Sem);

}

/* This is the read callback, note we are in a task context when this callback
is invoked, so kernel primitives can be used freely */
void rpmsg_read_cb(struct rpmsg_channel *rp_chnl, void *data, int len,
                void * priv, unsigned long src) {

    memcpy(r_payload,data,len);
    
    NU_Release_Semaphore(&App_Sem);
}

