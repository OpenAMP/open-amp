/*
 * Load firmware example
 *
 * Copyright(c) 2018 Xilinx Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 * * Neither the name Texas Instruments nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <metal/sys.h>
#include <openamp/remoteproc.h>
#include <openamp/remoteproc_loader.h>
#include <stdarg.h>
#include <stdio.h>
/* Xilinx headers */
#include <pm_api_sys.h>
#include <pm_defs.h>
#include <xil_printf.h>

#define LPRINTF(format, ...) xil_printf(format, ##__VA_ARGS__)
//#define LPRINTF(format, ...)
#define LPERROR(format, ...) LPRINTF("ERROR: " format, ##__VA_ARGS__)

extern struct remoteproc_ops r5_rproc_ops;
extern struct image_store_ops mem_image_store_ops;

struct mem_file {
	const void *base;
};

static struct mem_file image = {
	.base = (void *)0x3ED00000,
};

static XIpiPsu IpiInst;

static XStatus IpiConfigure(XIpiPsu *const IpiInstPtr)
{
	XStatus Status;
	XIpiPsu_Config *IpiCfgPtr;

	/* Look Up the config data */
	IpiCfgPtr = XIpiPsu_LookupConfig(XPAR_XIPIPSU_0_DEVICE_ID);
	if (NULL == IpiCfgPtr) {
		Status = XST_FAILURE;
		LPERROR("%s ERROR in getting CfgPtr\n", __func__);
		return Status;
	}

	/* Init with the Cfg Data */
	Status = XIpiPsu_CfgInitialize(IpiInstPtr, IpiCfgPtr, IpiCfgPtr->BaseAddress);
	if (XST_SUCCESS != Status) {
		LPERROR("%s ERROR #%d in configuring IPI\n", __func__, Status);
		return Status;
	}
	return Status;
}

static void app_log_handler(enum metal_log_level level,
			       const char *format, ...)
{
	char msg[1024];
	va_list args;
	static const char *level_strs[] = {
		"metal: emergency: ",
		"metal: alert:     ",
		"metal: critical:  ",
		"metal: error:     ",
		"metal: warning:   ",
		"metal: notice:    ",
		"metal: info:      ",
		"metal: debug:     ",
	};

	va_start(args, format);
	vsnprintf(msg, sizeof(msg), format, args);
	va_end(args);

	if (level <= METAL_LOG_EMERGENCY || level > METAL_LOG_DEBUG)
		level = METAL_LOG_EMERGENCY;

	xil_printf("%s%s", level_strs[level], msg);
}

int main(void)
{
	struct remoteproc rproc;
	struct remoteproc *ret_rproc;
	void *store = &image;
	unsigned int cpu_id = NODE_RPU_1;
	int ret;
	struct metal_init_params metal_param = {
		.log_handler = app_log_handler,
		.log_level = METAL_LOG_DEBUG,
	};

	if (XST_SUCCESS != IpiConfigure(&IpiInst)) {
		LPERROR("Failed to config IPI instance\n\r");
		return -1;
	}
	if (XST_SUCCESS != XPm_InitXilpm(&IpiInst)) {
		LPERROR("Failed to initialize PM\n\r");
		return -1;
	}

	LPRINTF("rproc app\n");
	/* Initialize libmetal evironment */
	metal_init(&metal_param);
	/* Initialize remoteproc instance */
	ret_rproc = remoteproc_init(&rproc, &r5_rproc_ops, &cpu_id);
	if (!ret_rproc) {
		LPRINTF("failed to initialize coprocessor\n\r");
		return -1;
	}

	/* Load remtoeproc firmware */
	LPRINTF("Start to load firmwaer\n\r");
	ret = remoteproc_load(&rproc, NULL, store, &mem_image_store_ops, NULL);
	if (ret) {
		LPRINTF("failed to load firmware\n\r");
		return -1;
	}
	/* Start the processor */
	ret = remoteproc_start(&rproc);
	if (ret) {
		LPRINTF("failed to start processor\n\r");
		return -1;
	}
	LPRINTF("successfully started the processor\n\r");
	/* ... */
	asm volatile("wfi");
	LPRINTF("going to stop the processor\n\r");
	remoteproc_stop(&rproc);
	/* application may want to do some cleanup before shutdown */
	LPRINTF("going to shutdown the processor\n\r");
	remoteproc_shutdown(&rproc);
	remoteproc_remove(&rproc);
	return 0;
}
