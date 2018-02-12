/* @file xil_sd.h
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
#ifndef XIL_SD_H
#define XIL_SD_H

#include <ff.h>

/*****************************************************************************/
/**
 * This function is used to initialize the qspi controller and driver
 *
 * @param	None
 *
 * @return	None
 *
 *****************************************************************************/
int xil_sdInit(uint32_t DeviceFlags, const char *filename, FIL *fil);

/*****************************************************************************/
/**
 * This function is used to copy the data from SD/eMMC to destination
 * address
 *
 * @param SrcAddress is the address of the SD flash where copy should
 * start from
 *
 * @param DestAddress is the address of the destination where it
 * should copy to
 *
 * @param Length Length of the bytes to be copied
 *
 * @return
 *		- XFSBL_SUCCESS for successful copy
 *		- errors as mentioned in xfsbl_error.h
 *
 *****************************************************************************/
int xil_sdCopy(void *fil, uint32_t SrcAddress, unsigned long DestAddress,
	       uint32_t Length);

/*****************************************************************************/
/**
 * This function is used to release the sd settings
 *
 * @param	None
 *
 * @return	None
 *
 *****************************************************************************/
int xil_sdRelease(void *fil);

#endif /* end of XIL_SD_H */
