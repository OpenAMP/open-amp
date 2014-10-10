/***********************************************************************
*
*             Copyright 2006 Mentor Graphics Corporation
*                         All Rights Reserved.
*
* THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION WHICH IS
* THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS LICENSORS AND IS
* SUBJECT TO LICENSE TERMS.
*
************************************************************************

************************************************************************
*
*   FILE NAME
*
*       arm_mmu_defs.h
*
*   DESCRIPTION
*
*       This file contains all definitions, structures, etc for the
*       ARM MMU.
*
*   DATA STRUCTURES
*
*       None
*
*   DEPENDENCIES
*
*       None
*
***********************************************************************/

#ifndef     ARM_MMU_DEFS_H
#define     ARM_MMU_DEFS_H

/* Define core cache availability
   NOTE:  A differentiation is made in ESAL between cache that
          is contained on a processor and cache that is
          inherent as part of a core (L2 vs L1 cache). */
#define     ESAL_CO_CACHE_AVAILABLE                 NU_TRUE

/*********************************************
* ARM v4/5 & v6/7 MMU Variant definitions
*********************************************/

#if (CFG_NU_OS_ARCH_ARM_COM_MMU_VARIANT != 2)

/* Translation table is 16K in size */
#define     ESAL_CO_MEM_TTB_SIZE                    ESAL_GE_MEM_16K

/* Each TTB descriptor covers a 1MB region */
#define     ESAL_CO_MEM_TTB_SECT_SIZE               ESAL_GE_MEM_1M

/* Mask off lower bits of addr */
#define     ESAL_CO_MEM_TTB_SECT_SIZE_MASK          (~(ESAL_CO_MEM_TTB_SECT_SIZE-1UL))

/* Define shift to convert memory address to index of translation table entry (descriptor).
   Shift 20 bits (for a 1MB section) - 2 bits (for a 4 byte TTB descriptor) */
#define     ESAL_CO_MEM_TTB_SECT_TO_DESC_SHIFT      (20-2)

/* Define domain access values */
#define     ESAL_CO_MEM_DOMAIN_D0_MANAGER_ACCESS    0x3

#define     ESAL_CO_MEM_TTB_DESC_BACKWARDS          ESAL_GE_MEM_32BIT_SET(4)
#define     ESAL_CO_MEM_TTB_DESC_AP_MANAGER        (ESAL_GE_MEM_32BIT_SET(10)        |          \
                                                    ESAL_GE_MEM_32BIT_SET(11))
#define     ESAL_CO_MEM_TTB_DESC_SECT               ESAL_GE_MEM_32BIT_SET(1)

/* Define translation table descriptor bits */
#define     ESAL_CO_MEM_TTB_DESC_B                  ESAL_GE_MEM_32BIT_SET(2)
#define     ESAL_CO_MEM_TTB_DESC_C                  ESAL_GE_MEM_32BIT_SET(3)
#define     ESAL_CO_MEM_TTB_DESC_TEX                ESAL_GE_MEM_32BIT_SET(12)
#define     ESAL_CO_MEM_TTB_DESC_S                  ESAL_GE_MEM_32BIT_SET(16)

#endif /* (CFG_NU_OS_ARCH_ARM_COM_MMU_VARIANT != 2) */

/*********************************************
* ARM v7-R MPU definitions
*********************************************/
#if (CFG_NU_OS_ARCH_ARM_COM_MMU_VARIANT == 2)

/* Define MPU region attribute bits. */
#define     ESAL_CO_MEM_MPU_ATTR_B                  ESAL_GE_MEM_32BIT_SET(0)
#define     ESAL_CO_MEM_MPU_ATTR_C                  ESAL_GE_MEM_32BIT_SET(1)
#define     ESAL_CO_MEM_MPU_ATTR_S                  ESAL_GE_MEM_32BIT_SET(2)
#define     ESAL_CO_MEM_MPU_ATTR_TEX                ESAL_GE_MEM_32BIT_SET(3)
#define     ESAL_CO_MEM_MPU_AP_XN                   ESAL_GE_MEM_32BIT_SET(12)
#define     ESAL_CO_MEM_MPU_AP_FULL                (ESAL_GE_MEM_32BIT_SET(8)        |          \
                                                    ESAL_GE_MEM_32BIT_SET(9))
#define     ESAL_CO_MEM_MPU_AP_RO                  (ESAL_GE_MEM_32BIT_SET(9)        |          \
                                                    ESAL_GE_MEM_32BIT_SET(10))
#define     ESAL_CO_MEM_MPU_REG_ENABLE              ESAL_GE_MEM_32BIT_SET(0)
#define     ESAL_CO_MEM_MPU_REG_COUNT_MASK          0xFF00

#endif /* (CFG_NU_OS_ARCH_ARM_COM_MMU_VARIANT == 2) */

