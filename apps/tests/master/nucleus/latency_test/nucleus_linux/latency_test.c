/* This is a test demonstration application that mesaures performance of remoteproc
and rpmsg APIs. This application is meant to run on the master CPU running Nucleus
and showcases booting of two sub-sequent remote firmware cycles using remoteproc and 
IPC with remote firmware using rpmsg; It brings up a remote linux based remote 
firmware which can respond to test calls. Master app executes tests to caluclate the performance 
of the rpmsg and remoteproc APIs and shutsdown the core once the test has been completed.*/

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

struct _payload* p_payload = NULL;
struct _payload* r_payload = NULL;

UINT64 time_start, time_end;
UINT64 fw1_average_rtt, fw2_average_rtt;
UINT64 fw1_boot_time, fw2_boot_time;
UINT64 fw1_shutdown_time, fw2_shutdown_time;
    
UINT64 payload_roundtrip_time[NUM_PAYLOADS];

extern unsigned long long    boot_time_stamp;
extern unsigned long long    shutdown_time_stamp;

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
VOID Application_Initialize(NU_MEMORY_POOL* mem_pool , NU_MEMORY_POOL* uncached_mem_pool)
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
static VOID Main_Task_Entry( UNSIGNED argc , VOID *argv ) 
{
    struct remote_proc *proc;
    int                i, size, status;
    NU_MEMORY_POOL     *sys_pool_ptr;

    NU_System_Memory_Get(&sys_pool_ptr, NU_NULL);
   
    printf("\r\n\nExecuting tests for remote context : %s. It corresponds to  Linux remote firmware \r\n" , fw_name1);
    
    /* Allocate memory for Rx buffer. */
    NU_Allocate_Memory(sys_pool_ptr, (VOID **)&r_payload, (2 * sizeof(unsigned long) + PAYLOAD_MAX_SIZE), NU_NO_SUSPEND);
            
    /* Initialize remoteproc for first firmware. */
    status = remoteproc_init((void *) fw_name1, rpmsg_channel_created, rpmsg_channel_deleted, rpmsg_read_cb, &proc);
    
    if(!status && (proc))
    {
        /* Note time stamp. */
        time_start = NU_Get_Time_Stamp();
        
        /* Boot firmware 1. */            
        status = remoteproc_boot(proc);
    }
    
    if(!status)
    {
        /* Firmware 1 boot time. */
        fw1_boot_time = boot_time_stamp - time_start;
            
        /* Wait for channel creation complete  callback. */
        NU_Obtain_Semaphore(&App_Sem, NU_SUSPEND);
        
        /* Send paylaods of incremental data sizes. */                
        for(i = 0, size=PAYLOAD_MIN_SIZE; i < NUM_PAYLOADS; i++, size++)
        {     
            NU_Allocate_Memory(sys_pool_ptr, (VOID **)&p_payload, (2 * sizeof(unsigned long) + size), NU_NO_SUSPEND);
            
            /* Setup payload size and properties. */        
            p_payload->num = i;
            p_payload->size = size;
            
            /* Setup the buffer with a pattern*/    
            memset(&(p_payload->data[0]), 0xA5, size);  
            
            time_start = NU_Get_Time_Stamp();
              
            /* Send data to remote side. */
            rpmsg_send(app_rp_chnl, p_payload, (2 * sizeof(unsigned long)) + size);
            
            /* Wait for echo. */
            NU_Obtain_Semaphore(&App_Sem , NU_SUSPEND);
            
            NU_Deallocate_Memory(p_payload);
            
            /* Save payload round-trip time.*/
            payload_roundtrip_time[i] = time_end - time_start;
        }
        
        /* Average out the payload round-trip time. */
        for(i =0; i<NUM_PAYLOADS; i++)
        {
            fw1_average_rtt += payload_roundtrip_time[i];
        }
        
        fw1_average_rtt = fw1_average_rtt / NUM_PAYLOADS;
        
        time_start = NU_Get_Time_Stamp();
                
        remoteproc_shutdown(proc);
        
        fw1_shutdown_time = shutdown_time_stamp - time_start;
                
        remoteproc_deinit(proc);
    }
    else
    {
        printf("\r\n\nLoading remote context: %s failed \r\n" , fw_name1);
    }       
        
    /* Convert the values into nano-seconds. */
    fw1_average_rtt = (fw1_average_rtt * 1000000000)/ (NU_HW_Ticks_Per_SW_Tick * NU_PLUS_TICKS_PER_SEC);
    fw1_boot_time = (fw1_boot_time * 1000000000) / (NU_HW_Ticks_Per_SW_Tick * NU_PLUS_TICKS_PER_SEC);
    fw1_shutdown_time = (fw1_shutdown_time * 1000000000)/ (NU_HW_Ticks_Per_SW_Tick * NU_PLUS_TICKS_PER_SEC);
    fw2_average_rtt = (fw2_average_rtt * 1000000000)/ (NU_HW_Ticks_Per_SW_Tick * NU_PLUS_TICKS_PER_SEC);
    fw2_boot_time = (fw2_boot_time * 1000000000) / (NU_HW_Ticks_Per_SW_Tick * NU_PLUS_TICKS_PER_SEC);
    fw2_shutdown_time = (fw2_shutdown_time * 1000000000)/ (NU_HW_Ticks_Per_SW_Tick * NU_PLUS_TICKS_PER_SEC);
    
    printf("\r\n **************************************** \r\n");
    printf(" OpenAMP Latency Test Results \r\n");
    printf(" **************************************** \r\n");
    
    printf("\r\n Nucleus Remote: Average rpmsg_send API Round-Trip:           %d ns\r\n", (int)fw1_average_rtt);
    printf("\r\n Nucleus Remote: remoteproc_boot API:                         %d ns\r\n", (int)fw1_boot_time);
    printf("\r\n Nucleus Remote: remoteproc_shutdown API:                     %d ns\r\n", (int)fw1_shutdown_time);
        
    printf("\r\n Baremetal Remote: Average rpmsg_send API Round-Trip:         %d ns\r\n", (int)fw2_average_rtt);
    printf("\r\n Baremetal Remote: remoteproc_boot API:                       %d ns\r\n", (int)fw2_boot_time);
    printf("\r\n Baremetal Remote: remoteproc_shutdown API:                   %d ns\r\n", (int)fw2_shutdown_time);
}

/* This callback gets invoked when the remote channel is created */
void rpmsg_channel_created(struct rpmsg_channel *rp_chnl) 
{
    app_rp_chnl = rp_chnl;
    
    rp_ept = rpmsg_create_ept(rp_chnl , rpmsg_read_cb ,RPMSG_NULL , RPMSG_ADDR_ANY);
    
    NU_Release_Semaphore(&App_Sem);
}

/* This callback gets invoked when the remote channel is deleted */
void rpmsg_channel_deleted(struct rpmsg_channel *rp_chnl) 
{   
    rpmsg_destroy_ept(rp_ept);
}

/* This is the read callback, note we are in a task context when this callback
is invoked, so kernel primitives can be used freely */
void rpmsg_read_cb(struct rpmsg_channel *rp_chnl, void *data, int len,
                void * priv, unsigned long src) 
{
    time_end = NU_Get_Time_Stamp();
    
    memcpy(r_payload,data,len);
    
    NU_Release_Semaphore(&App_Sem);
}

