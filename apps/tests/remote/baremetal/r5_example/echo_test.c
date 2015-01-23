/* This is a sample demonstration application that showcases usage of rpmsg 
This application is meant to run on the remote CPU running baremetal code. 
This application echoes back data that was sent to it by the master core. */

#include <stdio.h>
#include <string.h>
#include "open_amp.h"
#include "rsc_table.h"
#include "baremetal.h"
#include "xpseudo_asm_gcc.h"

#define SHUTDOWN_MSG 0xEF56A55A

/* Internal functions */
static void rpmsg_channel_created(struct rpmsg_channel *rp_chnl);
static void rpmsg_channel_deleted(struct rpmsg_channel *rp_chnl);
static void rpmsg_read_cb(struct rpmsg_channel *, void *, int, void *, unsigned long);
static void sleep();
static void init_system();

/* Globals */
char DBG_MSG[1024] = {1, 2, 3, 4} ; 
static struct rpmsg_channel *app_rp_chnl;
static struct rpmsg_endpoint *rp_ept;
static struct remote_proc *proc = NULL;
static struct rsc_table_info rsc_info;
extern const struct remote_resource_table resources;

/* Application entry point */
int main() {
	
	/* Switch to System Mode */
	SWITCH_TO_SYS_MODE();

	/* Initialize HW system components */
	init_system();

	rsc_info.rsc_tab = (struct resource_table *)&resources;
	rsc_info.size = sizeof(resources);
	sprintf(DBG_MSG, "i am in main function.\n");

	remoteproc_resource_init(&rsc_info, rpmsg_channel_created, rpmsg_channel_deleted, rpmsg_read_cb,
		&proc);
#if 0
/* Test Interrupt Only */
#define XSCUGIC_SPI_CPU0_MASK 1
#define INTC_DEVICE_INT_ID 14
#define XSCUGIC_SFI_TRIG_CPU_MASK  0x00FF0000U    /**< CPU Target list */
#define XSCUGIC_SFI_TRIG_INTID_MASK        0x0000000FU /**< Set to the INTID signaled to the CPU */ 

	unsigned int int_mask = ((XSCUGIC_SPI_CPU0_MASK << 16U) | INTC_DEVICE_INT_ID) &
			(XSCUGIC_SFI_TRIG_CPU_MASK | XSCUGIC_SFI_TRIG_INTID_MASK);

	XScuGic_DistWriteReg(XSCUGIC_SFI_TRIG_OFFSET, int_mask);
#endif
#if 0
/* Test IPI */
     HIL_MEM_WRITE32((0xff300000 + IPI_TRIG_OFFSET), 0x100);
#endif

	while (1);
}

static void rpmsg_channel_created(struct rpmsg_channel *rp_chnl) {
	app_rp_chnl = rp_chnl;
 //sprintf(DBG_MSG, "i am in %s.\n", __func__);
	rp_ept = rpmsg_create_ept(rp_chnl, rpmsg_read_cb, RPMSG_NULL,
			RPMSG_ADDR_ANY);
}

static void rpmsg_channel_deleted(struct rpmsg_channel *rp_chnl) {
}

static void rpmsg_read_cb(struct rpmsg_channel *rp_chnl, void *data, int len,
		void * priv, unsigned long src) {
	sprintf(DBG_MSG, "i am in %s.\n", __func__);
	if ((*(int *) data) == SHUTDOWN_MSG) {
		remoteproc_resource_deinit(proc);
	} else {
		/* Send data back to master */
		rpmsg_send(rp_chnl, data, len);
	}
}

static void init_system() {

	/* To Do -- Fix Me later */
	/* Place the vector table -- Do we need it? */
	/* Initialize stacks -- Do we need it? */
	/* Initilaize GIC */
	zynqMP_r5_gic_initialize();
}