/*********************************************
* Common definitions
*********************************************/
/* Define CP15 Register 1: control register bits */
#define     ESAL_CO_MEM_CP15_CTRL_V                ESAL_GE_MEM_32BIT_SET(13)
#define     ESAL_CO_MEM_CP15_CTRL_I                ESAL_GE_MEM_32BIT_SET(12)
#define     ESAL_CO_MEM_CP15_CTRL_Z                ESAL_GE_MEM_32BIT_SET(11)
#define     ESAL_CO_MEM_CP15_CTRL_W                ESAL_GE_MEM_32BIT_SET(3)
#define     ESAL_CO_MEM_CP15_CTRL_C                ESAL_GE_MEM_32BIT_SET(2)
#define     ESAL_CO_MEM_CP15_CTRL_A                ESAL_GE_MEM_32BIT_SET(1)
#define     ESAL_CO_MEM_CP15_CTRL_M                ESAL_GE_MEM_32BIT_SET(0)

/* MVA Format SBZ mask */
#define     ESAL_CO_MEM_MVA_SBZ_MASK             ~(ESAL_CO_MEM_CACHE_LINE_SIZE - 1UL)

/* Defines related to Cache Level ID Register */
#define     ESAL_CO_MEM_DCACHE_SIZE_SHIFT           16
#define     ESAL_CO_MEM_CACHE_SIZE_BIT              4
#define     ESAL_CO_MEM_CACHE_SIZE_MASK             0xF

/*********************************************
* ARM v4/5 MMU Variant definitions
*********************************************/
#if (CFG_NU_OS_ARCH_ARM_COM_MMU_VARIANT  == 0)

/* Define all access  (manager access permission / not cachable / not buffered)  */
#define     ESAL_CO_MEM_TTB_DESC_ALL_ACCESS         (ESAL_CO_MEM_TTB_DESC_AP_MANAGER |  \
                                                     ESAL_CO_MEM_TTB_DESC_BACKWARDS  |  \
                                                     ESAL_CO_MEM_TTB_DESC_SECT)

/* The size of each cache segment (in bytes) */
#define     ESAL_CO_MEM_CACHE_LINE_SIZE             32

/* Define the set and way sizes and bit shift values */
#define     ESAL_CO_MEM_CACHE_WAY_SIZE              4
#define     ESAL_CO_MEM_DSIZE_BITSHIFT              18
#define     ESAL_CO_MEM_CACHE_SET_SHIFT             2
#define     ESAL_CO_MEM_CACHE_WAY_SHIFT             30

#if (ESAL_CO_CACHE_AVAILABLE == NU_TRUE)

/* This macro invalidates all of the cache at the core level. */
#define     ESAL_CO_MEM_CACHE_ALL_INVALIDATE()                          \
            {                                                           \
                ESAL_TS_RTE_CP_WRITE(ESAL_TS_RTE_CP15, 0,               \
                                     0, ESAL_TS_RTE_C7,                 \
                                     ESAL_TS_RTE_C7, 0);                \
            }

/* This macro invalidates all of the data cache at the core level. */
#define     ESAL_CO_MEM_DCACHE_ALL_INVALIDATE()                         \
            {                                                           \
                ESAL_TS_RTE_CP_WRITE(ESAL_TS_RTE_CP15, 0,               \
                                     0, ESAL_TS_RTE_C7,                 \
                                     ESAL_TS_RTE_C6, 0);                \
            }

/* This macro flushes all data cache to physical memory (writeback cache)
   and invalidates all data cache entries at the core level. */
#define     ESAL_CO_MEM_DCACHE_ALL_FLUSH_INVAL()                        \
            {                                                           \
                UINT32  cache_nsets;                                    \
                UINT32  cache_way;                                      \
                UINT32  cache_val;                                      \
                UINT32  cache_sets;                                     \
                                                                        \
                /* Get cache info */                                    \
                ESAL_TS_RTE_CP_READ(ESAL_TS_RTE_CP15, 0, &cache_sets,   \
                                    ESAL_TS_RTE_C0, ESAL_TS_RTE_C0, 1); \
                                                                        \
                /* Isolate data cache size bits */                      \
                cache_sets >>= ESAL_CO_MEM_DSIZE_BITSHIFT;              \
                cache_sets &= ESAL_CO_MEM_CACHE_SIZE_MASK;              \
                                                                        \
                /* Calculate number of sets from this size value */     \
                cache_sets = (1UL << (cache_sets +                      \
                                      ESAL_CO_MEM_CACHE_SET_SHIFT));    \
                                                                        \
                /* Loop through each cache way */                       \
                for (cache_way = 0;                                     \
                     cache_way < ESAL_CO_MEM_CACHE_WAY_SIZE;            \
                     cache_way++)                                       \
                {                                                       \
                    /* Update the 'way' bit and clear the 'set' bits */ \
                    cache_val = (cache_way << ESAL_CO_MEM_CACHE_WAY_SHIFT); \
                                                                        \
                    /* Loop through each cache set */                   \
                    for (cache_nsets = 0;                               \
                         cache_nsets < cache_sets;                      \
                         cache_nsets++)                                 \
                    {                                                   \
                        /* Clean and invalidate the DCache */           \
                        ESAL_TS_RTE_CP_WRITE(ESAL_TS_RTE_CP15, 0,       \
                                             cache_val, ESAL_TS_RTE_C7, \
                                             ESAL_TS_RTE_C14, 2);       \
                                                                        \
                        /* Move to the next cache 'set' */              \
                        cache_val += ESAL_CO_MEM_CACHE_LINE_SIZE;       \
                    }                                                   \
                }                                                       \
            }

