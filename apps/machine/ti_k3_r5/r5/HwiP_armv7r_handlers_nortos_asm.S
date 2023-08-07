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

/* make this 0 to disable FPU context save/restore */
#define ENABLE_FPU_SAVE_RESTORE (0)

        .text
        .arm
        .align 2

        .set SVC_MODE,  0x13
        .set IRQ_MODE,  0x12

/* FUNCTION DEF: void HwiP_irq_handler(void) */
        .global HwiP_irq_handler
        .type HwiP_irq_handler,%function
        .section ".text.hwi","ax",%progbits
        .arm
        .align 2
HwiP_irq_handler:
        /* Return to the interrupted instruction. */
        SUB             lr, lr, #4

        /* Push the return address and SPSR. */
        PUSH    {lr}
        MRS     lr, SPSR
        PUSH    {lr}

        /* Change to supervisor mode to allow reentry. */
        CPS  #SVC_MODE

        /* Push used registers. */
        PUSH    {r0-r4, r12}

        #if ENABLE_FPU_SAVE_RESTORE
        FMRX  R0, FPSCR
        VPUSH {D0-D15}
        /* VPUSH {D16-D31} */
        PUSH  {R0}
        #endif

        /* Ensure bit 2 of the stack pointer is clear.  r2 holds the bit 2 value for
         * future use.  _RB_ Is this ever needed provided the start of the stack is
         * alligned on an 8-byte boundary?
         */
        MOV     r2, sp
        AND     r2, r2, #4
        SUB     sp, sp, r2

        /* Call the interrupt handler. */
        PUSH    {r0-r4, lr}
        LDR     r1, HwiP_irq_handler_const
        BLX     r1
        POP     {r0-r4, lr}
        ADD     sp, sp, r2

        /* disable IRQ */
        CPSID   i
        DSB
        ISB

        #if ENABLE_FPU_SAVE_RESTORE
        POP   {R0}
        /* VPOP {D16-D31} */
        VPOP  {D0-D15}
        VMSR  FPSCR, R0
        #endif

        POP     {r0-r4, r12}
        CPS  #IRQ_MODE
        POP     {LR}
        MSR     SPSR_cxsf, LR
        POP     {LR}
        MOVS    PC, LR

/* FUNCTION DEF: void HwiP_data_abort_handler(void) */
        .global HwiP_data_abort_handler
        .type HwiP_data_abort_handler,%function
        .section ".text.hwi","ax",%progbits
        .arm
        .align 2
HwiP_data_abort_handler:
        /* Return to the instruction following the interrupted.
         * The instruction that caused the data abort is at R14_ABT – 8 which means
         * that the pointer points two instructions beyond the instruction that caused
         * the abort. (pc - 4) in arm mode and (pc - 6) in thumb mode (2 bytes aligned
         * instruction set) points to the second instruction beyond the address where
         * the exception was generated.
         */
        SUB		lr, lr, #6

        /*   Push the return address and SPSR. */
        PUSH	{lr}
        MRS	lr, SPSR
        PUSH	{lr}

        /*   Push used registers. */
        PUSH	{r0-r4, r12}

        /*   Call the interrupt handler. */
        LDR	r1, HwiP_data_abort_handler_const
        BLX	r1

        /*  Restore used registers, LR and SPSR before  returning. */
        POP	{r0-r4, r12}
        POP	{LR}
        MSR	SPSR_cxsf, LR
        POP	{LR}
        MOVS	PC, LR

HwiP_irq_handler_const: .word HwiP_irq_handler_c
HwiP_data_abort_handler_const: .word HwiP_data_abort_handler_c

        .end
