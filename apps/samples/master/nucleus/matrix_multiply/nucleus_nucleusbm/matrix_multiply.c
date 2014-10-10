/* This is a sample demonstration application that showcases usage of remoteproc
and rpmsg APIs. This application is meant to run on the master CPU running Nucleus
and showcases booting of two sub-sequent remote firmware cycles using remoteproc and 
IPC with remote firmware using rpmsg; 1. It brings up a remote Nucleus based remote 
firmware which performs matrix multiplication, 2. It brings up a baremetal based 
remote firmware which performs matrix multiplication. Master app generates two random 
matrices and transmits them to remotr firmware which computes the product and transmit 
results back to master CPU */

/* Including required headers */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "nucleus.h"
#include "kernel/nu_kernel.h"
#include "open_amp.h"

/* Define the main task's stack size */
#define STACK_SIZE      (NU_MIN_STACK_SIZE * 64)

/* Define the main task's priority */
#define TASK_PRIORITY   26

/* Define the main task's time slice */
#define TASK_SLICE      20

#define MATRIX_SIZE 	6

#define SHUTDOWN_MSG	0xEF56A55A

typedef struct _matrix
{
    unsigned long size;
    unsigned long elements[MATRIX_SIZE][MATRIX_SIZE];
} matrix;

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

char fw1_name []= "firmware1"; /* Nucleus based matrix multiply remote firmware */
char fw2_name []= "firmware2"; /* Baremetal based matrix multiply remote firmware */

static struct _matrix* i_matrix;
static struct _matrix* r_matrix;
static int bytes_received;

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
            uncached_mem_pool , pointer , STACK_SIZE , TASK_PRIORITY , TASK_SLICE ,
            NU_PREEMPT , NU_START );
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
 * *       matrix_print
 * *
 * *   DESCRIPTION
 * *
 * *       Function to print a matrix.
 * *
 * ***********************************************************************/
static void matrix_print(struct _matrix* m)
{
    int i, j;

    /* Generate two random matrices */
    printf( " \r\n Master : Nucleus : Printing results \r\n");

    for (i = 0; i < m->size; ++i) {
        for (j = 0; j < m->size; ++j) {
            printf(" %d ", (unsigned int)m->elements[i][j]);
        }
        printf("\r\n");
    }
}

/***********************************************************************
 * *
 * *   FUNCTION
 * *
 * *       generate_matrices
 * *
 * *   DESCRIPTION
 * *
 * *       Function to generate random matrices.
 * *
 * ***********************************************************************/
static void generate_matrices(int num_matrices, 
                unsigned int matrix_size, void* p_data)
{
    int    i, j, k;
    struct _matrix* p_matrix = p_data;
    unsigned long value;

    /* Seed for random number generator */
    srand(ESAL_GE_TMR_OS_COUNT_READ());

    for(i=0; i< num_matrices; i++)
    {
        /* Initialize workload */
        p_matrix[i].size = matrix_size;
        
        printf( " \r\n Master : Nucleus : Input matrix %d \r\n", i);
        for(j = 0; j < matrix_size; j++)
        {
            printf( "\r\n");
            for(k = 0; k < matrix_size; k++)
            {
                value = (rand()&0x7F);
                value = value%10;
                p_matrix[i].elements[j][k] = value;
                printf( " %d ", (unsigned int)p_matrix[i].elements[j][k]);
            }
        }
        printf( "\r\n");
    }
    
}

/***********************************************************************
 * *
 * *   FUNCTION
 * *
 * *       matrix_multiply_shell
 * *
 * *   DESCRIPTION
 * *
 * *       Simple demo shell
 * *
 * ***********************************************************************/