#endif  /* (ESAL_CO_CACHE_AVAILABLE == NU_TRUE) */

#endif  /* (CFG_NU_OS_ARCH_ARM_COM_MMU_VARIANT  == 0) */

/*************************************************
* ARM v6/7 MMU and ARMv7-R MPU Variant definitions
*************************************************/
#if (CFG_NU_OS_ARCH_ARM_COM_MMU_VARIANT  != 0)

/* Define all access  (manager access permission / not cachable / not bufferd)  */
#define     ESAL_CO_MEM_TTB_DESC_ALL_ACCESS         (ESAL_CO_MEM_TTB_DESC_AP_MANAGER |          \
                                                     ESAL_CO_MEM_TTB_DESC_SECT)

/* The size of each cache segment (in bytes) */
#define     ESAL_CO_MEM_CACHE_LINE_SIZE             (UINT)(CFG_NU_OS_ARCH_ARM_COM_V6V7_CACHE_LINE_SIZE)

/* This mask is used to align the virtual address to cache line size */
#define     ESAL_CO_MEM_ADDR_ALIGN_MASK             (~(ESAL_CO_MEM_CACHE_LINE_SIZE-1UL))

/* CLIDR and CCSIDR mask values */
#define     ESAL_CO_MEM_CLIDR_LOC_MASK              0x7000000
#define     ESAL_CO_MEM_CCSIDR_LINESIZE_MASK        0x7
#define     ESAL_CO_MEM_CCSIDR_ASSOC_MASK           0x3FF
#define     ESAL_CO_MEM_CCSIDR_NUMSET_MASK          0x7FFF

/* CLIDR and CCSIDR shift values */
#define     ESAL_CO_MEM_CLIDR_LOC_RSHT_OFFSET       24
#define     ESAL_CO_MEM_CCSIDR_ASSOC_RSHT_OFFSET    3
#define     ESAL_CO_MEM_CCSIDR_NUMSET_RSHT_OFFSET   13

/* Extract 'encoded' line length of the cache */
#define     ESAL_CO_MEM_CCSIDR_LINESIZE_GET(ccsidr_reg) (ccsidr_reg &                           \
                                                         ESAL_CO_MEM_CCSIDR_LINESIZE_MASK)

/* Extract 'encoded' way size of the cache */
#define     ESAL_CO_MEM_CCSIDR_ASSOC_GET(ccsidr_reg)    (ESAL_CO_MEM_CCSIDR_ASSOC_MASK &        \
                                                        (ccsidr_reg >>                          \
                                                         ESAL_CO_MEM_CCSIDR_ASSOC_RSHT_OFFSET))

/* Extract 'encoded' maximum number of index size */
#define     ESAL_CO_MEM_CCSIDR_NUMSET_GET(ccsidr_reg)   (ESAL_CO_MEM_CCSIDR_NUMSET_MASK &       \
                                                        (ccsidr_reg >>                          \
                                                         ESAL_CO_MEM_CCSIDR_NUMSET_RSHT_OFFSET))

/* Refer to chapter B3.12.31 c7, Cache and branch predictor maintenance functions in the
   ARM Architecture Reference Manual ARMv7-A and ARMv7-R Edition 1360*/
/* Calculate # of bits to be shifted for set size and way size */

/* log2(line size in bytes) = ccsidr_linesize + 2 + logbase2(4) */
#define     ESAL_CO_MEM_L_CALCULATE(linesize)           (linesize + 2 + 2)

/* log2(nsets) = 32 - way_size_bit_pos */

/* Find the bit position of way size increment */
#define     ESAL_CO_MEM_A_CALCULATE(assoc, a_offset_ref)                                        \
            {                                                                                   \
                UINT32  temp_pos = 0x80000000;                                                  \
                                                                                                \
                *a_offset_ref = 0;                                                              \
                                                                                                \
                /* Logic to count the number of leading zeros before the first 1 */             \
                while(!((assoc & temp_pos) == temp_pos))                                        \
                {                                                                               \
                    (*a_offset_ref)++;                                                          \
                    temp_pos = temp_pos >> 1;                                                   \
                }                                                                               \
            }

/* Factor way, cache number, index number */
#define     ESAL_CO_MEM_DCCISW_SET(dccisw_ref, level, numsets, assoc, l_offset, a_offset)       \
            {                                                                                   \
                *dccisw_ref = (level | (numsets << l_offset) | (assoc << a_offset));            \
            }

