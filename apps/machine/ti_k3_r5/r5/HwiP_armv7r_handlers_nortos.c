/*
 *  Copyright (C) 2018-2023 Texas Instruments Incorporated
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

#include <stddef.h>

#include <kernel/dpl/HwiP.h>
#include <kernel/dpl/HwiP_armv7r_vim.h>

void __attribute__((interrupt("SWI"), section(".text.hwi"))) HwiP_svc_handler(void);

/* compile flag to enable or disable interrupt nesting */
#define HWIP_NESTED_INTERRUPTS_IRQ_ENABLE

/* IRQ handler starts execution in HwiP_irq_handler, defined in HwiP_armv7r_handlers_nortos_asm.S
 * After some initial assembly logic it then branches to this function.
 * After exiting this function it does some more assembly before exiting
 */
void __attribute__((section(".text.hwi"))) HwiP_irq_handler_c(void)
{
    int32_t status;
    uint32_t intNum;

    #ifndef HWIP_VIM_VIC_ENABLE
    volatile uint32_t dummy;

    /* Read to force prioritization logic to take effect, in non-VIC mode */
    dummy = HwiP_getIRQVecAddr();
    #endif

    status = HwiP_getIRQ(&intNum);
    if(status==SystemP_SUCCESS)
    {
        uint32_t isPulse = HwiP_isPulse(intNum);
        HwiP_FxnCallback isr;
        void *args;

        if(isPulse!=0U)
        {
            HwiP_clearInt(intNum);
        }

        isr = gHwiCtrl.isr[intNum];
        args = gHwiCtrl.isrArgs[intNum];

        #ifdef HWIP_NESTED_INTERRUPTS_IRQ_ENABLE
        /* allow nesting of interrupts */
        HwiP_enable();
        #endif

        if(isr!=NULL)
        {
            isr(args);
        }

        /* disallow nesting of interrupts */
        (void) HwiP_disable();

        if(isPulse==0U)
        {
            HwiP_clearInt(intNum);
        }
        HwiP_ackIRQ(intNum);
    }
    else
    {
        /* spurious interrupt */
        gHwiCtrl.spuriousIRQCount++;
        HwiP_ackIRQ(0);
    }
}

void __attribute__((interrupt("FIQ"), section(".text.hwi"))) HwiP_fiq_handler(void)
{
    int32_t status;
    uint32_t intNum;
    volatile uint32_t dummy;

    /* Read to force prioritization logic to take effect */
    dummy = HwiP_getFIQVecAddr();
    (void)dummy;

    status = HwiP_getFIQ(&intNum);
    if(status==SystemP_SUCCESS)
    {
        uint32_t isPulse = HwiP_isPulse(intNum);
        HwiP_FxnCallback isr;
        void *args;

        if(isPulse!=0U)
        {
            HwiP_clearInt(intNum);
        }

        isr = gHwiCtrl.isr[intNum];
        args = gHwiCtrl.isrArgs[intNum];

        #if 0   /* FIQ interrupt nesting not supported */
        /* allow nesting of interrupts */
        HwiP_enableFIQ();
        #endif

        if(isr!=NULL)
        {
            isr(args);
        }

        /* disallow nesting of interrupts */
        (void) HwiP_disableFIQ();

        if(isPulse==0U)
        {
            HwiP_clearInt(intNum);
        }
        HwiP_ackFIQ(intNum);
    }
    else
    {
        /* spurious interrupt */
        gHwiCtrl.spuriousFIQCount++;
        HwiP_ackFIQ(0);
    }
}

void __attribute__((interrupt("UNDEF"), section(".text.hwi"))) HwiP_reserved_handler(void)
{
    volatile uint32_t loop = 1;
    while(loop!=0U)
    {
        ;
    }
}

void __attribute__((interrupt("UNDEF"), section(".text.hwi"))) HwiP_undefined_handler(void)
{
    volatile uint32_t loop = 1;
    while(loop!=0U)
    {
        ;
    }
}

void __attribute__((interrupt("SWI"), section(".text.hwi"))) HwiP_svc_handler(void)
{
    volatile uint32_t loop = 1;
    while(loop!=0U)
    {
        ;
    }

}

void __attribute__((interrupt("ABORT"), section(".text.hwi"))) HwiP_prefetch_abort_handler(void)
{
    volatile uint32_t loop = 1;
    while(loop!=0U)
    {
        ;
    }
}

void __attribute__((interrupt("ABORT"), section(".text.hwi"),weak)) HwiP_data_abort_handler_c(void)
{
    volatile uint32_t loop = 1;
    while(loop!=0U)
    {
        ;
    }
}