static void matrix_multiply_shell(void)
{
    unsigned char cmd;
        
    for ( ; ; )
    {
        printf("\r\n **************************************** \r\n");
        printf(" Please enter command.\r\n");
        printf(" **************************************** \r\n");
        printf(" 1 - Generates random 6x6 matrices and transmits them to remote core over rpmsg .. \r\n"); 
        printf(" 2 - Quit this application .. \r\n");
        printf(" CMD>");
        
        cmd = getchar();
        
        if(cmd == '1')
        {
            printf("\r\n Generating random matrices now ... \r\n");            

            generate_matrices(2, 6, i_matrix); 

            printf("\r\n Writing generated matrices to rpmsg device, %d bytes written .. \r\n", \
                    sizeof(*i_matrix)*2);

            rpmsg_send(app_rp_chnl, i_matrix, sizeof(*i_matrix)*2);

            NU_Obtain_Semaphore(&App_Sem , NU_SUSPEND);
            
            printf("\r\n Received results! - %d bytes from rpmsg device (transmitted from remote context) \r\n", bytes_received);

            matrix_print(r_matrix);

        }
        else if(cmd == '2')
        {
            printf("\r\n Quitting application .. \r\n");
            printf("\r\n Matrix multiplication demo end \r\n");

            break;
        }
        else
        {
            printf("\r\n invalid command! \r\n");
        }
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
    NU_MEMORY_POOL     *sys_pool_ptr;
    STATUS             status;
    int shutdown_msg = SHUTDOWN_MSG;

    status = NU_System_Memory_Get(&sys_pool_ptr, NU_NULL);
    
	/* Allocate memory for input matrices */
    if(status == NU_SUCCESS)
    {
        status = NU_Allocate_Memory (sys_pool_ptr, (VOID *)&i_matrix,sizeof(*i_matrix)*2, NU_NO_SUSPEND);
    }
    
	/* Allocate memory for result matrix */
    if(status == NU_SUCCESS)
    {
        status = NU_Allocate_Memory (sys_pool_ptr, (VOID *)&r_matrix,sizeof(*r_matrix), NU_NO_SUSPEND);
    }
    
    /* Start first firmware*/
    if(status == NU_SUCCESS)
    {   
		printf("\n\n\r************************************");
		printf("*******************************************\r\n");
		printf("\r\n              Matrix Multiplication Application \r\n");
		printf("\r\n");
		printf("\r\nThis sample application will offload matrix multiplication \r\n");
		printf("\r\noperations from Nucleus master to remote cores\r\n");

		printf("\r\n******************************************");
		printf("*************************************\n\r\n");
    
    
        printf("\r\n\nLoading remote context : %s \r\n" , fw1_name);
        
        printf("\r\n******************************************");
		printf("*************************************\n\r\n");
            
        status = remoteproc_init((void *) fw1_name, rpmsg_channel_created, rpmsg_channel_deleted, rpmsg_read_cb, &proc);
    }
    
	/* Boot first firmware */
    if((!status) && (proc))
    {
        printf("\r\n\n********************************************\r\n");
        printf("BOOTING NUCLEUS REMOTE FIRMWARE");
	    printf("\r\n********************************************\r\n\n");
        status = remoteproc_boot(proc);
    }
    
    if(!status)
    {
        /* Wait till communication channel is established */
        NU_Obtain_Semaphore(&App_Sem, NU_SUSPEND);
 
        printf("\r\n******************************************");
		printf("*************************************\n\r\n");
        printf("\r\nStarting application for first firmware  .. \r\n");
        printf("\r\n******************************************");
		printf("*************************************\n\r\n");
		
		/* Call matrix multiply application */
        matrix_multiply_shell();

		printf("\r\n******************************************");
		printf("*************************************\n\r\n");
        printf("\r\nShutting down and deinitializing remote context : %s \r\n" , fw1_name);
		printf("\r\n******************************************");
		printf("*************************************\n\r\n");

        /* Send shutdown message to remote */
		rpmsg_send(app_rp_chnl, &shutdown_msg, sizeof(int));

        /* Wait for channel deletion callback. */
        NU_Obtain_Semaphore(&App_Sem , NU_SUSPEND);

		/* Shutdown remote processor and deinitialize remoteproc */
        remoteproc_shutdown(proc);
        remoteproc_deinit(proc);
    }
    else
    {
        printf("\r\n\nLoading remote context: %s failed \r\n" , fw1_name);
    }        
    
	/* Start second firmware */
    status = remoteproc_init((void *) fw2_name, rpmsg_channel_created, rpmsg_channel_deleted, rpmsg_read_cb, &proc);

	/* Boot first firmware */
    if((!status) && (proc))
    {
	    printf("\r\n\n********************************************\r\n");
        printf("BOOTING BAREMETAL  REMOTE FIRMWARE");
	    printf("\r\n********************************************\r\n\n");
        status = remoteproc_boot(proc);
    }
    
    if(!status)
    {
		/* Wait till communication channel is established */
        NU_Obtain_Semaphore(&App_Sem, NU_SUSPEND);
    
        printf("\r\n******************************************");
		printf("*************************************\n\r\n");
        printf("\r\nStarting application for second firmware  .. \r\n");
        printf("\r\n******************************************");
		printf("*************************************\n\r\n");
        
		/* Call matrix multiply application */
        matrix_multiply_shell();
        
		printf("\r\n******************************************");
		printf("*************************************\n\r\n");
        printf("\r\nShutting down and deinitializing remote context : %s \r\n" , fw2_name);
		printf("\r\n******************************************");
		printf("*************************************\n\r\n");

		/* Shut down remote processor and deinitialize remoteproc */
        remoteproc_shutdown(proc);
        remoteproc_deinit(proc);
    }
    else
    {
        printf("\r\n\nLoading remote context: %s failed \r\n" , fw2_name);
    }        
    
}

/* This callback gets invoked when the remote channel is created */
void rpmsg_channel_created(struct rpmsg_channel *rp_chnl) {
    app_rp_chnl = rp_chnl;
    rp_ept = rpmsg_create_ept(rp_chnl , rpmsg_read_cb ,RPMSG_NULL , RPMSG_ADDR_ANY);
    NU_Release_Semaphore(&App_Sem);
}

/* This callback gets invoked when the remote channel is deleted */
void rpmsg_channel_deleted(struct rpmsg_channel *rp_chnl) {
    NU_Release_Semaphore(&App_Sem);
}

/* This is the read callback, note we are in a task context when this callback
is invoked, so kernel primitives can be used freely */
void rpmsg_read_cb(struct rpmsg_channel *rp_chnl, void *data, int len,
                void * priv, unsigned long src) {

    memcpy(r_matrix,data,len);
    
    bytes_received = len;
    
    NU_Release_Semaphore(&App_Sem);
}

