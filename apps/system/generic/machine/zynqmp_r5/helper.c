/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 *
 * Copyright (c) 2015 Xilinx, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "xparameters.h"
#include "xil_exception.h"
#include "xil_cache.h"
#include "xscugic.h"
#include <xreg_cortexr5.h>
#include <metal/alloc.h>
#include <metal/io.h>
#include <metal/device.h>
#include <metal/irq.h>
#include <metal/mutex.h>
#include <metal/sys.h>
#include <metal/utilities.h>
#include "platform_info.h"

#define INTC_DEVICE_ID		XPAR_SCUGIC_0_DEVICE_ID

static XScuGic xInterruptController;

/** IRQ descriptor structure */
struct metal_irq_desc {
	int irq;                  /**< interrupt number */
	metal_irq_handler hd;     /**< irq handler */
	void *drv_id;             /**< id to identify the driver
								   of the irq handler */
	struct metal_device *dev; /**< device identifier */
	struct metal_list node;   /**< node on irqs list */
};

/** IRQ state structure */
struct metal_irqs_state {
	struct metal_list irqs;   /**< interrupt descriptors */
	metal_mutex_t irq_lock;   /**< access lock */
};

static struct metal_irqs_state _irqs = {
	.irqs = METAL_INIT_LIST(_irqs.irqs),
	.irq_lock = METAL_MUTEX_INIT(_irqs.irq_lock),
};

int app_intr_cntr_register_irq(struct metal_irq_controller *cntr,
				int irq, metal_irq_handler hd,
				struct metal_device *dev, void *drv_id)
{
	struct metal_irq_desc *irq_p = NULL;
	struct metal_list *node;
	unsigned int irq_flags_save;

	(void)cntr;
	if (irq < 0) {
		metal_log(METAL_LOG_ERROR,
			  "%s: irq %d need to be a positive number\n",
		          __func__, irq);
		return -EINVAL;
	}

	/* Search for irq in list */
	metal_mutex_acquire(&_irqs.irq_lock);
	metal_list_for_each(&_irqs.irqs, node) {
		irq_p = metal_container_of(node, struct metal_irq_desc, node);

		if (irq_p->irq == irq) {
			/* Check if handler has already registered */
			if (irq_p->hd != NULL && hd != NULL &&
			    irq_p->hd != hd) {
				metal_log(METAL_LOG_ERROR,
					  "%s: irq %d already registered."
					  "Will not register again.\n",
					  __func__, irq);
				metal_mutex_release(&_irqs.irq_lock);
				return -EINVAL;
			} else {
				if (hd == NULL) {
					irq_flags_save = metal_irq_save_disable();
					metal_list_del(node);
					metal_irq_restore_enable(irq_flags_save);
					metal_free_memory(irq_p);
				}
				metal_mutex_release(&_irqs.irq_lock);
				return 0;
			}
		}
	}

	/* Either need to add handler to an existing list or to a new one */
	irq_p = metal_allocate_memory(sizeof(*irq_p));
	if (irq_p == NULL) {
		metal_log(METAL_LOG_ERROR,
		          "%s: irq %d cannot allocate mem for drv_id %d.\n",
		          __func__, irq, drv_id);
		metal_mutex_release(&_irqs.irq_lock);
		return -ENOMEM;
	}
	irq_p->hd = hd;
	irq_p->drv_id = drv_id;
	irq_p->dev = dev;
	irq_p->irq = irq;

	irq_flags_save = metal_irq_save_disable();
	metal_list_add_tail(&_irqs.irqs, &irq_p->node);
	metal_irq_restore_enable(irq_flags_save);
	metal_mutex_release(&_irqs.irq_lock);

	metal_log(METAL_LOG_DEBUG, "%s: success, irq %d add drv_id %p \n",
	          __func__, irq, drv_id);
	return 0;
}

/**
 * @brief metal IRQ controller functions, enable interrupt
 *
 * @param[in]  cntr pointer to interrupt controller
 * @param[in]  vector interrupt vector
 *
 * @return     0 for success, negative value for failure
 */
