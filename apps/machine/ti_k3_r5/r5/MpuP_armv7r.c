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


#include <kernel/dpl/MpuP_armv7.h>
#include <kernel/dpl/CacheP.h>
#include <kernel/dpl/HwiP.h>

#define MPU_SECTION __attribute__((section(".text.mpu")))

/* Max possible regions in ARMv7-R CPU */
#define MpuP_MAX_REGIONS    (16u)

/* APIs defined in MpuP_armv7r_asm.s */
void MpuP_disableAsm(void);
void MpuP_enableAsm(void);
uint32_t MpuP_isEnableAsm(void);
void MpuP_disableBRAsm(void);
void MpuP_enableBRAsm(void);
void MpuP_setRegionAsm(uint32_t regionId, uint32_t regionBaseAddr,
              uint32_t sizeAndEnble, uint32_t regionAttrs);

/* these are defined as part of SysConfig */
extern MpuP_Config gMpuConfig;
extern MpuP_RegionConfig gMpuRegionConfig[];


static uint32_t MPU_SECTION MpuP_getAttrs(MpuP_RegionAttrs *region)
{
    uint32_t regionAttrs =
          ((uint32_t)(region->isExecuteNever & 0x1) << 12)
        | ((uint32_t)(region->accessPerm     & 0x7) <<  8)
        | ((uint32_t)(region->tex            & 0x7) <<  3)
        | ((uint32_t)(region->isSharable     & 0x1) <<  2)
        | ((uint32_t)(region->isCacheable    & 0x1) <<  1)
        | ((uint32_t)(region->isBufferable   & 0x1) <<  0);

    return regionAttrs;
}

void MPU_SECTION MpuP_RegionAttrs_init(MpuP_RegionAttrs *region)
{
    region->isExecuteNever = 0;
    region->accessPerm     = MpuP_AP_S_RW_U_R;
    region->tex            = 0;
    region->isSharable     = 1;
    region->isCacheable    = 0;
    region->isBufferable   = 0;
    region->isEnable       = 0;
    region->subregionDisableMask = 0;
}

void MPU_SECTION MpuP_setRegion(uint32_t regionNum, void * addr, uint32_t size, MpuP_RegionAttrs *attrs)
{
    uint32_t baseAddress, sizeAndEnable, regionAttrs;
    uint32_t enabled;
    uintptr_t key;

    // DebugP_assertNoLog( regionNum < MpuP_MAX_REGIONS);

    /* size 5b field */
    size = (size & 0x1F);

    /* If N is the value in size field, the region size is 2N+1 bytes. */
    sizeAndEnable = ((uint32_t)(attrs->subregionDisableMask & 0xFF) << 8)
                  | ((uint32_t)(size            & 0x1F) << 1)
                  | ((uint32_t)(attrs->isEnable &  0x1) << 0);

    /* align base address to region size */
    baseAddress = ((uint32_t)addr & ~( (1<<((uint64_t)size+1))-1 ));

    /* get region attribute mask */
    regionAttrs = MpuP_getAttrs(attrs);

    enabled = MpuP_isEnable();

    /* disable the MPU (if already disabled, does nothing) */
    MpuP_disable();

    key = HwiP_disable();

    MpuP_setRegionAsm(regionNum, baseAddress, sizeAndEnable, regionAttrs);

    HwiP_restore(key);

    if (enabled) {
        MpuP_enable();
    }
}

void MPU_SECTION MpuP_enable()
{
    if(!MpuP_isEnable())
    {
        uint32_t type;
        uintptr_t key;

        key = HwiP_disable();

        /* get the current enabled bits */
        type = CacheP_getEnabled();

        if (type & CacheP_TYPE_ALLP) {
            CacheP_disable(CacheP_TYPE_ALLP);
        }

        MpuP_enableAsm();

        /* set cache back to initial settings */
        CacheP_enable(type);

        __asm__ (" dsb");
        __asm__ (" isb");

        HwiP_restore(key);
    }
}

void MPU_SECTION MpuP_disable()
{
    if(MpuP_isEnable())
    {
        uint32_t type;
        uintptr_t key;

        key = HwiP_disable();

        /* get the current enabled bits */
        type = CacheP_getEnabled();

        /* disable all enabled caches */
        CacheP_disable(type);

        __asm__ (" dsb");

        MpuP_disableAsm();

        /* set cache back to initial settings */
        CacheP_enable(type);

        HwiP_restore(key);
    }
}

uint32_t MPU_SECTION MpuP_isEnable()
{
    return MpuP_isEnableAsm();
}

void MPU_SECTION MpuP_init()
{
    uint32_t i;

    if (MpuP_isEnable()) {
        MpuP_disable();
    }

    MpuP_disableBRAsm();

    // DebugP_assertNoLog( gMpuConfig.numRegions < MpuP_MAX_REGIONS);

    /*
     * Initialize MPU regions
     */
    for (i = 0; i < gMpuConfig.numRegions; i++)
    {
        MpuP_setRegion(i,
                (void*)gMpuRegionConfig[i].baseAddr,
                gMpuRegionConfig[i].size,
                &gMpuRegionConfig[i].attrs
        );
    }

    if (gMpuConfig.enableBackgroundRegion) {
        MpuP_enableBRAsm();
    }

    if (gMpuConfig.enableMpu) {
        MpuP_enable();
    }
}
