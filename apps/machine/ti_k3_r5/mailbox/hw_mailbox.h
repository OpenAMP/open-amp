/* =============================================================================
 *   Copyright (c) Texas Instruments Incorporated 2014
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
*
*  \file   hw_mailbox.h
*
*  \brief  register-level header file for MAILBOX
*
**/

#ifndef HW_MAILBOX_H_
#define HW_MAILBOX_H_

#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************************************************************
* Register Definitions
****************************************************************************************************/
#define MAILBOX_REVISION                                                                                   (0x0U)
#define MAILBOX_SYSCONFIG                                                                                  (0x10U)
#define MAILBOX_MESSAGE(n)                                                                                 ((uintptr_t)0x40U + ((n) * 4U))
#define MAILBOX_FIFOSTATUS(n)                                                                              ((uintptr_t)0x80U + ((n) * 4U))
#define MAILBOX_MSGSTATUS(n)                                                                               ((uintptr_t)0xc0U + ((n) * 4U))
#define MAILBOX_IRQSTATUS_RAW(n)                                                                           ((uintptr_t)0x100U + ((n) * 16U))
#define MAILBOX_IRQSTATUS_CLR(n)                                                                           ((uintptr_t)0x104U + ((n) * 16U))
#define MAILBOX_IRQENABLE_CLR(n)                                                                           ((uintptr_t)0x10cU + ((n) * 16U))
#define MAILBOX_IRQENABLE_SET(n)                                                                           ((uintptr_t)0x108U + ((n) * 16U))
#define MAILBOX_IRQ_EOI                                                                                    (0x140U)

/****************************************************************************************************
* Field Definition Macros
****************************************************************************************************/

/* REVISION */

#define MAILBOX_REVISION_SCHEME_SHIFT                                                                     (0x1EU)
#define MAILBOX_REVISION_SCHEME_MASK                                                                      (0xC0000000U)

#define MAILBOX_REVISION_BU_SHIFT                                                                         (0x1CU)
#define MAILBOX_REVISION_BU_MASK                                                                          (0x30000000U)

#define MAILBOX_REVISION_FUNCTION_SHIFT                                                                   (0x10U)
#define MAILBOX_REVISION_FUNCTION_MASK                                                                    (0x0FFF0000U)

#define MAILBOX_REVISION_RTL_VER_SHIFT                                                                    (0xBU)
#define MAILBOX_REVISION_RTL_VER_MASK                                                                     (0x0000F800U)

#define MAILBOX_REVISION_MAJOR_REV_SHIFT                                                                  (0x8U)
#define MAILBOX_REVISION_MAJOR_REV_MASK                                                                   (0x00000700U)

#define MAILBOX_REVISION_CUSTOM_SHIFT                                                                     (0x6U)
#define MAILBOX_REVISION_CUSTOM_MASK                                                                      (0x000000C0U)

#define MAILBOX_REVISION_MINOR_REV_SHIFT                                                                  (0x0U)
#define MAILBOX_REVISION_MINOR_REV_MASK                                                                   (0x0000003FU)

#define MAILBOX_SYSCONFIG_SOFTRESET_SHIFT                                                                 (0U)
#define MAILBOX_SYSCONFIG_SOFTRESET_MASK                                                                  (0x00000001U)
#define MAILBOX_SYSCONFIG_SOFTRESET_B0                                                                    (0U)
#define MAILBOX_SYSCONFIG_SOFTRESET_B1                                                                    (1U)

#define MAILBOX_IRQ_EOI_EOI0_SHIFT                                                                        (0U)
#define MAILBOX_IRQ_EOI_EOI0_MASK                                                                         (0x00000001U)

#define MAILBOX_IRQ_EOI_EOI1_SHIFT                                                                        (0x1U)
#define MAILBOX_IRQ_EOI_EOI1_MASK                                                                         (0x00000002U)

