
/* This is a test application that runs Nucleus on the remote core 
and responds to commands from master core to test the usage of rpmsg APIs. */

/* Including required headers */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "open_amp.h"
#include "rsc_table.h"
#include "test_suite.h"
#include "nucleus.h"
#include "kernel/nu_kernel.h"

/* Prototypes */
static void Main_Task_Entry( UNSIGNED argc , VOID *argv );

#define EPT_ADDR        59

/* Application provided callbacks */
void rpmsg_channel_created( struct rpmsg_channel *rp_chnl );
void rpmsg_channel_deleted( struct rpmsg_channel *rp_chnl );
void rpmsg_read_default_cb( struct rpmsg_channel *rp_chnl , void *data , int len , void * pric ,
                unsigned long src );
void rpmsg_read_ept_cb( struct rpmsg_channel *rp_chnl , void *data , int len , void * pric ,
                unsigned long src );
/* Globals */
NU_TASK Task_Control_Block;
NU_SEMAPHORE App_Sem;
struct rpmsg_endpoint *rp_ept;
struct rpmsg_channel *app_rp_chnl;
UINT32 Src;
UINT32 Len;
CHAR firmware_name[] = "nucleus-fn-test-suite-remote-firmware";
CHAR r_buffer[512];
struct rsc_table_info rsc_info;
extern const struct remote_resource_table resources;
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
VOID Application_Initialize(
                NU_MEMORY_POOL* mem_pool , NU_MEMORY_POOL* uncached_mem_pool )
{
    VOID *pointer;
    STATUS status = NU_SUCCESS;

    /* Reference unused parameters to avoid toolset warnings */
    NU_UNUSED_PARAM( uncached_mem_pool );

    if ( status == NU_SUCCESS )
    {
        status = NU_Allocate_Memory(mem_pool, &pointer, STACK_SIZE, NU_NO_SUSPEND);

        /* Create the main task for matrix processing */
        if ( status == NU_SUCCESS )
        {
            status = NU_Create_Task( &Task_Control_Block , "MAIN" , Main_Task_Entry , 0 ,
                            uncached_mem_pool , pointer , STACK_SIZE , TASK_PRIORITY , TASK_SLICE ,
                            NU_PREEMPT , NU_START );
        }
        if ( status == NU_SUCCESS )
        {
            status = NU_Create_Semaphore( &App_Sem , "APP_SEM" , 0 , NU_FIFO );
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
    BOOLEAN uninit = NU_FALSE;
    struct ept_cmd_data *ept_data;

    rsc_info.rsc_tab = (struct resource_table *)&resources;
    rsc_info.size = sizeof(struct remote_resource_table);

    /* This API creates the virtio devices for this remote node and initializes
     other relevant resources defined in the resource table */
    remoteproc_resource_init( &rsc_info, rpmsg_channel_created , rpmsg_channel_deleted ,
                    rpmsg_read_default_cb , &proc );

    for ( ; ; )
    {

        NU_Obtain_Semaphore( &App_Sem , NU_SUSPEND );

        struct command *cmd = (struct command *)r_buffer;
        if(cmd->comm_start == CMD_START)
        {
            unsigned int cm_code = cmd->comm_code;
            void *data = cmd->data;

            switch ( cm_code )
            {
                case CREATE_EPT :
                    ept_data = (struct ept_cmd_data *)data;
                    rp_ept = rpmsg_create_ept( app_rp_chnl , rpmsg_read_ept_cb , RPMSG_NULL ,
                                    ept_data->dst );
                    if (rp_ept){
                        /* Send data back to ack. */
                        rpmsg_sendto( app_rp_chnl , r_buffer , Len , Src );
                    }
                    break;
                case DELETE_EPT :
                    rpmsg_destroy_ept(rp_ept);
                    rpmsg_sendto( app_rp_chnl , r_buffer , Len , Src );

                    break;
                case CREATE_CHNL :
                    break;
                case DELETE_CHNL :
                    rpmsg_sendto( app_rp_chnl , r_buffer , Len , Src );
                    remoteproc_resource_deinit( proc );
                    uninit = NU_TRUE;
                    break;
                case QUERY_FW_NAME :
                    rpmsg_send( app_rp_chnl , &firmware_name[0], strlen(firmware_name)+1);
                    break;
                default :
                    rpmsg_sendto( app_rp_chnl , r_buffer , Len , Src );
                    break;
            }
        }else
        {
            rpmsg_sendto( app_rp_chnl , r_buffer , Len , Src );
        }

        if ( uninit )
            break;
    }
}

/* This callback gets invoked when the remote chanl is created */
void rpmsg_channel_created( struct rpmsg_channel *rp_chnl )
{
    app_rp_chnl = rp_chnl;
}

/* This callback gets invoked when the remote channel is deleted */
void rpmsg_channel_deleted( struct rpmsg_channel *rp_chnl )
{

}

/* This is the read callback, note we are in a task context when this callback
 is invoked, so kernel primitives can be used freely */
void rpmsg_read_default_cb( struct rpmsg_channel *rp_chnl , void *data , int len , void * priv ,
                unsigned long src )
{
    memcpy( r_buffer , data , len );
    Src = src;
    Len = len;
    NU_Release_Semaphore( &App_Sem );
}

void rpmsg_read_ept_cb( struct rpmsg_channel *rp_chnl , void *data , int len , void * priv ,
                unsigned long src )
{
    rpmsg_send_offchannel( rp_chnl , rp_ept->addr , src , data , len );
}
