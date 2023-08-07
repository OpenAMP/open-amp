/*
 *  Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
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
 *
 */
/**
 *  \file  mailbox.c
 *
 *  \brief Mailbox Device Abstraction Layer APIs
 *
 *   This file contains the device abstraction layer APIs for the
 *   mailbox module. These are used for IPC communication.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
/** \brief This is to disable HW_SYNC_BARRIER for register access */
#define MEM_BARRIER_DISABLE

#include "stdint.h"
#include "mailbox.h"

/* ========================================================================== */
/*                          Function Definitions                             */
/* ========================================================================== */
void MailboxReset(uintptr_t baseAddr)
{
    /*    Start the soft reset sequence    */
    /* write SOFTRESET field */
    CSL_REG32_FINS(baseAddr + CSL_MAILBOX_SYSCONFIG, MAILBOX_SYSCONFIG_SOFT_RESET,
                  MAILBOX_SYSCONFIG_SOFTRESET_B1);

    /*    Wait till the reset is complete    */
    while (MAILBOX_SYSCONFIG_SOFTRESET_B1 ==
           CSL_REG32_FEXT(baseAddr + MAILBOX_SYSCONFIG,
                         MAILBOX_SYSCONFIG_SOFT_RESET))
    {
        /* Do nothing - Busy wait */
    }

    return;
}

void MailboxConfigIdleMode(uintptr_t baseAddr, uint32_t idleMode)
{
#if defined(SOC_AM572x)|| defined(SOC_AM571x) || defined(SOC_TDA2XX) || \
    defined(SOC_DRA7XX) || defined(SOC_TDA2EX) || defined(SOC_TDA3XX)
    /*    Configure idle mode    */
    HW_WR_FIELD32(baseAddr + MAILBOX_SYSCONFIG, MAILBOX_SYSCONFIG_SIDLEMODE,
                  idleMode);
#else
    (void) baseAddr;
    (void) idleMode;
#endif
}

uint32_t MailboxGetMessage(uintptr_t baseAddr, uint32_t queueId,
                           uint32_t *msgPtr)
{
    uint32_t msgCount;
    uint32_t retval;

    msgCount = MailboxGetMessageCount(baseAddr, queueId);

    if (msgCount > 0U)
    {
        /*    Read message    */
        *msgPtr = CSL_REG32_RD(baseAddr + CSL_MAILBOX_MESSAGE(queueId));
        retval  = MESSAGE_VALID;
    }
    else
    {
        /*    Queue empty*/
        retval = MESSAGE_INVALID;
    }

    return retval;
}

void MailboxReadMessage(uintptr_t baseAddr, uint32_t queueId,
                        uint32_t *msgPtr)
{
        /*    Read message    */
        *msgPtr = CSL_REG32_RD(baseAddr + CSL_MAILBOX_MESSAGE(queueId));
}

uint32_t MailboxSendMessage(uintptr_t baseAddr, uint32_t queueId, uint32_t msg)
{
    uint32_t fifoFull;
    uint32_t retval;

    /* Read the FIFO Status */
    fifoFull = CSL_REG32_RD(baseAddr + CSL_MAILBOX_FIFO_STATUS(queueId));
    if (fifoFull == 0U)
    {
        /* FIFO not full write msg */
        MailboxWriteMessage(baseAddr, queueId, msg);
        retval = MESSAGE_VALID;
    }
    else
    {
        retval = MESSAGE_INVALID;
    }

    return retval;
}

void MailboxEnableNewMsgInt(uintptr_t baseAddr, uint32_t userId,
                            uint32_t queueId)
{
    /* Set the NewMsgEnable field - Writing zero has no effect */
    CSL_REG32_WR(baseAddr + CSL_MAILBOX_USER_IRQ_ENABLE_SET(userId), (uint32_t) 0x1 <<
                (queueId * 2U));
}

void MailboxEnableQueueNotFullInt(uintptr_t baseAddr, uint32_t userId,
                                  uint32_t queueId)
{
    /* Set the FIFO Not Full field - Writing zero has no effect */
    CSL_REG32_WR(baseAddr + CSL_MAILBOX_USER_IRQ_ENABLE_SET(userId), (uint32_t) 0x2 <<
                (queueId * 2U));
}

