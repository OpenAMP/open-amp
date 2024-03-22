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
 *  \ingroup CSL_IP_MODULE
 *  \defgroup CSL_MAILBOX Mailbox
 *
 *  @{
 */
/**
 *  \file  mailbox.h
 *
 *  \brief This file contains the function prototypes for Mailbox access.
 */

#ifndef      MAILBOX_H_
#define      MAILBOX_H_

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "hw_types.h"
#include "cslr_mailbox.h"
#include "hw_mailbox.h"

/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */
/*
 * \name  User Id's Corresponds to the Core using the mailbox
 *        Mailbox Queue's Corresponds to the Mailbox Queue Number
 *  @{
 */
/**
 * \brief MAILBOX_QUEUE_0              - Mailbox message queue 0
 */
#define MAILBOX_QUEUE_0         0U
/**
 * \brief MAILBOX_QUEUE_1              - Mailbox message queue 1
 */
#define MAILBOX_QUEUE_1         1U
/**
 * \brief MAILBOX_QUEUE_2              - Mailbox message queue 2
 */
#define MAILBOX_QUEUE_2         2U
/**
 * \brief MAILBOX_QUEUE_3              - Mailbox message queue 3
 */
#define MAILBOX_QUEUE_3         3U
/**
 * \brief MAILBOX_QUEUE_4              - Mailbox message queue 4
 */
#define MAILBOX_QUEUE_4         4U
/**
 * \brief MAILBOX_QUEUE_5              - Mailbox message queue 5
 */
#define MAILBOX_QUEUE_5         5U
/**
 * \brief MAILBOX_QUEUE_6              - Mailbox message queue 6
 */
#define MAILBOX_QUEUE_6         6U
/**
 * \brief MAILBOX_QUEUE_7              - Mailbox message queue 7
 */
#define MAILBOX_QUEUE_7         7U
/**
 * \brief MAILBOX_QUEUE_8              - Mailbox message queue 8
 */
#define MAILBOX_QUEUE_8         8U
/**
 * \brief MAILBOX_QUEUE_9              - Mailbox message queue 9
 */
#define MAILBOX_QUEUE_9         9U
/**
 * \brief MAILBOX_QUEUE_10             - Mailbox message queue 10
 */
#define MAILBOX_QUEUE_10        10U
/**
 * \brief MAILBOX_QUEUE_11             - Mailbox message queue 11
 */
#define MAILBOX_QUEUE_11        11U
/**
 * \brief MAILBOX_QUEUE_12             - Mailbox message queue 12
 */
#define MAILBOX_QUEUE_12        12U
/**
 * \brief MAILBOX_QUEUE_13             - Mailbox message queue 13
 */
#define MAILBOX_QUEUE_13        13U
/**
 * \brief MAILBOX_QUEUE_14             - Mailbox message queue 14
 */
#define MAILBOX_QUEUE_14        14U
/**
 * \brief MAILBOX_QUEUE_15             - Mailbox message queue 15
 */
#define MAILBOX_QUEUE_15        15U
/* Incase of SOC_AM65XX, there are 64 queues; macros not provided for simplicity */
/* @} */

/*
 * \name Values that can be returned by MailboxGetMessage/MailboxSendMessage
 * to notify whether the message received/send is valid
 * @{
 */
/**
 * \brief MESSAGE_VALID       - Valid message
 */
#define MESSAGE_VALID           0U
/**
 * \brief MESSAGE_INVALID     - Invalid message
 */
#define MESSAGE_INVALID         1U
/* @} */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/*  Queue Access API's  */
/**
 * \brief   This function resets the mailbox
 *
 * @param   baseAddr    It is the Memory address of the Mailbox instance.
 *                      MAILBOXn (n = 1 to 10) Ex MAILBOX1, MAILBOX2, etc
 *                      EVEn_MLBm (n = 1 to 3, m = 1 to 4)
 *
 * @return  None
 */
void MailboxReset(uintptr_t baseAddr);

/**
 * \brief   This function configures the idle mode of the mailbox
 *
 * @param   baseAddr    It is the Memory address of the Mailbox instance.
 * @param   idleMode    Idle mode to be configured. Possible values are
 *                      0x0: Force-idle. An idle request is acknowledged
 *                           unconditionally
 *                      0x1: No-idle. An idle request is never acknowledged
 *                      0x2: Smart-idle. Acknowledgement to an idle request is
 *                           given based on the internal activity of the module
 *
 * @return  None
 */
void MailboxConfigIdleMode(uintptr_t baseAddr, uint32_t idleMode);

/**
 *  \brief  This function gets the first message in the queue
 *
 * @param   baseAddr    It is the Memory address of the Mailbox instance.
 * @param   queueId     Queue to be read
 * @param   *msgPtr     Message pointer in which the message will be returned
 *
 * @return  Validity    The return value indicates whether the message is valid
 */
uint32_t MailboxGetMessage(uintptr_t baseAddr, uint32_t queueId,
                           uint32_t *msgPtr);

/**
 *  \brief  This function writes message in the queue
 *
 * @param   baseAddr    It is the Memory address of the Mailbox instance.
 * @param   queueId     Queue to be written
 * @param   msg         Message to be sent
 *
 * @return  status      The return value indicates whether the message is
 *                      written to the queue. Possible values are,
 *                          0   -   Written successfully
 *                          0   -   Queue full
 */
uint32_t MailboxSendMessage(uintptr_t baseAddr, uint32_t queueId, uint32_t msg);

/*  Mailbox user(HW using mailbox) access API's */
/**
 * \brief   This function enables the new message interrupt for a user for given
 *          queue
 *
 * @param   baseAddr    It is the Memory address of the Mailbox instance.
 * @param   userId      User for whom the new message should be intimated
 * @param   queueId     Queue to be monitored for new message
 *
 * @return  None
 */
void MailboxEnableNewMsgInt(uintptr_t baseAddr, uint32_t userId,
                            uint32_t queueId);

/**
 * \brief   This function enables the queue not full interrupt for a user for
 *          given
 *  queue
 *
 * @param   baseAddr    It is the Memory address of the Mailbox instance.
 * @param   userId      User for whom the event should be intimated
 * @param   queueId     Queue to be monitored for non-full condition
 *
 * @return  None
 */
void MailboxEnableQueueNotFullInt(uintptr_t baseAddr, uint32_t userId,
                                  uint32_t queueId);

/**
 * \brief   This function disables the new message interrupt for a user for given
 *          queue
 *
 * @param   baseAddr    It is the Memory address of the Mailbox instance.
 * @param   userId      User for whom the new message event should be disabled
 * @param   queueId     Queue to be monitored for new message
 *
 * @return  None
 */
void MailboxDisableNewMsgInt(uintptr_t baseAddr, uint32_t userId,
                             uint32_t queueId);

/**
 * \brief   This function disables the queue not full interrupt for a user for
 *          given queue
 *
 * @param   baseAddr    It is the Memory address of the Mailbox instance.
 * @param   userId      User for whom the event should be disabled
 * @param   queueId     Queue for  which the non-full event to be disabled
 *
 * @return  None
 */
void MailboxDisableQueueNotFullInt(uintptr_t baseAddr, uint32_t userId,
                                   uint32_t queueId);

/**
 * \brief   This function clears the queue not-full status
 *
 * @param   baseAddr    It is the Memory address of the Mailbox instance.
 * @param   userId      User for whom the event should be cleared
 * @param   queueId     Queue for  which the event should be cleared
 *
 * @return  None
 */
void MailboxClrNewMsgStatus(uintptr_t baseAddr, uint32_t userId,
                            uint32_t queueId);

/**
 * \brief   This function clears the queue not-full status
 *
 * @param   baseAddr    It is the Memory address of the Mailbox instance.
 * @param   userId      User for whom the event should be cleared
 * @param   queueId     Queue for  which the event should be cleared
 *
 * @return  None
 */
void MailboxClrQueueNotFullStatus(uintptr_t baseAddr, uint32_t userId,
                                  uint32_t queueId);

/**
 * \brief   This function gets the raw new message status
 *
 * @param   baseAddr    It is the Memory address of the Mailbox instance.
 * @param   userId      User for whom the event should be checked
 * @param   queueId     Queue for  which the event should be checked
 *
 * @return  status      status of new message
 */
uint32_t MailboxGetRawNewMsgStatus(uintptr_t baseAddr, uint32_t userId,
                                   uint32_t queueId);

/**
 * \brief   This function gets the raw queue not-full status
 *
 * @param   baseAddr    It is the Memory address of the Mailbox instance.
 * @param   userId      User for whom the event should be checked
 * @param   queueId     Queue for  which the event should be checked
 *
 * @return  status      Queue not full status
 */
uint32_t MailboxGetRawQueueNotFullStatus(uintptr_t baseAddr, uint32_t userId,
                                         uint32_t queueId);

/**
 * \brief   This function gets IRQ enable status
 *
 * @param   baseAddr    It is the Memory address of the Mailbox instance.
 * @param   userId      User for whom the event should be checked
 * @param   queueId     Queue for  which the event should be checked
 *
 * @return  status      Queue not full status
 */
uint32_t MailboxGetIrqEnableStatus(uintptr_t baseAddr, uint32_t userId,
                                   uint32_t queueId);
/**
 * \brief   This function gets message count in the mailbox
 *
 * @param   baseAddr    It is the Memory address of the Mailbox instance.
 * @param   queueId     Mailbox FIFO id
 *
 * @return  status      Queue not full status
 */
uint32_t MailboxGetMessageCount(uintptr_t baseAddr, uint32_t queueId);

/**
 * \brief   This function writes mailbox register
 *
 * @param   baseAddr    It is the Memory address of the Mailbox instance.
 * @param   queueId     Mailbox FIFO id
 * @param   msg         value to be written to mailbox
 *
 * @return  None
 */
void MailboxWriteMessage(uintptr_t baseAddr, uint32_t queueId, uint32_t msg);

/**
 * \brief   This function writes EOI register
 *
 * @param   baseAddr    It is the Memory address of the Mailbox instance.
 * @param   value       User for whom the event should be checked
 *
 * @return  None
 */
void MailboxWriteEOI(uintptr_t baseAddr, uint32_t value);

/**
 * \brief   This function reads mailbox register
 *
 * @param   baseAddr    It is the Memory address of the Mailbox instance.
 * @param   queueId     Mailbox FIFO id
 * @param   msgPtr      Message pointer in which the message will be returned
 *
 * @return  None
 */
void MailboxReadMessage(uintptr_t baseAddr, uint32_t queueId,
                        uint32_t *msgPtr);

#ifdef __cplusplus
}
#endif
#endif
 /** @} */
/********************************* End of file ******************************/