#define MAILBOX_IRQ_EOI_EOI2_SHIFT                                                                        (0x2U)
#define MAILBOX_IRQ_EOI_EOI2_MASK                                                                         (0x00000004U)

#define MAILBOX_IRQ_EOI_EOI3_SHIFT                                                                        (0x3U)
#define MAILBOX_IRQ_EOI_EOI3_MASK                                                                         (0x00000008U)

#define MAILBOX_MESSAGE_MESSAGEVALUEMBM_SHIFT                                                             (0U)
#define MAILBOX_MESSAGE_MESSAGEVALUEMBM_MASK                                                              (0xffffffffU)

#define MAILBOX_FIFOSTATUS_FIFOFULLMBM_SHIFT                                                              (0U)
#define MAILBOX_FIFOSTATUS_FIFOFULLMBM_MASK                                                               (0x00000001U)

#define MAILBOX_FIFOSTATUS_RESERVED_0_SHIFT                                                               (1U)
#define MAILBOX_FIFOSTATUS_RESERVED_0_MASK                                                                (0xfffffffeU)

#define MAILBOX_MSGSTATUS_NBOFMSGMBM_SHIFT                                                                (0U)
#define MAILBOX_MSGSTATUS_NBOFMSGMBM_MASK                                                                 (0x0000003fU)

#define MAILBOX_MSGSTATUS_RESERVED_SHIFT                                                                  (7U)
#define MAILBOX_MSGSTATUS_RESERVED_MASK                                                                   (0xffffff80U)

#define MAILBOX_IRQSTATUS_RAW_NEWMSGSTATUSUUMB0_SHIFT                                                     (0U)
#define MAILBOX_IRQSTATUS_RAW_NEWMSGSTATUSUUMB0_MASK                                                      (0x00000001U)

#define MAILBOX_IRQSTATUS_RAW_NEWMSGSTATUSUUMB1_SHIFT                                                     (2U)
#define MAILBOX_IRQSTATUS_RAW_NEWMSGSTATUSUUMB1_MASK                                                      (0x00000004U)

#define MAILBOX_IRQSTATUS_RAW_NOTFULLSTATUSUUMB1_SHIFT                                                    (3U)
#define MAILBOX_IRQSTATUS_RAW_NOTFULLSTATUSUUMB1_MASK                                                     (0x00000008U)

#define MAILBOX_IRQSTATUS_RAW_NOTFULLSTATUSUUMB0_SHIFT                                                    (1U)
#define MAILBOX_IRQSTATUS_RAW_NOTFULLSTATUSUUMB0_MASK                                                     (0x00000002U)

#define MAILBOX_IRQSTATUS_RAW_NEWMSGSTATUSUUMB2_SHIFT                                                     (4U)
#define MAILBOX_IRQSTATUS_RAW_NEWMSGSTATUSUUMB2_MASK                                                      (0x00000010U)

#define MAILBOX_IRQSTATUS_RAW_NOTFULLSTATUSUUMB2_SHIFT                                                    (5U)
#define MAILBOX_IRQSTATUS_RAW_NOTFULLSTATUSUUMB2_MASK                                                     (0x00000020U)

#define MAILBOX_IRQSTATUS_RAW_NEWMSGSTATUSUUMB3_SHIFT                                                     (6U)
#define MAILBOX_IRQSTATUS_RAW_NEWMSGSTATUSUUMB3_MASK                                                      (0x00000040U)

#define MAILBOX_IRQSTATUS_RAW_NOTFULLSTATUSUUMB3_SHIFT                                                    (7U)
#define MAILBOX_IRQSTATUS_RAW_NOTFULLSTATUSUUMB3_MASK                                                     (0x00000080U)

#define MAILBOX_IRQSTATUS_RAW_NEWMSGSTATUSUUMB4_SHIFT                                                     (8U)
#define MAILBOX_IRQSTATUS_RAW_NEWMSGSTATUSUUMB4_MASK                                                      (0x00000100U)

