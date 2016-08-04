
/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 *
 * Copyright (c) 2015 Xilinx, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the <ORGANIZATION> nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <string.h>
#include "xparameters.h"
#include "xil_exception.h"
#include "xscugic.h"
#include "xil_cache.h"
#include "metal/sys.h"

#define INTC_DEVICE_ID		XPAR_SCUGIC_0_DEVICE_ID

XScuGic InterruptController;

extern void bm_env_isr(int vector);
extern int platform_register_metal_device(void);

void zynqmp_r5_irq_isr()
{

	unsigned int raw_irq;
	int irq_vector;
	raw_irq =
	    (unsigned int)XScuGic_CPUReadReg(&InterruptController,
					     XSCUGIC_INT_ACK_OFFSET);
	irq_vector = (int)(raw_irq & XSCUGIC_ACK_INTID_MASK);

	bm_env_isr(irq_vector);

	XScuGic_CPUWriteReg(&InterruptController, XSCUGIC_EOI_OFFSET, raw_irq);
}

int zynqmp_r5_gic_initialize()
{
	u32 Status;

	Xil_ExceptionDisable();

	XScuGic_Config *IntcConfig;	/* The configuration parameters of the interrupt controller */

	/*
	 * Initialize the interrupt controller driver
	 */
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(&InterruptController, IntcConfig,
				       IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Register the interrupt handler to the hardware interrupt handling
	 * logic in the ARM processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
				     (Xil_ExceptionHandler) zynqmp_r5_irq_isr,
				     &InterruptController);

	Xil_ExceptionEnable();

	return 0;
}

void init_system()
{
	struct metal_init_params metal_param = METAL_INIT_DEFAULTS;

	metal_init(&metal_param);
	zynqmp_r5_gic_initialize();
	platform_register_metal_device();
}

void cleanup_system()
{
	metal_finish();
	Xil_DCacheDisable();
	Xil_ICacheDisable();
	Xil_DCacheInvalidate();
	Xil_ICacheInvalidate();
}
