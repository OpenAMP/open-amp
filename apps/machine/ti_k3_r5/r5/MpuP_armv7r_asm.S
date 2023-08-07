/*
 *  Copyright (C) 2018-2021 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


        .text

/* FUNCTION DEF: void MpuP_disableAsm(void) */
        .global MpuP_disableAsm
        .type MpuP_disableAsm,%function
        .section ".text.mpu","ax",%progbits
        .arm
        .align 2
MpuP_disableAsm:
        mrc     p15, #0, r0, c1, c0, #0  // read SCTLR register
        bic     r0, r0, #0x1             // clear bit 0 in r0
        dsb
        mcr     p15, #0, r0, c1, c0, #0  // MPU disabled (bit 0 = 0)
        isb                              // flush instruction pipeline
        bx      LR

/* FUNCTION DEF: void MpuP_disableBRAsm(void) */
        .global MpuP_disableBRAsm
        .type MpuP_disableBRAsm,%function
        .section ".text.mpu","ax",%progbits
        .arm
        .align 2
MpuP_disableBRAsm:
        mrc     p15, #0, r0, c1, c0, #0  // read SCTLR register
        bic     r0, r0, #0x20000         // clear bit 17 in r0
        mcr     p15, #0, r0, c1, c0, #0  // disable background region
        bx      LR

/* FUNCTION DEF: void MpuP_enableAsm(void) */
        .global MpuP_enableAsm
        .type MpuP_enableAsm,%function
        .section ".text.mpu","ax",%progbits
        .arm
        .align 2
MpuP_enableAsm:
        mrc     p15, #0, r0, c1, c0, #0  // read SCTLR register
        orr     r0, r0, #0x1             // set bit 0 in r0
        dsb
        mcr     p15, #0, r0, c1, c0, #0  // MPU enabled (bit 0 = 1)
        isb                              // flush instruction pipeline
        bx      LR

/* FUNCTION DEF: void MpuP_enableBRAsm(void) */
        .global MpuP_enableBRAsm
        .type MpuP_enableBRAsm,%function
        .section ".text.mpu","ax",%progbits
        .arm
        .align 2
MpuP_enableBRAsm:
        mrc     p15, #0, r0, c1, c0, #0  // read SCTLR register
        orr     r0, r0, #0x20000         // set bit 17 in r0
        mcr     p15, #0, r0, c1, c0, #0  // background region enabled
        bx      LR

/* FUNCTION DEF: uint32_t MpuP_isEnableAsm(void) */
        .global MpuP_isEnableAsm
        .type MpuP_isEnableAsm,%function
        .section ".text.mpu","ax",%progbits
        .arm
        .align 2
MpuP_isEnableAsm:
        mov     r0, #0
        mrc     p15, #0, r1, c1, c0, #0  // read SCTLR register to r1
        tst     r1, #0x1                 // test bit 0
        movne   r0, #1                   // if not 0, MPU is enabled
        bx      LR

/* FUNCTION DEF:
 *       void MpuP_setRegionAsm(
 *              uint32_t regionId, uint32_t regionBaseAddr, uint32_t sizeAndEnble, uint32_t regionAttrs)
 *
 * r0 = regionId
 * r1 = regionBaseAddr
 * r2 = sizeAndEnable
 * r3 = regionAttrs
 */
        .global MpuP_setRegionAsm
        .type MpuP_setRegionAsm,%function
        .section ".text.mpu","ax",%progbits
        .arm
        .align 2
MpuP_setRegionAsm:
        mcr     p15, #0, r0, c6, c2, #0  // select MPU region
        mcr     p15, #0, r1, c6, c1, #0  // set region base address
        mcr     p15, #0, r2, c6, c1, #2  // set region size and enable it
        mcr     p15, #0, r3, c6, c1, #4  // set protection attributes
        bx      LR

	.end