#define MAILBOX_IRQSTATUS_RAW_NOTFULLSTATUSUUMB4_SHIFT                                                    (9U)
#define MAILBOX_IRQSTATUS_RAW_NOTFULLSTATUSUUMB4_MASK                                                     (0x00000200U)

#define MAILBOX_IRQSTATUS_RAW_NEWMSGSTATUSUUMB5_SHIFT                                                     (10U)
#define MAILBOX_IRQSTATUS_RAW_NEWMSGSTATUSUUMB5_MASK                                                      (0x00000400U)

#define MAILBOX_IRQSTATUS_RAW_NOTFULLSTATUSUUMB5_SHIFT                                                    (11U)
#define MAILBOX_IRQSTATUS_RAW_NOTFULLSTATUSUUMB5_MASK                                                     (0x00000800U)

#define MAILBOX_IRQSTATUS_RAW_NOTFULLSTATUSUUMB6_SHIFT                                                    (13U)
#define MAILBOX_IRQSTATUS_RAW_NOTFULLSTATUSUUMB6_MASK                                                     (0x00002000U)

#define MAILBOX_IRQSTATUS_RAW_NEWMSGSTATUSUUMB6_SHIFT                                                     (12U)
#define MAILBOX_IRQSTATUS_RAW_NEWMSGSTATUSUUMB6_MASK                                                      (0x00001000U)

#define MAILBOX_IRQSTATUS_RAW_NEWMSGSTATUSUUMB7_SHIFT                                                     (14U)
#define MAILBOX_IRQSTATUS_RAW_NEWMSGSTATUSUUMB7_MASK                                                      (0x00004000U)

#define MAILBOX_IRQSTATUS_RAW_NOTFULLSTATUSUUMB7_SHIFT                                                    (15U)
#define MAILBOX_IRQSTATUS_RAW_NOTFULLSTATUSUUMB7_MASK                                                     (0x00008000U)

#define MAILBOX_IRQSTATUS_RAW_RESERVED_SHIFT                                                              (16U)
#define MAILBOX_IRQSTATUS_RAW_RESERVED_MASK                                                               (0xffff0000U)

#define MAILBOX_IRQSTATUS_CLR_NOTFULLSTATUSENUUMB0_SHIFT                                                  (1U)
#define MAILBOX_IRQSTATUS_CLR_NOTFULLSTATUSENUUMB0_MASK                                                   (0x00000002U)

#define MAILBOX_IRQSTATUS_CLR_NOTFULLSTATUSENUUMB1_SHIFT                                                  (3U)
#define MAILBOX_IRQSTATUS_CLR_NOTFULLSTATUSENUUMB1_MASK                                                   (0x00000008U)

#define MAILBOX_IRQSTATUS_CLR_NOTFULLSTATUSENUUMB2_SHIFT                                                  (5U)
#define MAILBOX_IRQSTATUS_CLR_NOTFULLSTATUSENUUMB2_MASK                                                   (0x00000020U)

#define MAILBOX_IRQSTATUS_CLR_NOTFULLSTATUSENUUMB3_SHIFT                                                  (7U)
#define MAILBOX_IRQSTATUS_CLR_NOTFULLSTATUSENUUMB3_MASK                                                   (0x00000080U)

#define MAILBOX_IRQSTATUS_CLR_NOTFULLSTATUSENUUMB4_SHIFT                                                  (9U)
#define MAILBOX_IRQSTATUS_CLR_NOTFULLSTATUSENUUMB4_MASK                                                   (0x00000200U)

#define MAILBOX_IRQSTATUS_CLR_NOTFULLSTATUSENUUMB5_SHIFT                                                  (11U)
#define MAILBOX_IRQSTATUS_CLR_NOTFULLSTATUSENUUMB5_MASK                                                   (0x00000800U)

#define MAILBOX_IRQSTATUS_CLR_NOTFULLSTATUSENUUMB6_SHIFT                                                  (13U)
#define MAILBOX_IRQSTATUS_CLR_NOTFULLSTATUSENUUMB6_MASK                                                   (0x00002000U)

