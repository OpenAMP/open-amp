/* This is a sample demonstration application that showcases usage of rpmsg 
This application is meant to run on the remote CPU running Nucleus.
This application echoes back data that was sent to it by the master core. */


/* Including required headers */
#include  <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "open_amp.h"
#include "rsc_table.h"
#include "nucleus.h"
#include "kernel/nu_kernel.h"

/* Define the main task's stack size */
#define STACK_SIZE      (NU_MIN_STACK_SIZE * 64)

/* Define the main task's priority */
#define TASK_PRIORITY   26

/* Define the main task's time slice */
#define TASK_SLICE      20

#define SHUTDOWN_MSG	0xEF56A55A

/* Prototypes */
static void Main_Task_Entry( UNSIGNED argc , VOID *argv );

/* Application provided callbacks */
void rpmsg_channel_created( struct rpmsg_channel *rp_chnl );
void rpmsg_channel_deleted( struct rpmsg_channel *rp_chnl );
void rpmsg_read_default_cb(struct rpmsg_channel *rp_chnl, void *data, int len,
                void * pric, unsigned long src);

/* Globals */
static NU_TASK Task_Control_Block;
static struct rpmsg_endpoint *rp_ept;
static struct rpmsg_channel *app_rp_chnl;
static struct remote_proc *proc;
static struct rsc_table_info rsc_info;
extern const struct remote_resource_table resources;
static volatile int shutdown_flag = 0;

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

    if (status == NU_SUCCESS) {
        status = NU_Allocate_Memory(mem_pool, &pointer, STACK_SIZE,
                        NU_NO_SUSPEND);

        /* Create the main task for matrix processing */
        if (status == NU_SUCCESS) {
            status = NU_Create_Task(&Task_Control_Block, "MAIN",
                            Main_Task_Entry, 0, uncached_mem_pool, pointer,
                            STACK_SIZE, TASK_PRIORITY, TASK_SLICE,
                            NU_PREEMPT, NU_START);
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

	rsc_info.rsc_tab = (struct resource_table *)&resources;
    rsc_info.size = sizeof(struct remote_resource_table);

    /* This API creates the virtio devices for this remote node and initializes
     other relevant resources defined in the resource table */
    remoteproc_resource_init(&rsc_info, rpmsg_channel_created,
                    rpmsg_channel_deleted, rpmsg_read_default_cb, &proc);

    for ( ; ; )
    {
        if(shutdown_flag)
        {
            remoteproc_resource_deinit(proc);
            break;
        }
        NU_Sleep(100);
    }
}

/* This callback gets invoked when the remote chanl is created */
void rpmsg_channel_created(struct rpmsg_channel *rp_chnl) {

    app_rp_chnl = rp_chnl;
    rp_ept = rpmsg_create_ept(rp_chnl, rpmsg_read_default_cb, RPMSG_NULL,
                    RPMSG_ADDR_ANY);
}

/* This callback gets invoked when the remote channel is deleted */
void rpmsg_channel_deleted(struct rpmsg_channel *rp_chnl) {

}

/* This is the read callback, note we are in a task context when this callback 
is invoked, so kernel primitives can be used freely */
void rpmsg_read_default_cb(struct rpmsg_channel *rp_chnl, void *data, int len,
                void * priv, unsigned long src) {

    if ((*((int *) data)) == SHUTDOWN_MSG) {
        shutdown_flag = 1;
    } else {
        /* Send the result of matrix multiplication back to master. */
        rpmsg_send(rp_chnl, data, len);
    }
}