/* This macro extracts line size, assoc and set size from CCSIDR */
#define     ESAL_CO_MEM_CCSIDR_VALS_GET(linesize_ref, assoc_ref, numsets_ref,                   \
                                        l_offset_ref, a_offset_ref)                             \
            {                                                                                   \
                UINT32  ccsidr_val;                                                             \
                                                                                                \
                /* Read the selected cache's CCSIDR */                                          \
                ESAL_TS_RTE_CP_READ(ESAL_TS_RTE_CP15, 1, &ccsidr_val,                           \
                                    ESAL_TS_RTE_C0, ESAL_TS_RTE_C0, 0);                         \
                                                                                                \
                /* Extract 'encoded' line length of the cache */                                \
                *linesize_ref = ESAL_CO_MEM_CCSIDR_LINESIZE_GET(ccsidr_val);                    \
                                                                                                \
                /* Extract 'encoded' way size of the cache */                                   \
                *assoc_ref = ESAL_CO_MEM_CCSIDR_ASSOC_GET(ccsidr_val);                          \
                                                                                                \
                /* Extract 'encoded' maximum number of index size */                            \
                *numsets_ref = ESAL_CO_MEM_CCSIDR_NUMSET_GET(ccsidr_val);                       \
                                                                                                \
                /* Calculate # of bits to be shifted for set size and way size */               \
                                                                                                \
                /* log2(line size in bytes) = ccsidr_linesize + 2 + log2(4) */                  \
                *l_offset_ref = ESAL_CO_MEM_L_CALCULATE(*linesize_ref);                         \
                                                                                                \
                /* log2(nsets) = 32 - way_size_bit_pos */                                       \
                ESAL_CO_MEM_A_CALCULATE(*assoc_ref, a_offset_ref);                              \
            }

#if (ESAL_CO_CACHE_AVAILABLE == NU_TRUE)

/* This macro invalidates all of the cache at the core level. */
#define     ESAL_CO_MEM_CACHE_ALL_INVALIDATE()                                                  \
            {                                                                                   \
                ESAL_CO_MEM_ICACHE_ALL_INVALIDATE();                                            \
                ESAL_CO_MEM_DCACHE_ALL_INVALIDATE();                                            \
            }

/* This macro invalidates all of the data cache at the core level. */
#define     ESAL_CO_MEM_DCACHE_ALL_OP(type)                                                     \
            {                                                                                   \
                UINT32  clidr_val = 0;                                                          \
                UINT32  clidr_loc = 0;                                                          \
                UINT32  cache_number = 0;                                                       \
                UINT32  cache_type = 0;                                                         \
                UINT32  ccsidr_linesize = 0;                                                    \
                UINT32  ccsidr_assoc = 0;                                                       \
                INT32   ccsidr_numsets = 0;                                                     \
                INT32   way_size_copy = 0;                                                      \
                UINT32  set_size_bit_pos = 0;                                                   \
                UINT32  cache_number_pos = 0;                                                   \
                UINT32  way_size_bit_pos = 0;                                                   \
                UINT32  set_way_value = 0;                                                      \
                                                                                                \
                                                                                                \
                /* Read CLIDR to extract level of coherence (LOC) */                            \
                ESAL_TS_RTE_CP_READ(ESAL_TS_RTE_CP15, 1, &clidr_val,                            \
                                    ESAL_TS_RTE_C0, ESAL_TS_RTE_C0, 1);                         \
                                                                                                \
                /* Extract LOC from CLIDR and align it at bit 1 */                              \
                clidr_loc = (clidr_val & ESAL_CO_MEM_CLIDR_LOC_MASK) >>                         \
                            ESAL_CO_MEM_CLIDR_LOC_RSHT_OFFSET;                                  \
                                                                                                \
                /* Proceed only iff LOC is non-zero */                                          \
                if (clidr_loc != 0)                                                             \
                {                                                                               \
                    do                                                                          \
                    {                                                                           \
                        /* Extract cache type from CLIDR */                                     \
                        cache_number_pos = cache_number + (cache_number >> 1);                  \
                        cache_type = (clidr_val >> cache_number_pos) & 0x7;                     \
                                                                                                \
                        /* Continue only iff data cache */                                      \
                        if (cache_type >= 2)                                                    \
                        {                                                                       \
                            /* Select desired cache level in CSSELR */                          \
                            ESAL_TS_RTE_CP_WRITE(ESAL_TS_RTE_CP15, 2, cache_number,             \
                                                 ESAL_TS_RTE_C0, ESAL_TS_RTE_C0, 0);            \
                                                                                                \
                            ESAL_TS_RTE_ISB_EXECUTE();                                          \
                                                                                                \
                            /* Get data like linesize, assoc and set size */                    \
                            ESAL_CO_MEM_CCSIDR_VALS_GET(&ccsidr_linesize,                       \
                                                        &ccsidr_assoc,                          \
                                                        &ccsidr_numsets,                        \
                                                        &set_size_bit_pos,                      \
                                                        &way_size_bit_pos);                     \
                                                                                                \
                            do                                                                  \
                            {                                                                   \
                                way_size_copy = ccsidr_assoc;                                   \
                                                                                                \
                                do                                                              \
                                {                                                               \
                                    /* Factor way, cache number, index number */                \
                                    ESAL_CO_MEM_DCCISW_SET(&set_way_value, cache_number,        \
                                                           ccsidr_numsets, way_size_copy,       \
                                                           set_size_bit_pos,                    \
                                                           way_size_bit_pos);                   \
                                                                                                \
                                    /* Execute invalidate if type = 0 */                        \
                                    if (type == 0)                                              \
                                    {                                                           \
                                        ESAL_TS_RTE_CP_WRITE(ESAL_TS_RTE_CP15, 0,               \
                                                             set_way_value,                     \
                                                             ESAL_TS_RTE_C7,                    \
                                                             ESAL_TS_RTE_C6, 2);                \
                                    }                                                           \
                                    else                                                        \
                                    {                                                           \
                                        ESAL_TS_RTE_CP_WRITE(ESAL_TS_RTE_CP15, 0,               \
                                                             set_way_value,                     \
                                                             ESAL_TS_RTE_C7,                    \
                                                             ESAL_TS_RTE_C14, 2);               \
                                    }                                                           \
                                                                                                \
                                /* decrement the way */                                         \
                                } while((--way_size_copy) >= 0);                                \
                                                                                                \
                            /* decrement the set */                                             \
                            } while((--ccsidr_numsets) >= 0);                                   \
                                                                                                \
                        } /* end if */                                                          \
                                                                                                \
                        /* Increment cache number */                                            \
                        cache_number += 2;                                                      \
                                                                                                \
                    /* end do-while */                                                          \
                    } while(clidr_loc >= cache_number);                                         \
                                                                                                \
                }                                                                               \
                                                                                                \
                /* Switch back to cache level 0 in CSSELR */                                    \
                ESAL_TS_RTE_CP_WRITE(ESAL_TS_RTE_CP15, 2, 0,                                    \
                                     ESAL_TS_RTE_C0, ESAL_TS_RTE_C0, 0);                        \
                                                                                                \
                /* Sync */                                                                      \
                ESAL_TS_RTE_DSB_EXECUTE();                                                      \
                ESAL_TS_RTE_ISB_EXECUTE();                                                      \
            }