#define MAILBOX_IRQSTATUS_CLR_NOTFULLSTATUSENUUMB7_SHIFT                                                  (15U)
#define MAILBOX_IRQSTATUS_CLR_NOTFULLSTATUSENUUMB7_MASK                                                   (0x00008000U)

#define MAILBOX_IRQSTATUS_CLR_NEWMSGSTATUSENUUMB0_SHIFT                                                   (0U)
#define MAILBOX_IRQSTATUS_CLR_NEWMSGSTATUSENUUMB0_MASK                                                    (0x00000001U)

#define MAILBOX_IRQSTATUS_CLR_NEWMSGSTATUSENUUMB1_SHIFT                                                   (2U)
#define MAILBOX_IRQSTATUS_CLR_NEWMSGSTATUSENUUMB1_MASK                                                    (0x00000004U)

#define MAILBOX_IRQSTATUS_CLR_NEWMSGSTATUSENUUMB2_SHIFT                                                   (4U)
#define MAILBOX_IRQSTATUS_CLR_NEWMSGSTATUSENUUMB2_MASK                                                    (0x00000010U)

#define MAILBOX_IRQSTATUS_CLR_NEWMSGSTATUSENUUMB3_SHIFT                                                   (6U)
#define MAILBOX_IRQSTATUS_CLR_NEWMSGSTATUSENUUMB3_MASK                                                    (0x00000040U)

#define MAILBOX_IRQSTATUS_CLR_NEWMSGSTATUSENUUMB4_SHIFT                                                   (8U)
#define MAILBOX_IRQSTATUS_CLR_NEWMSGSTATUSENUUMB4_MASK                                                    (0x00000100U)

#define MAILBOX_IRQSTATUS_CLR_NEWMSGSTATUSENUUMB5_SHIFT                                                   (10U)
#define MAILBOX_IRQSTATUS_CLR_NEWMSGSTATUSENUUMB5_MASK                                                    (0x00000400U)

#define MAILBOX_IRQSTATUS_CLR_NEWMSGSTATUSENUUMB6_SHIFT                                                   (12U)
#define MAILBOX_IRQSTATUS_CLR_NEWMSGSTATUSENUUMB6_MASK                                                    (0x00001000U)

#define MAILBOX_IRQSTATUS_CLR_NEWMSGSTATUSENUUMB7_SHIFT                                                   (14U)
#define MAILBOX_IRQSTATUS_CLR_NEWMSGSTATUSENUUMB7_MASK                                                    (0x00004000U)

#define MAILBOX_IRQSTATUS_CLR_RESERVED_SHIFT                                                              (16U)
#define MAILBOX_IRQSTATUS_CLR_RESERVED_MASK                                                               (0xffff0000U)

#define MAILBOX_IRQENABLE_CLR_NEWMSGENABLEUUMB0_SHIFT                                                     (0U)
#define MAILBOX_IRQENABLE_CLR_NEWMSGENABLEUUMB0_MASK                                                      (0x00000001U)

#define MAILBOX_IRQENABLE_CLR_NEWMSGENABLEUUMB1_SHIFT                                                     (2U)
#define MAILBOX_IRQENABLE_CLR_NEWMSGENABLEUUMB1_MASK                                                      (0x00000004U)

#define MAILBOX_IRQENABLE_CLR_NEWMSGENABLEUUMB5_SHIFT                                                     (10U)
#define MAILBOX_IRQENABLE_CLR_NEWMSGENABLEUUMB5_MASK                                                      (0x00000400U)

#define MAILBOX_IRQENABLE_CLR_NEWMSGENABLEUUMB3_SHIFT                                                     (6U)
#define MAILBOX_IRQENABLE_CLR_NEWMSGENABLEUUMB3_MASK                                                      (0x00000040U)

#define MAILBOX_IRQENABLE_CLR_NEWMSGENABLEUUMB2_SHIFT                                                     (4U)
#define MAILBOX_IRQENABLE_CLR_NEWMSGENABLEUUMB2_MASK                                                      (0x00000010U)

