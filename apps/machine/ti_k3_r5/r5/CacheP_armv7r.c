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

#include <kernel/dpl/CacheP.h>
#include <kernel/dpl/MpuP_armv7.h>
#include <kernel/dpl/HwiP.h>

#define CACHE_SECTION __attribute__((section(".text.cache")))

/* APIs defined in CacheP_armv7r_asm.S */
uint32_t CacheP_getCacheLevelInfo(uint32_t level);
uint32_t CacheP_getEnabled();
void CacheP_configForceWrThru(uint32_t enable);
void CacheP_disableL1d();
void CacheP_disableL1p();
void CacheP_enableL1d();
void CacheP_enableL1p();
void CacheP_invL1p(uint32_t blockPtr, uint32_t byteCnt);
void CacheP_invL1d(uint32_t blockPtr, uint32_t byteCnt);
void CacheP_setDLFO();

uint32_t  gCacheL1dCacheLineSize = 32;
uint32_t  gCacheL1pCacheLineSize = 32;

/* these are defined as part of SysConfig */
extern CacheP_Config gCacheConfig;

void CACHE_SECTION CacheP_init()
{
    uint32_t info, enabled;

    /* Read L1D cache info registers */
    info = CacheP_getCacheLevelInfo(0);
    gCacheL1dCacheLineSize = 4 << ((info & 0x7) + 2);

    /* Read L1P cache info registers for ROV */
    info = CacheP_getCacheLevelInfo(1);
    gCacheL1pCacheLineSize = 4 << ((info & 0x7) + 2);

    enabled = CacheP_getEnabled();

    /* disable the caches if anything is currently enabled */
    if (enabled) {
        CacheP_disable(CacheP_TYPE_ALL);
    }

    /* set DLFO, this is not needed on SOC AM64x and later SOCs */
    /* CacheP_setDLFO(); */

    if (gCacheConfig.enable) {
         CacheP_configForceWrThru(gCacheConfig.enableForceWrThru);

        /*
         * CacheP_enable() code will invalidate the L1D and L1P caches.
         * Therefore, no need to explicitly invalidate the cache here.
         */
        CacheP_enable(CacheP_TYPE_ALL);
    }
}

void CACHE_SECTION CacheP_disable(uint32_t type)
{
    uint32_t enabled;
    uintptr_t key;

    /* only disable caches that are currently enabled */
    enabled = CacheP_getEnabled();

    if (enabled & (type & CacheP_TYPE_L1D)) {
        key = HwiP_disable();
        CacheP_disableL1d();             /* Disable L1D Cache */
        HwiP_restore(key);
    }
    if (enabled & (type & CacheP_TYPE_L1P)) {
        key = HwiP_disable();
        CacheP_disableL1p();             /* Disable L1P Cache */
        HwiP_restore(key);
    }

}

void CACHE_SECTION CacheP_enable(uint32_t type)
{
    uint32_t disabled;

    /* only enable caches that are currently disabled */
    disabled = ~(CacheP_getEnabled());

    if (disabled & (type & CacheP_TYPE_L1D)) {
        CacheP_enableL1d();              /* Enable L1D Cache */
    }
    if (disabled & (type & CacheP_TYPE_L1P)) {
        CacheP_enableL1p();              /* Enable L1P Cache */
    }
}

void CACHE_SECTION CacheP_inv(void *blockPtr, uint32_t byteCnt, uint32_t type)
{
    if (type & CacheP_TYPE_L1P) {
        CacheP_invL1p((uint32_t)blockPtr, byteCnt);
    }
    if (type & CacheP_TYPE_L1D) {
        CacheP_invL1d((uint32_t)blockPtr, byteCnt);
    }
}
