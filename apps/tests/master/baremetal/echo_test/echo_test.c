/* This is a sample demonstration application that showcases usage of remoteproc
and rpmsg APIs. This application is meant to run on the master CPU running baremetal env
and showcases booting of linux remote firmware using remoteproc and 
IPC with remote firmware using rpmsg; Baremetal env on master core acts as a remoteproc master
but as an rpmsg remote;It brings up a remote Linux based 
firmware which acts as an rpmsg master and transmits data payloads to bametal code.
Linux app sends paylaods of incremental sizes to baremetal code which echoes them back to Linux. 
Once Linux application is complete, it requests a shutdown from baremetal env. 
Baremetal env acknowledges with a shutdown message which results in Linux starting a system halt. 
Baremetal env shutsdown the remote core after a reasonable delay which allows
Linux to gracefully shutdown. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "open_amp.h"

#define BAREMETAL_MASTER 1

#include "baremetal.h"

#define SHUTDOWN_MSG	0xEF56A55A


/* Internal functions */
static void rpmsg_channel_created(struct rpmsg_channel *rp_chnl);
static void rpmsg_channel_deleted(struct rpmsg_channel *rp_chnl);
static void rpmsg_read_cb(struct rpmsg_channel *, void *, int, void *, unsigned long);
static void sleep();
static void init_system();

/* Globals */
static struct rpmsg_channel *app_rp_chnl;
static struct rpmsg_endpoint *rp_ept;

char fw_name []= "firmware1";

static int shutdown_called = 0;

/* Application entry point */
int main() {
	
	int status;
    struct remote_proc *proc;
	int shutdown_msg = SHUTDOWN_MSG;
	int i;

    /* Switch to System Mode */
    SWITCH_TO_SYS_MODE();

    /* Initialize HW system components */
    init_system();

    status = remoteproc_init((void *) fw_name, rpmsg_channel_created, rpmsg_channel_deleted, rpmsg_read_cb, &proc);
    
    if(!status)
    {
        status = remoteproc_boot(proc);
    }
    
    if(status)
    {
	    return -1;	
	}

    while (1) {
		
		if(shutdown_called == 1)
		{
		    break;
		}
        sleep();
    }

	/* Send shutdown message to remote */
	rpmsg_send(app_rp_chnl, &shutdown_msg, sizeof(int));

    for (i = 0; i < 100000; i++)
    {
	    sleep();
    }
    
	remoteproc_shutdown(proc);
		
	remoteproc_deinit(proc);
		
    return 0;
}

static void rpmsg_channel_created(struct rpmsg_channel *rp_chnl) {
    app_rp_chnl = rp_chnl;
    rp_ept = rpmsg_create_ept(rp_chnl, rpmsg_read_cb, RPMSG_NULL,
                    RPMSG_ADDR_ANY);
}

static void rpmsg_channel_deleted(struct rpmsg_channel *rp_chnl) {
    rpmsg_destroy_ept(rp_ept);
}

static void rpmsg_read_cb(struct rpmsg_channel *rp_chnl, void *data, int len,
                void * priv, unsigned long src) {
    
    if ((*(int *) data) == SHUTDOWN_MSG)
    {
		shutdown_called  = 1;
    }
    else
    { 
        /* Send data back to master*/	
        rpmsg_send(rp_chnl, data, len);
    }
}

void sleep() {
    volatile int i;
    for (i = 0; i < 100000; i++);
}


static void init_system() {

    /* Place the vector table at the image entry point */
    arm_arch_install_isr_vector_table(RAM_VECTOR_TABLE_ADDR);

    /* Enable MMU */
    arm_ar_mem_enable_mmu();

    /* Initialize ARM stacks */
    init_arm_stacks();

    /* Initialize GIC */
    zc702evk_gic_initialize();
}