#define MAILBOX_IRQENABLE_CLR_NEWMSGENABLEUUMB6_SHIFT                                                     (12U)
#define MAILBOX_IRQENABLE_CLR_NEWMSGENABLEUUMB6_MASK                                                      (0x00001000U)

#define MAILBOX_IRQENABLE_CLR_NEWMSGENABLEUUMB7_SHIFT                                                     (14U)
#define MAILBOX_IRQENABLE_CLR_NEWMSGENABLEUUMB7_MASK                                                      (0x00004000U)

#define MAILBOX_IRQENABLE_CLR_NOTFULLENABLEUUMB6_SHIFT                                                    (13U)
#define MAILBOX_IRQENABLE_CLR_NOTFULLENABLEUUMB6_MASK                                                     (0x00002000U)

#define MAILBOX_IRQENABLE_CLR_NOTFULLENABLEUUMB2_SHIFT                                                    (5U)
#define MAILBOX_IRQENABLE_CLR_NOTFULLENABLEUUMB2_MASK                                                     (0x00000020U)

#define MAILBOX_IRQENABLE_CLR_NOTFULLENABLEUUMB7_SHIFT                                                    (15U)
#define MAILBOX_IRQENABLE_CLR_NOTFULLENABLEUUMB7_MASK                                                     (0x00008000U)

#define MAILBOX_IRQENABLE_CLR_NOTFULLENABLEUUMB5_SHIFT                                                    (11U)
#define MAILBOX_IRQENABLE_CLR_NOTFULLENABLEUUMB5_MASK                                                     (0x00000800U)

#define MAILBOX_IRQENABLE_CLR_NOTFULLENABLEUUMB1_SHIFT                                                    (3U)
#define MAILBOX_IRQENABLE_CLR_NOTFULLENABLEUUMB1_MASK                                                     (0x00000008U)

#define MAILBOX_IRQENABLE_CLR_NOTFULLENABLEUUMB3_SHIFT                                                    (7U)
#define MAILBOX_IRQENABLE_CLR_NOTFULLENABLEUUMB3_MASK                                                     (0x00000080U)

#define MAILBOX_IRQENABLE_CLR_NOTFULLENABLEUUMB8_SHIFT                                                    (17U)
#define MAILBOX_IRQENABLE_CLR_NOTFULLENABLEUUMB8_MASK                                                     (0x00020000U)

#define MAILBOX_IRQENABLE_CLR_NOTFULLENABLEUUMB4_SHIFT                                                    (9U)
#define MAILBOX_IRQENABLE_CLR_NOTFULLENABLEUUMB4_MASK                                                     (0x00000200U)

#define MAILBOX_IRQENABLE_CLR_NOTFULLENABLEUUMB0_SHIFT                                                    (1U)
#define MAILBOX_IRQENABLE_CLR_NOTFULLENABLEUUMB0_MASK                                                     (0x00000002U)

#define MAILBOX_IRQENABLE_CLR_NEWMSGENABLEUUMB4_SHIFT                                                     (8U)
#define MAILBOX_IRQENABLE_CLR_NEWMSGENABLEUUMB4_MASK                                                      (0x00000100U)

#define MAILBOX_IRQENABLE_CLR_RESERVED_SHIFT                                                              (16U)
#define MAILBOX_IRQENABLE_CLR_RESERVED_MASK                                                               (0xffff0000U)

#define MAILBOX_IRQENABLE_SET_NOTFULLENABLEUUMB0_SHIFT                                                    (1U)
#define MAILBOX_IRQENABLE_SET_NOTFULLENABLEUUMB0_MASK                                                     (0x00000002U)

#define MAILBOX_IRQENABLE_SET_NOTFULLENABLEUUMB4_SHIFT                                                    (9U)
#define MAILBOX_IRQENABLE_SET_NOTFULLENABLEUUMB4_MASK                                                     (0x00000200U)

