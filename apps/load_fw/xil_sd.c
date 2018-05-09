/* @file xil_sd.c
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

#include <ff.h>
#include <xil_io.h>

/*****************************************************************************/
/**
 * This function is used to initialize the qspi controller and driver
 *
 * @param	None
 *
 * @return	None
 *
 *****************************************************************************/
int xil_sdInit(uint32_t DeviceFlags, const char *filename, FIL *fil)
{
	static FATFS fatfs;
	int Status;
	FRESULT rc;
	uint32_t DrvNum;

	DrvNum = DeviceFlags;

	/* Set logical drive number */
	/* Register volume work area, initialize device */
	if (DrvNum == 0) {
		rc=f_mount(&fatfs, "0:/", 0);
		xil_printf("SD: mount 0:/\n\r");
	}
	else {
		xil_printf("SD: mount 1:/\n\r");
		rc=f_mount(&fatfs, "1:/", 0);
	}

	xil_printf("SD: rc= %x\n\r", rc);

	if (rc != FR_OK) {
		Status = 1;
		xil_printf("XFSBL_ERROR_SD_INIT\n\r");
		goto END;
	}

	/**
	 * Create boot image name
	 */

	if(filename && fil) {
		rc = f_open(fil, filename, (BYTE)FA_READ);
		if (rc != FR_OK) {
			xil_printf("SD: Unable to open file %s: %d\n\r",
				   filename, rc);
			Status = -1;
			xil_printf("XFSBL_ERROR_SD_F_OPEN\n\r");
			goto END;
		}
	}
	else
	{
		Status = -1;
	}

	Status = 0;
END:
	return Status;
}

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
	       uint32_t Length)
{
	u32 Status;

	FRESULT rc;	 /* Result code */
	UINT br=0U;

	if (!fil)
		return -1;
	rc = f_lseek(fil, SrcAddress);
	if (rc != FR_OK) {
		xil_printf("SD: Unable to seek to %x\n", SrcAddress);
		Status = -1;
		xil_printf("XFSBL_ERROR_SD_F_LSEEK\n\r");
		goto END;
	}

	rc = f_read(fil, (void*)DestAddress, Length, &br);

	if (rc != FR_OK) {
		xil_printf("SD: f_read returned %d\r\n", rc);
		Status = -1;
		xil_printf("XFSBL_ERROR_SD_F_READ\n\r");
		goto END;
	}

	Status = 0;
END:
	return Status;
}

/*****************************************************************************/
/**
 * This function is used to release the sd settings
 *
 * @param	None
 *
 * @return	None
 *
 *****************************************************************************/
int xil_sdRelease(void *fil)
{
	if (fil)
		(void )f_close(fil);

	return 0;
}