void MailboxDisableNewMsgInt(uintptr_t baseAddr, uint32_t userId,
                             uint32_t queueId)
{
    /* Set the NewMsgEnable field - Writing zero has no effect */
    CSL_REG32_WR(baseAddr + CSL_MAILBOX_USER_IRQ_ENABLE_CLR(userId), (uint32_t) 0x1 <<
                (queueId * 2U));
}

void MailboxDisableQueueNotFullInt(uintptr_t baseAddr, uint32_t userId,
                                   uint32_t queueId)
{
    /* Set the FIFO Not Full field - Writing zero has no effect */
    CSL_REG32_WR(baseAddr + CSL_MAILBOX_USER_IRQ_ENABLE_CLR(userId), (uint32_t) 0x2 <<
                (queueId * 2U));
}

void MailboxClrNewMsgStatus(uintptr_t baseAddr, uint32_t userId,
                            uint32_t queueId)
{
    /* Set the NewMsgEnable field - Writing zero has no effect */
    CSL_REG32_WR(baseAddr + CSL_MAILBOX_USER_IRQ_STATUS_CLR(userId), (uint32_t) 0x1 <<
                (queueId * 2U));
}

void MailboxClrQueueNotFullStatus(uintptr_t baseAddr, uint32_t userId,
                                  uint32_t queueId)
{
    /* Set the NewMsgEnable field - Writing zero has no effect */
    CSL_REG32_WR(baseAddr + CSL_MAILBOX_USER_IRQ_STATUS_CLR(userId), (uint32_t) 0x2 <<
                (queueId * 2U));
}

uint32_t MailboxGetRawNewMsgStatus(uintptr_t baseAddr,
                                   uint32_t userId,
                                   uint32_t queueId)
{
    uint32_t regVal;

    /* Read the IRQSTATUSRAW */
    regVal = CSL_REG32_RD(baseAddr + CSL_MAILBOX_USER_IRQ_STATUS_RAW(userId));

    /* Mask & read the NewMsgStatus for given queueId */
    regVal &= (uint32_t) 0x1 << (queueId * 2U);

    return (regVal >> (queueId * 2U));
}

uint32_t MailboxGetRawQueueNotFullStatus(uintptr_t baseAddr,
                                         uint32_t userId,
                                         uint32_t queueId)
{
    uint32_t regVal;

    /* Read the IRQSTATUSRAW */
    regVal = CSL_REG32_RD(baseAddr + CSL_MAILBOX_USER_IRQ_STATUS_RAW(userId));

    /* Mask & read the FIFO Not full for given queueId */
    regVal &= (uint32_t) 0x2 << (queueId * 2U);

    return (regVal >> (queueId * 2U));
}

uint32_t MailboxGetIrqEnableStatus(uintptr_t baseAddr,
                                   uint32_t userId,
                                   uint32_t queueId)
{
    uint32_t regVal;

    /* Read the IRQENABLESET */
    regVal = CSL_REG32_RD(baseAddr + CSL_MAILBOX_USER_IRQ_ENABLE_SET(userId));

    /* Mask & read the FIFO Not full for given queueId */
    regVal &= (uint32_t) 0x1 << (queueId * 2U);

    return (regVal);
}

uint32_t MailboxGetMessageCount(uintptr_t baseAddr,
                                uint32_t queueId)
{
    /* Return message count */
    return (CSL_REG32_RD(baseAddr + CSL_MAILBOX_MSG_STATUS(queueId)));
}

void MailboxWriteMessage(uintptr_t baseAddr, uint32_t queueId, uint32_t msg)
{
    /* Write mailbox message */
    CSL_REG32_WR(baseAddr + CSL_MAILBOX_MESSAGE(queueId), msg);
}

void MailboxWriteEOI(uintptr_t baseAddr, uint32_t value)
{
    /* Write mailbox message */
    CSL_REG32_WR(baseAddr + CSL_MAILBOX_IRQ_EOI, value);
}
/********************************* End of file ******************************/