/* This macro flushes all data cache to physical memory (writeback cache)
   and invalidates all data cache entries at the core level. */
#define     ESAL_CO_MEM_DCACHE_ALL_FLUSH_INVAL()        ESAL_CO_MEM_DCACHE_ALL_OP(1)

#if (CFG_NU_OS_ARCH_ARM_COM_MMU_VARIANT  == 1)

/* This macro invalidates all of the data cache at the core level. */
#define     ESAL_CO_MEM_DCACHE_ALL_INVALIDATE()         ESAL_CO_MEM_DCACHE_ALL_OP(0)

#elif (CFG_NU_OS_ARCH_ARM_COM_MMU_VARIANT  == 2)

/* This macro invalidates all of the data caches at the core level. */
#define     ESAL_CO_MEM_DCACHE_ALL_INVALIDATE()                         \
            {                                                           \
                ESAL_TS_RTE_CP_WRITE(ESAL_TS_RTE_CP15, 0,               \
                                     0, ESAL_TS_RTE_C15,                \
                                     ESAL_TS_RTE_C5, 0);                \
                /* Sync */                                              \
                ESAL_TS_RTE_DSB_EXECUTE();                              \
                ESAL_TS_RTE_ISB_EXECUTE();                              \
            }

#endif /* (CFG_NU_OS_ARCH_ARM_COM_MMU_VARIANT  == 1) */

#endif  /* (ESAL_CO_CACHE_AVAILABLE == NU_TRUE) */

#endif  /* (CFG_NU_OS_ARCH_ARM_COM_MMU_VARIANT  != 0) */

/*********************************************
* Common cache operation macros
*********************************************/

#if (ESAL_CO_CACHE_AVAILABLE == NU_TRUE)

/* This macro invalidates all of the instruction cache at the core level. */
#define     ESAL_CO_MEM_ICACHE_ALL_INVALIDATE()                         \
            {                                                           \
                ESAL_TS_RTE_CP_WRITE(ESAL_TS_RTE_CP15, 0,               \
                                     0, ESAL_TS_RTE_C7,                 \
                                     ESAL_TS_RTE_C5, 0);                \
            }

/* This macro invalidates all instruction cache for the specified address
   range at the core level. */
#define     ESAL_CO_MEM_ICACHE_INVALIDATE(addr, size)                   \
            {                                                           \
                UINT32  addr_v=(UINT32)addr & ESAL_CO_MEM_MVA_SBZ_MASK; \
                UINT32  l_size = 0;                                     \
                UINT32  align_size = ((UINT32)size + ((UINT32)addr &    \
                                      (ESAL_CO_MEM_CACHE_LINE_SIZE-1)));\
                                                                        \
                /* This is to prevent compiler warnings. */             \
                NU_UNUSED_PARAM(align_size);                            \
                                                                        \
                do                                                      \
                {                                                       \
                    /* Invalidate Icache in MVA format */               \
                    ESAL_TS_RTE_CP_WRITE(ESAL_TS_RTE_CP15, 0,           \
                                         (addr_v), ESAL_TS_RTE_C7,      \
                                         ESAL_TS_RTE_C5, 1);            \
                                                                        \
                    /* Move to the next way */                          \
                    addr_v += ESAL_CO_MEM_CACHE_LINE_SIZE;              \
                    l_size += ESAL_CO_MEM_CACHE_LINE_SIZE;              \
                                                                        \
                } while (l_size < align_size);                          \
            }

