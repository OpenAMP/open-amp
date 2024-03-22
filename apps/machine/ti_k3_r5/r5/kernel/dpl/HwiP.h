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

#ifndef HWIP_H
#define HWIP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * \defgroup KERNEL_DPL_HWI APIs for HW Interrupts
 * \ingroup KERNEL_DPL
 *
 * For more details and example usage, see \ref KERNEL_DPL_HWI_PAGE
 *
 * @{
 */

/**
 * \brief Callback that is called when a HW interrupt is received
 *
 * \param args  [in] user argument passed during \ref HwiP_construct
 */
typedef void (*HwiP_FxnCallback)(void *args);


/**
 * \brief HwiP config parameters, setup as part of SysConfig, not to be set by end-users directly
 */
typedef struct HwiP_Config_
{
    uint32_t intcBaseAddr; /**< For R5F, this is VIM base addr */

} HwiP_Config;

/**
 * \brief Parameters passed during \ref HwiP_construct
 */
typedef struct HwiP_Params_ {

    uint32_t intNum;   /**< CPU interrupt number. */
    HwiP_FxnCallback callback; /**< Callback to call when interrupt is received */
    void *args; /**< Arguments to pass to the callback */
    uint16_t eventId; /**< Event ID to register against, only used with c6x with event combiner */
    uint8_t priority; /**< Interrupt priority, only used with ARM R5, ARM M4 */
    uint8_t isFIQ; /**< 0: Map interrupt as ISR, 1: map interrupt as FIQ, only used with ARM R5 */
    uint8_t isPulse; /**< 0: Map interrupt as level interrupt, 1: Map interrupt as pulse interrupt, only used with ARM R5, ARM M4 */

} HwiP_Params;

/**
 * \brief Max size of Hwi object across no-RTOS and all OS's
 */
#define HwiP_OBJECT_SIZE_MAX    (32u)
/**
 * \brief Opaque Hwi object used with the Hwi APIs
 */
typedef struct HwiP_Object_ {

    uint32_t rsv[HwiP_OBJECT_SIZE_MAX/sizeof(uint32_t)]; /**< reserved, should NOT be modified by end users */

} HwiP_Object;

/**
 * \brief Set default values to HwiP_Params
 *
 * Strongly recommended to be called before seting values in HwiP_Params
 *
 * \param params [out] parameter structure to set to default
 */
void HwiP_Params_init(HwiP_Params *params);

/**
 * \brief Create a Hwi object
 *
 * \param obj [out] created object
 * \param params [in] parameter structure
 *
 * \return \ref SystemP_SUCCESS on success, \ref SystemP_FAILURE on error
 */
int32_t HwiP_construct(HwiP_Object *obj, HwiP_Params *params);


/**
 * \brief Set argument to pass to the ISR
 *
 * \param obj [out] created object
 * \param args [in] argument to pass to the ISR
 *
 * \return \ref SystemP_SUCCESS on success, \ref SystemP_FAILURE on error
 */
int32_t HwiP_setArgs(HwiP_Object *obj, void *args);

/**
 * \brief Cleanup, delete, destruct a Hwi object
 *
 * \param obj [in] Hwi object
 */
void HwiP_destruct(HwiP_Object *obj);

/**
 * \brief Enable a specific interrupt
 *
 * \param intNum [in] Interrupt number
 */
void HwiP_enableInt(uint32_t intNum);

/**
 * \brief Disable a specific interrupt
 *
 * The return value is typically used with \ref HwiP_restoreInt to restore the interrupt state
 * to old value.
 *
 * \param intNum [in] Interrupt number
 *
 * \return old interrupt state, \n 0: interrupt was disabled previously, \n 1: interrupt was enabled previously
 */
uint32_t HwiP_disableInt(uint32_t intNum);

/**
 * \brief Restore a specific interrupt
 *
 * The oldIntState value typically returned by \ref HwiP_disableInt is used to restore the interrupt state
 * to old value.
 *
 * \param intNum [in] Interrupt number
 * \param oldIntState [in] 0: disable interrupt, 1: enable interrupt
 */
void HwiP_restoreInt(uint32_t intNum, uint32_t oldIntState);

/**
 * \brief Clear a pending specific interrupt
 *
 * \param intNum [in] Interrupt number
 */
void HwiP_clearInt(uint32_t intNum);



/**
 * \brief Force trigger a specific interrupt
 *
 * \param intNum [in] Interrupt number
 */
void HwiP_post(uint32_t intNum);

/**
 * \brief Disable all interrupts
 *
 * \note In case of ARM R5F, ARM M4F, this only disables IRQ. \n
 *       FIQ is not disabled.
 *
 * \return interrupt state before disable, typically used by \ref HwiP_restore later
 */
uintptr_t HwiP_disable();

/**
 * \brief Enable all interrupts
 *
 * \note In case of ARM R5F, ARM M4F, this only enables IRQ. \n
 *       FIQ is not enabled.
 */
void HwiP_enable();

/**
 * \brief Restores all interrupts to a given state
 *
 * \note In case of ARM R5F, ARM M4F, this only restores IRQ state. \n
 *       FIQ state is not changed.
 *
 * \param oldIntState [in] interrupt state, typically returned by \ref HwiP_disable earlier
 */
void HwiP_restore(uintptr_t oldIntState);


/**
 * \brief Check if the caller of this function is inside a ISR or not
 *
 * In some cases, like with freertos, some functions cannot be called from within the OS
 * ISR handler, this API allows the user and some driver porting layer (DPL) APIs
 * to check and call the appropiate ISR safe API when in ISR handler mode.
 *
 * To get the exact CPU mode of the executing CPU, use the low level CPU specific system
 * calls/registers.
 *
 * \note In case of ARM R5F, this only checks if caller is inside IRQ or not.
 *       This means when HwiP_inISR returns 1, CPU is in IRQ mode
 *       and when HwiP_inISR return 0, CPU could be in system mode or FIQ or abort mode and so on
 *
 * \return 0 not in interrupt mode, 1 in interrupt mode
 */
uint32_t HwiP_inISR(void);

/**
 * \brief Initialize Hwi module
 *
 * - Disables all individual interrupts
 * - Clears all individual interrupts
 * - Enables global interrupts
 *
 * \note MUST be called during system intialization before any \ref HwiP_construct API calls.
 * \note In case of ARM R5F, ARM M4F, this initializes and enables both FIQ and IRQ
 */
void HwiP_init();

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* HWIP_H */