int app_intr_cntr_enable_irq(struct metal_irq_controller *cntr,
                 unsigned int vector)
{
	(void)cntr;
	XScuGic_Enable(&xInterruptController, vector);
	return 0;
}

/**
 * @brief metal IRQ controller functions, disable interrupt
 *
 * @param[in]  cntr pointer to interrupt controller
 * @param[in]  vector interrupt vector
 */
void app_intr_cntr_disable_irq(struct metal_irq_controller *cntr,
                   unsigned int vector)
{
	(void)cntr;
	XScuGic_Disable(&xInterruptController, vector);
}



struct metal_irq_controller app_intr_cntr = {
	.enable_irq =  app_intr_cntr_enable_irq,
	.disable_irq = app_intr_cntr_disable_irq,
	.register_irq = app_intr_cntr_register_irq,
};

/**
 * @brief default handler
 */
static void app_intr_cntr_isr(unsigned int vector)
{
	struct metal_list *node;
	struct metal_irq_desc *irq_p;

	metal_list_for_each(&_irqs.irqs, node) {
		irq_p = metal_container_of(node, struct metal_irq_desc, node);

		if ((unsigned int)irq_p->irq == vector) {
			if (irq_p->hd)
				(irq_p->hd)(vector, irq_p->drv_id);
			return;
		}
	}
}

/* Interrupt Controller setup */
static int app_gic_initialize(void)
{
	uint32_t status;
	XScuGic_Config *int_ctrl_config; /* interrupt controller configuration params */
	uint32_t int_id;
	uint32_t mask_cpu_id = ((u32)0x1 << XPAR_CPU_ID);
	uint32_t target_cpu;

	mask_cpu_id |= mask_cpu_id << 8U;
	mask_cpu_id |= mask_cpu_id << 16U;

	Xil_ExceptionDisable();

	/*
	 * Initialize the interrupt controller driver
	 */
	int_ctrl_config = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == int_ctrl_config) {
		return XST_FAILURE;
	}

	status = XScuGic_CfgInitialize(&xInterruptController, int_ctrl_config,
				       int_ctrl_config->CpuBaseAddress);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Only associate interrupt needed to this CPU */
	for (int_id = 32U; int_id<XSCUGIC_MAX_NUM_INTR_INPUTS;int_id=int_id+4U) {
		target_cpu = XScuGic_DistReadReg(&xInterruptController,
						XSCUGIC_SPI_TARGET_OFFSET_CALC(int_id));
		/* Remove current CPU from interrupt target register */
		target_cpu &= ~mask_cpu_id;
		XScuGic_DistWriteReg(&xInterruptController,
					XSCUGIC_SPI_TARGET_OFFSET_CALC(int_id), target_cpu);
	}
	XScuGic_InterruptMaptoCpu(&xInterruptController, XPAR_CPU_ID, IPI_IRQ_VECT_ID);

	/*
	 * Register the interrupt handler to the hardware interrupt handling
	 * logic in the ARM processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
			(Xil_ExceptionHandler)XScuGic_InterruptHandler,
			&xInterruptController);

	Xil_ExceptionEnable();

	/* Connect Interrupt ID with ISR */
	XScuGic_Connect(&xInterruptController, IPI_IRQ_VECT_ID,
			(Xil_ExceptionHandler)app_intr_cntr_isr,
			(void *)IPI_IRQ_VECT_ID);

	metal_irq_set_controller(&app_intr_cntr);

	return 0;
}

static void system_metal_logger(enum metal_log_level level,
			   const char *format, ...)
{
	(void)level;
	(void)format;
}


/* Main hw machinery initialization entry point, called from main()*/
/* return 0 on success */
int init_system(void)
{
	struct metal_init_params metal_param = {
		.log_handler = system_metal_logger,
		.log_level = METAL_LOG_INFO,
	};

	/* Low level abstraction layer for openamp initialization */
	metal_init(&metal_param);

	/* configure the global interrupt controller */
	app_gic_initialize();

	return 0;
}

void cleanup_system()
{
	metal_finish();

	Xil_DCacheDisable();
	Xil_ICacheDisable();
	Xil_DCacheInvalidate();
	Xil_ICacheInvalidate();
}