/* This macro invalidates all data cache for the specified address
   range at the core level. */
#define     ESAL_CO_MEM_DCACHE_INVALIDATE(addr, size)                   \
            {                                                           \
                UINT32  addr_v=(UINT32)addr & ESAL_CO_MEM_MVA_SBZ_MASK; \
                UINT32  l_size = 0;                                     \
                UINT32  align_size = ((UINT32)size + ((UINT32)addr &    \
                                      (ESAL_CO_MEM_CACHE_LINE_SIZE-1)));\
                                                                        \
                /* This is to prevent compiler warnings. */             \
                NU_UNUSED_PARAM(align_size);                            \
                                                                        \
                do                                                      \
                {                                                       \
                    /* Invalidate Dcache in MVA format */               \
                    ESAL_TS_RTE_CP_WRITE(ESAL_TS_RTE_CP15, 0,           \
                                         (addr_v), ESAL_TS_RTE_C7,      \
                                         ESAL_TS_RTE_C6, 1);            \
                                                                        \
                    /* Move to the next way */                          \
                    addr_v += ESAL_CO_MEM_CACHE_LINE_SIZE;              \
                    l_size += ESAL_CO_MEM_CACHE_LINE_SIZE;              \
                                                                        \
                } while (l_size < align_size);                          \
            }

/* This macro flushes all data cache to physical memory (writeback cache)
   for the given address range, then invalidates all data cache entries
   at the core level. */
#define     ESAL_CO_MEM_DCACHE_FLUSH_INVAL(addr, size)                  \
            {                                                           \
                UINT32  addr_v=(UINT32)addr & ESAL_CO_MEM_MVA_SBZ_MASK; \
                UINT32  l_size = 0;                                     \
                UINT32  align_size = ((UINT32)size + ((UINT32)addr &    \
                                      (ESAL_CO_MEM_CACHE_LINE_SIZE-1)));\
                                                                        \
                /* This is to prevent compiler warnings. */             \
                NU_UNUSED_PARAM(align_size);                            \
                                                                        \
                do                                                      \
                {                                                       \
                    /* Clean (flush) and invalidate the Dcache */       \
                    ESAL_TS_RTE_CP_WRITE(ESAL_TS_RTE_CP15, 0,           \
                                         (addr_v), ESAL_TS_RTE_C7,      \
                                         ESAL_TS_RTE_C14, 1);           \
                                                                        \
                    /* Move to the next way */                          \
                    addr_v += ESAL_CO_MEM_CACHE_LINE_SIZE;              \
                    l_size += ESAL_CO_MEM_CACHE_LINE_SIZE;              \
                                                                        \
                } while (l_size < align_size);                          \
            }

/* This macro disables instruction and data cache */
#define         ESAL_CO_MEM_CACHE_DISABLE()                                                     \
                {                                                                               \
                    UINT32  cp15_ctrl_val;                                                      \
                                                                                                \
                    /* Read current CP15 control register value */                              \
                    ESAL_TS_RTE_CP_READ(ESAL_TS_RTE_CP15, 0, &cp15_ctrl_val, ESAL_TS_RTE_C1, ESAL_TS_RTE_C0, 0); \
                                                                                                \
                    /* Clear instruction cache enable and data cache enable bits */             \
                    cp15_ctrl_val &= ~(ESAL_CO_MEM_CP15_CTRL_I | ESAL_CO_MEM_CP15_CTRL_C);      \
                                                                                                \
                    /* Write updated CP15 control register value */                             \
                    ESAL_TS_RTE_CP_WRITE(ESAL_TS_RTE_CP15, 0, cp15_ctrl_val, ESAL_TS_RTE_C1, ESAL_TS_RTE_C0, 0); \
                    ESAL_TS_RTE_NOP_EXECUTE();                                                  \
                    ESAL_TS_RTE_NOP_EXECUTE();                                                  \
                    ESAL_TS_RTE_NOP_EXECUTE();                                                  \
                }