#define MAILBOX_IRQENABLE_SET_NOTFULLENABLEUUMB3_SHIFT                                                    (7U)
#define MAILBOX_IRQENABLE_SET_NOTFULLENABLEUUMB3_MASK                                                     (0x00000080U)

#define MAILBOX_IRQENABLE_SET_NOTFULLENABLEUUMB1_SHIFT                                                    (3U)
#define MAILBOX_IRQENABLE_SET_NOTFULLENABLEUUMB1_MASK                                                     (0x00000008U)

#define MAILBOX_IRQENABLE_SET_NOTFULLENABLEUUMB5_SHIFT                                                    (11U)
#define MAILBOX_IRQENABLE_SET_NOTFULLENABLEUUMB5_MASK                                                     (0x00000800U)

#define MAILBOX_IRQENABLE_SET_NOTFULLENABLEUUMB6_SHIFT                                                    (13U)
#define MAILBOX_IRQENABLE_SET_NOTFULLENABLEUUMB6_MASK                                                     (0x00002000U)

#define MAILBOX_IRQENABLE_SET_NOTFULLENABLEUUMB7_SHIFT                                                    (15U)
#define MAILBOX_IRQENABLE_SET_NOTFULLENABLEUUMB7_MASK                                                     (0x00008000U)

#define MAILBOX_IRQENABLE_SET_NOTFULLENABLEUUMB2_SHIFT                                                    (5U)
#define MAILBOX_IRQENABLE_SET_NOTFULLENABLEUUMB2_MASK                                                     (0x00000020U)

#define MAILBOX_IRQENABLE_SET_NEWMSGENABLEUUMB7_SHIFT                                                     (14U)
#define MAILBOX_IRQENABLE_SET_NEWMSGENABLEUUMB7_MASK                                                      (0x00004000U)

#define MAILBOX_IRQENABLE_SET_NEWMSGENABLEUUMB2_SHIFT                                                     (4U)
#define MAILBOX_IRQENABLE_SET_NEWMSGENABLEUUMB2_MASK                                                      (0x00000010U)

#define MAILBOX_IRQENABLE_SET_NEWMSGENABLEUUMB6_SHIFT                                                     (12U)
#define MAILBOX_IRQENABLE_SET_NEWMSGENABLEUUMB6_MASK                                                      (0x00001000U)

#define MAILBOX_IRQENABLE_SET_NEWMSGENABLEUUMB5_SHIFT                                                     (10U)
#define MAILBOX_IRQENABLE_SET_NEWMSGENABLEUUMB5_MASK                                                      (0x00000400U)

#define MAILBOX_IRQENABLE_SET_NEWMSGENABLEUUMB1_SHIFT                                                     (2U)
#define MAILBOX_IRQENABLE_SET_NEWMSGENABLEUUMB1_MASK                                                      (0x00000004U)

#define MAILBOX_IRQENABLE_SET_NEWMSGENABLEUUMB3_SHIFT                                                     (6U)
#define MAILBOX_IRQENABLE_SET_NEWMSGENABLEUUMB3_MASK                                                      (0x00000040U)

#define MAILBOX_IRQENABLE_SET_NEWMSGENABLEUUMB4_SHIFT                                                     (8U)
#define MAILBOX_IRQENABLE_SET_NEWMSGENABLEUUMB4_MASK                                                      (0x00000100U)

#define MAILBOX_IRQENABLE_SET_NEWMSGENABLEUUMB0_SHIFT                                                     (0U)
#define MAILBOX_IRQENABLE_SET_NEWMSGENABLEUUMB0_MASK                                                      (0x00000001U)

#define MAILBOX_IRQENABLE_SET_RESERVED_SHIFT                                                              (16U)
#define MAILBOX_IRQENABLE_SET_RESERVED_MASK                                                               (0xffff0000U)

#ifdef __cplusplus
}
#endif
#endif  /* _HW_MAILBOX_H_ */
