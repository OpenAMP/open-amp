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

#ifndef MPUP_ARM_V7_H
#define MPUP_ARM_V7_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * \defgroup KERNEL_DPL_MPU_ARMV7 APIs for MPU for ARMv7 (ARM R5, ARM M4)
 * \ingroup KERNEL_DPL
 *
 * For more details and example usage, see \ref KERNEL_DPL_MPU_ARMV7_PAGE
 *
 * @{
 */

/**
 * \brief Enum's to represent different types of access permissions that are possible for a given MPU region
 */
typedef enum MpuP_AccessPerm_
{
    MpuP_AP_ALL_BLOCK = (0x0u), /**< All accesses are blocked */
    MpuP_AP_S_RW      = (0x1u), /**< Only RD+WR supervisor mode accesses are allowed  */
    MpuP_AP_S_RW_U_R  = (0x2u), /**< RD+WR supervisor and RD only user mode accesses are allowed  */
    MpuP_AP_ALL_RW    = (0x3u), /**< All RD+WR accesses are allowed */
    MpuP_AP_S_R       = (0x5u), /**< Only RD supervisor mode accesses are allowed */
    MpuP_AP_ALL_R     = (0x6u)  /**< All RD accesses are allowed */
} MpuP_AccessPerm;

/**
 * \brief Enum's to represent different possible MPU region size
 */
typedef enum MpuP_RegionSize_ {
    MpuP_RegionSize_32 = 0x4,
    MpuP_RegionSize_64,
    MpuP_RegionSize_128,
    MpuP_RegionSize_256,
    MpuP_RegionSize_512,
    MpuP_RegionSize_1K,
    MpuP_RegionSize_2K,
    MpuP_RegionSize_4K,
    MpuP_RegionSize_8K,
    MpuP_RegionSize_16K,
    MpuP_RegionSize_32K,
    MpuP_RegionSize_64K,
    MpuP_RegionSize_128K,
    MpuP_RegionSize_256K,
    MpuP_RegionSize_512K,
    MpuP_RegionSize_1M,
    MpuP_RegionSize_2M,
    MpuP_RegionSize_4M,
    MpuP_RegionSize_8M,
    MpuP_RegionSize_16M,
    MpuP_RegionSize_32M,
    MpuP_RegionSize_64M,
    MpuP_RegionSize_128M,
    MpuP_RegionSize_256M,
    MpuP_RegionSize_512M,
    MpuP_RegionSize_1G,
    MpuP_RegionSize_2G,
    MpuP_RegionSize_4G
} MpuP_RegionSize;

/**
 * \brief Attribute's to apply for a MPU region
 *
 * \note Refer to ARMv7-R or ARMv7-M architecture manual for more details
 *
 * \note C, B, S, TEX[0:2] bits
 *   together control if a region should be fully cached or non-cached or marked as device memory
 */
typedef struct MpuP_RegionAttrs_ {

    uint8_t isEnable; /**< 1: enable this region, 0: disable this region */
    uint8_t isCacheable; /**< 1: set C bit, 0: clear C bit */
    uint8_t isBufferable; /**< 1: set B bit, 0: clear B bit */
    uint8_t isSharable; /**< 1: set S bit, 0: clear S bit */
    uint8_t isExecuteNever; /**< 1: set XN bit, 0: clear XN bit */
    uint8_t tex; /**< set TEX[0:2] bits */
    uint8_t accessPerm; /**< set AP[0:2] bits, see \ref MpuP_AccessPerm */
    uint8_t subregionDisableMask; /**< subregion disable mask, bitN = 1 means disable that subregion */
} MpuP_RegionAttrs;

/**
 * \brief Region config structure, this used by SysConfig and not to be used by end-users directly
 */
typedef struct MpuP_RegionConfig_ {

    uint32_t baseAddr;  /**< region start address, MUST aligned to region size */
    uint32_t size; /**< region size, see \ref MpuP_RegionSize */
    MpuP_RegionAttrs attrs; /** region attributes, see \ref MpuP_RegionAttrs */

} MpuP_RegionConfig;

/**
 * \brief MPU config structure, this used by SysConfig and not to be used by end-users directly
 */
typedef struct MpuP_Config_ {

    uint32_t numRegions;             /** Number of regions to configure */
    uint32_t enableBackgroundRegion; /**< 0: disable backgroun region, 1: enable background region */
    uint32_t enableMpu;              /**< 0: keep MPU disabled, 1: enable MPU */

} MpuP_Config;

/**
 * \brief Set default values to MpuP_RegionAttrs
 *
 * Strongly recommended to be called before seting values in MpuP_RegionAttrs
 *
 * \param region [out] parameter structure to set to default
 */
void MpuP_RegionAttrs_init(MpuP_RegionAttrs *region);

/**
 * \brief Setup a region in the MPU
 *
 * \note Refer to ARMv7-R or ARMv7-M architecture manual for more details
 * \note Recommended to disable MPU and disable cache before setting up MPU regions
 *
 * \param regionNum [in] region to setup
 * \param addr [in] region start address, MUST aligned to region size
 * \param size [in] region size, see \ref MpuP_RegionSize
 * \param attrs [in] region attrs, see \ref MpuP_RegionAttrs
 */
void MpuP_setRegion(uint32_t regionNum, void * addr, uint32_t size, MpuP_RegionAttrs *attrs);

/**
 * \brief Enable MPU sub-system using the region that are setup using \ref MpuP_setRegion
 */
void MpuP_enable();

/**
 * \brief Disable MPU sub-system
 */
void MpuP_disable();

/**
 * \brief Check if MPU sub-system is enabled
 *
 * \return 0: MPU sub-system is disabled, 1: MPU sub-system is enabled
 */
uint32_t MpuP_isEnable();

/**
 * \brief Initialize MPU sub-system, called by SysConfig, not to be called by end users
 *
 */
void MpuP_init();

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* MPUP_ARM_V7_H */