#if (CFG_NU_OS_ARCH_ARM_COM_MMU_VARIANT  == 1)
/* This macro flushes data cache and invalidate instruction cache */
#define         ESAL_CO_MEM_ICACHE_FLUSH_INVAL(addr, size)                                      \
                {                                                                               \
                    UINT32  cache_info;                                                         \
                    UINT32  line_size;                                                          \
                    UINT32  current_addr;                                                       \
                                                                                                \
                    /* Read cache level ID register value */                                    \
                    ESAL_TS_RTE_CP_READ(ESAL_TS_RTE_CP15, 0, &cache_info,                       \
                                        ESAL_TS_RTE_C0, ESAL_TS_RTE_C0, 1);                     \
                                                                                                \
                    /* Calculate the size */                                                    \
                    line_size = cache_info >> ESAL_CO_MEM_DCACHE_SIZE_SHIFT;                    \
                    line_size = line_size & ESAL_CO_MEM_CACHE_SIZE_MASK;                        \
                    line_size = ESAL_CO_MEM_CACHE_SIZE_BIT << line_size;                        \
                                                                                                \
                    /* Force alignment with current cache line */                               \
                    current_addr = (UINT32)addr & ~(line_size - 1);                             \
                                                                                                \
                    do                                                                          \
                    {                                                                           \
                        /* Clean dcache to PoU */                                               \
                        ESAL_TS_RTE_CP_WRITE(ESAL_TS_RTE_CP15, 0, current_addr,                 \
                                             ESAL_TS_RTE_C7, ESAL_TS_RTE_C11, 1);               \
                        current_addr += line_size;                                              \
                    } while (current_addr < ((UINT32)addr + size));                             \
                                                                                                \
                    /* Data barrier */                                                          \
                    ESAL_TS_RTE_DSB_EXECUTE();                                                  \
                                                                                                \
                    /* Calculate the size */                                                    \
                    line_size = cache_info & ESAL_CO_MEM_CACHE_SIZE_MASK;                       \
                    line_size = ESAL_CO_MEM_CACHE_SIZE_BIT << line_size;                        \
                                                                                                \
                    /* Force alignment with current cache line */                               \
                    current_addr = (UINT32)addr & ~(line_size - 1);                             \
                                                                                                \
                    do                                                                          \
                    {                                                                           \
                        /* Invalidate icache line */                                            \
                        ESAL_TS_RTE_CP_WRITE(ESAL_TS_RTE_CP15, 0, current_addr,                 \
                                             ESAL_TS_RTE_C7, ESAL_TS_RTE_C5, 1);                \
                        current_addr += line_size;                                              \
                    } while (current_addr < ((UINT32)addr + size));                             \
                                                                                                \
                    /* Invalidate BTB */                                                        \
                    ESAL_TS_RTE_CP_WRITE(ESAL_TS_RTE_CP15, 0, 0, ESAL_TS_RTE_C7,                \
                                         ESAL_TS_RTE_C5, 6);                                    \
                    ESAL_TS_RTE_DSB_EXECUTE();                                                  \
                    ESAL_TS_RTE_ISB_EXECUTE();                                                  \
                }
#else
#define         ESAL_CO_MEM_ICACHE_FLUSH_INVAL(addr, size)                                      \
                {                                                                               \
                    ESAL_CO_MEM_DCACHE_FLUSH_INVAL(addr, size);                                 \
                    ESAL_CO_MEM_ICACHE_INVALIDATE(addr, size);                                  \
                }
#endif

#endif  /* (ESAL_CO_CACHE_AVAILABLE == NU_TRUE) */

/*********************************************
* ARM Architecture MMU definitions
*********************************************/

/* All domains in client mode */
#define ESAL_AR_DACR_CLIENT                 0x55555555UL

/* All domains in manager mode */
#define ESAL_AR_DACR_MANAGER                0xFFFFFFFFUL

#if (CFG_NU_OS_ARCH_ARM_COM_MMU_VARIANT == 1)
#define ESAL_AR_MMU_ENABLE_BITS     (ESAL_CO_MEM_CP15_CTRL_M |          \
                                     ESAL_CO_MEM_CP15_CTRL_C |          \
                                     ESAL_CO_MEM_CP15_CTRL_W |          \
                                     ESAL_CO_MEM_CP15_CTRL_Z |          \
                                     ESAL_CO_MEM_CP15_CTRL_I)
#else
#define ESAL_AR_MMU_ENABLE_BITS     (ESAL_CO_MEM_CP15_CTRL_M |          \
                                     ESAL_CO_MEM_CP15_CTRL_C |          \
                                     ESAL_CO_MEM_CP15_CTRL_W |          \
                                     ESAL_CO_MEM_CP15_CTRL_I)

#endif

#define ESAL_AR_MMU_DISABLE_BITS    (~(ESAL_AR_MMU_ENABLE_BITS))

/* This macro provides a slight delay to make sure instructions
   have finished. */
#if (CFG_NU_OS_ARCH_ARM_COM_MMU_VARIANT == 1)

#define ESAL_AR_BARRIER()                                               \
    {                                                                   \
         ESAL_TS_RTE_ISB_EXECUTE();                                     \
         ESAL_TS_RTE_DSB_EXECUTE();                                     \
    }

#else

#define ESAL_AR_BARRIER()                                               \
    {                                                                   \
         ESAL_TS_RTE_NOP_EXECUTE();                                     \
         ESAL_TS_RTE_NOP_EXECUTE();                                     \
         ESAL_TS_RTE_NOP_EXECUTE();                                     \
    }

#endif

#if (CFG_NU_OS_ARCH_ARM_COM_MMU_VARIANT == 0)
/* This macro drains cache write buffers. */
#define ESAL_AR_DRAIN_WRITE_BUFFERS()                                   \
    {                                                                   \
        ESAL_TS_RTE_CP_WRITE(ESAL_TS_RTE_CP15, 0, 0, ESAL_TS_RTE_C7,    \
                             ESAL_TS_RTE_C10, 4);                       \
    }
#endif

/* This macro modifies the value of the Translation Table Base
   Register. */
