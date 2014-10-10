
/* This is a sample demonstration application that showcases usage of proxy from the remote core. 
This application is meant to run on the remote CPU running Nucleus. 
This applicationr can print to to master console and perform file I/O using proxy mechanism*/


/* Including required headers */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "open_amp.h"
#include "rsc_table.h"
#include "nucleus.h"
#include "kernel/nu_kernel.h"
#include "rpmsg_retarget.h"

/* Define the main task's stack size */
#define STACK_SIZE      (NU_MIN_STACK_SIZE * 64)

/* Define the main task's priority */
#define TASK_PRIORITY   26

/* Define the main task's time slice */
#define TASK_SLICE      20

/* Prototype for the main task's entry function */
static void Main_Task_Entry( UNSIGNED argc , VOID *argv );

static NU_TASK Task_Control_Block;
static NU_SEMAPHORE Remote_Sem;
volatile int int_flag = 0;
static struct rpmsg_channel *app_rp_chnl;
static struct remote_proc *proc;
static struct rsc_table_info rsc_info;
extern const struct remote_resource_table resources;

void rpmsg_channel_created( struct rpmsg_channel *rp_chnl );
void rpmsg_channel_deleted( struct rpmsg_channel *rp_chnl );
void rpmsg_read_default_cb( struct rpmsg_channel *rp_chnl , void *data , int len , void * pric ,
                unsigned long src );
void shutdown_cb(struct rpmsg_channel *rp_chnl);

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
    STATUS status;
    /* Reference unused parameters to avoid toolset warnings */
    NU_UNUSED_PARAM( uncached_mem_pool );

    status = NU_Create_Semaphore( &Remote_Sem , "r_sem" , 0 , NU_FIFO );

    if (status == NU_SUCCESS)
    {
          status = NU_Allocate_Memory(mem_pool, &pointer,
                              STACK_SIZE, NU_NO_SUSPEND);
          if ( status == NU_SUCCESS )
          {
               /* Create task 0.  */
               status = NU_Create_Task( &Task_Control_Block , "MAIN" , Main_Task_Entry , 0 ,
                                   uncached_mem_pool , pointer , STACK_SIZE , TASK_PRIORITY , TASK_SLICE ,
                                   NU_PREEMPT , NU_START );
          }
    }

    /* Check to see if previous operations were successful */
    if ( status != NU_SUCCESS )
    {
        /* Loop forever */
        while ( 1 );
    }
}

void rpmsg_channel_created(struct rpmsg_channel *rp_chnl)
{
    app_rp_chnl = rp_chnl;
    NU_Release_Semaphore( &Remote_Sem );
}

void rpmsg_channel_deleted(struct rpmsg_channel *rp_chnl)
{

}

void rpmsg_read_default_cb(struct rpmsg_channel *rp_chnl, void *data, int len,
                void * priv, unsigned long src)
{

}

#define REDEF_O_CREAT 100
#define REDEF_O_EXCL 200
#define REDEF_O_RDONLY 0
#define REDEF_O_WRONLY 1
#define REDEF_O_RDWR 2
#define REDEF_O_APPEND 2000
#define REDEF_O_ACCMODE 3


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
    int fd, bytes_written, bytes_read;
    char fname[]="remote.file";
    char wbuff[50];
    char rbuff[1024];
    char ubuff[50];
    float fdata;
    int idata;
    int ret;

    rsc_info.rsc_tab = (struct resource_table *)&resources;
    rsc_info.size = sizeof(resources);

    remoteproc_resource_init(&rsc_info, rpmsg_channel_created , rpmsg_channel_deleted ,rpmsg_read_default_cb, &proc);

    NU_Obtain_Semaphore( &Remote_Sem , NU_SUSPEND );

    rpmsg_retarget_init(app_rp_chnl , shutdown_cb);

    printf("\r\nRemote>******************************************************\r\n");
    printf("\r\nRemote>Nucleus RTOS Remote Procedure Call (RPC) Demonstration\r\n");
    printf("\r\nRemote>******************************************************\r\n");

    printf("\r\nRemote>Rpmsg based retargetting to proxy initialized..\r\n");

    /* Remote performing file IO on Master */
    printf("\r\nRemote>FileIO demo using open, write, read, close CRTL calls ..\r\n");

    printf("\r\nRemote>Creating a file on master and writing to it..\r\n");
    fd = open(fname, REDEF_O_CREAT | REDEF_O_WRONLY | REDEF_O_APPEND, S_IRUSR | S_IWUSR);
    printf("\r\nRemote>Opened file '%s' with fd = %d\r\n", fname, fd);

    sprintf(wbuff,"This is a test string being written to file..");
    bytes_written = write(fd, wbuff, strlen(wbuff));
    printf("\r\nRemote>Wrote to fd = %d, size = %d bytes \r\ncontent = %s\r\n", fd, bytes_written, wbuff);

    close(fd);
    printf("\r\nRemote>Closed fd = %d\r\n", fd);

    /* Remote performing file IO on Master */
    printf("\r\nRemote>Reading a file on master and displaying its contents..\r\n");
    fd = open(fname, REDEF_O_RDONLY, S_IRUSR | S_IWUSR);
    printf("\r\nRemote>Opened file '%s' with fd = %d\r\n", fname, fd);
    bytes_read = read(fd, rbuff, 1024);
    *(char*)(&rbuff[0]+bytes_read+1)=0;
    printf("\r\nRemote>Read from fd = %d, size = %d bytes \r\ncontent = %s\r\n", fd, bytes_read, rbuff);
    close(fd);
    printf("\r\nRemote>Closed fd = %d\r\n", fd);

    for(;;)
    {
        /* Remote performing STDIO on Master */
        printf("\r\nRemote>Remote firmware using scanf and printf ..\r\n");
        printf("\r\nRemote>Scanning user input from master..\r\n");
        printf("\r\nRemote>Enter name\r\n");
        ret = scanf("%s", ubuff);
        if(ret) {
            printf("\r\nRemote>Enter age\r\n");
            ret = scanf("%d", &idata);
            if(ret) {
                printf("\r\nRemote>Enter value for pi\r\n");
                ret = scanf("%f", &fdata);
                if(ret) {
                    printf("\r\nRemote>User name = '%s'\r\n", ubuff);
                    printf("\r\nRemote>User age = '%d'\r\n", idata);
                    printf("\r\nRemote>User entered value of pi = '%f'\r\n", fdata);
                }
            }
        }

        if(!ret)
        {
            /* Consume the read buffer */
            while(getchar() != '\n');
            printf("Remote> Invalid value. Starting again....");
        }
        else
        {
            printf("\r\nRemote>Repeat demo ? (enter yes or no) \r\n");
            scanf("%s", ubuff);

            if((strcmp(ubuff,"no")) && (strcmp(ubuff,"yes")))
            {
                printf("\r\nRemote>Invalid option. Starting again....\r\n");
            }
            else if((!strcmp(ubuff,"no")))
            {
                printf("\r\nRemote>RPC retargetting quitting ...\r\n");
                break;
            }
        }
    }


    printf("\r\nRemote> Firmware's rpmsg-openamp-demo-channel going down! \r\n");
}

void shutdown_cb(struct rpmsg_channel *rp_chnl){
    rpmsg_retarget_deinit(rp_chnl);
    remoteproc_resource_deinit(proc);
}