#define ESAL_AR_SET_TRANSLATION_TABLE(ttbr_value)                       \
    {                                                                   \
       /* Write to translation table */                                 \
        ESAL_TS_RTE_CP_WRITE(ESAL_TS_RTE_CP15, 0, ttbr_value,           \
                             ESAL_TS_RTE_C2, ESAL_TS_RTE_C0, 0);        \
        ESAL_AR_BARRIER();                                              \
    }

/* This macro invalidates TLBs. */
#define ESAL_AR_INVALIDATE_TLB()                                        \
    {                                                                   \
        ESAL_TS_RTE_CP_WRITE(ESAL_TS_RTE_CP15, 0, 0, ESAL_TS_RTE_C8,    \
                             ESAL_TS_RTE_C7, 0);                        \
        ESAL_AR_BARRIER();                                              \
    }

/* This macro sets the domain access register to Manager mode. */
#define ESAL_AR_SET_DOMAIN(dacr)                                        \
    {                                                                   \
        ESAL_TS_RTE_CP_WRITE(ESAL_TS_RTE_CP15, 0, dacr,                 \
                             ESAL_TS_RTE_C3, ESAL_TS_RTE_C0, 0);        \
        ESAL_AR_BARRIER();                                              \
    }

#if (CFG_NU_OS_ARCH_ARM_COM_MMU_VARIANT == 0)
#define ESAL_AR_MMU_ENABLE_COMPLETE()                                   \
    {                                                                   \
	    /* Flush all of data cache on the core.*/                       \
        ESAL_GE_MEM_DCACHE_ALL_FLUSH_INVAL();                           \
        ESAL_AR_BARRIER();                                              \
                                                                        \
        /* Invalidate the cache immediately following an MMU enable */  \
        /* Interrupts should be disabled during these instructions. */  \
                                                                        \
        /* Invalidate all cache */                                      \
        ESAL_GE_MEM_CACHE_ALL_INVALIDATE();                             \
                                                                        \
        /* Drain Write buffer */                                        \
        ESAL_AR_DRAIN_WRITE_BUFFERS();                                  \
                                                                        \
        /* Invalidate all TLBs */                                       \
        ESAL_AR_INVALIDATE_TLB();                                       \
    }
#else
#define ESAL_AR_MMU_ENABLE_COMPLETE()                                   \
    {                                                                   \
        /* Invalidate all TLBs */                                       \
        ESAL_AR_INVALIDATE_TLB();                                       \
    }
#endif

/* This macro turns the MMU on using the Default bits for operation. */
#define ESAL_AR_MMU_ENABLE()                                            \
    {                                                                   \
        UNSIGNED tmp_val;                                               \
        ESAL_TS_RTE_CP_READ(ESAL_TS_RTE_CP15, 0, &tmp_val,              \
                            ESAL_TS_RTE_C1, ESAL_TS_RTE_C0, 0);         \
                                                                        \
        /* Clear high vectors */                                        \
        tmp_val &= ~(ESAL_CO_MEM_CP15_CTRL_V);                          \
                                                                        \
        /* Clean alignment bit */                                       \
        tmp_val &= ~(ESAL_CO_MEM_CP15_CTRL_A);                          \
                                                                        \
        /* Or with enable bits */                                       \
        tmp_val |= ESAL_AR_MMU_ENABLE_BITS;                             \
        ESAL_TS_RTE_CP_WRITE(ESAL_TS_RTE_CP15, 0, tmp_val,              \
                             ESAL_TS_RTE_C1, ESAL_TS_RTE_C0, 0);        \
        ESAL_AR_BARRIER();                                              \
                                                                        \
        /* Complete the MMU enable process */                           \
        ESAL_AR_MMU_ENABLE_COMPLETE();                                  \
    }

/* Turn OFF the MMU using the Default bits for the operation. */
#define ESAL_AR_MMU_DISABLE()                                           \
    {                                                                   \
        UNSIGNED tmp_val;                                               \
                                                                        \
        /* Read CP15 reg 1 */                                           \
        ESAL_TS_RTE_CP_READ(ESAL_TS_RTE_CP15, 0, &tmp_val,              \
                            ESAL_TS_RTE_C1, ESAL_TS_RTE_C0, 0);         \
                                                                        \
        /* And in disable bits */                                       \
        tmp_val &= ESAL_AR_MMU_DISABLE_BITS;                            \
                                                                        \
        /* Write value back */                                          \
        ESAL_TS_RTE_CP_WRITE(ESAL_TS_RTE_CP15, 0, tmp_val,              \
                             ESAL_TS_RTE_C1, ESAL_TS_RTE_C0, 0);        \
        ESAL_AR_BARRIER();                                              \
    }

#if (CFG_NU_OS_ARCH_ARM_COM_MMU_VARIANT == 1)

/* This macro writes to the address space identifier. */
#define ESAL_AR_SET_ASID(asid_value)                                    \
    {                                                                   \
        ESAL_TS_RTE_CP_WRITE(ESAL_TS_RTE_CP15, 0, asid_value,           \
                             ESAL_TS_RTE_C13, ESAL_TS_RTE_C0, 1);       \
        ESAL_AR_BARRIER();                                              \
    }

#endif

#endif  /* ARM_MMU_DEFS_H */
