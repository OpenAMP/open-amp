/***********************************************************************
*
*            Copyright 2011 Mentor Graphics Corporation
*                         All Rights Reserved.
*
* THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION WHICH IS
* THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS LICENSORS AND IS
* SUBJECT TO LICENSE TERMS.
*
************************************************************************

************************************************************************
*
*   DESCRIPTION
*
*       This file provides the external interface to the Embedded
*       Software Abstraction Layer components
*
***********************************************************************/

#ifndef             ESAL_EXTR_H
#define             ESAL_EXTR_H

#ifdef              __cplusplus

/* C declarations in C++     */
extern              "C" {
#endif

/* Include configuration header files */
#include <string.h>
#include "services/nu_trace_os_mark.h"

/************************************************************************/
/*       Other GENERIC ESAL macros/defines needed by PLUS               */
/************************************************************************/

/* Define all externally accessible, interrupt related function prototypes */
INT                 ESAL_GE_INT_Global_Set(INT new_value);


/************************************************************************/
/*            Nucleus Toolset specific macros                           */
/************************************************************************/

#define             ESAL_GE_INT_DISABLE_BITS                ESAL_AR_INTERRUPTS_DISABLE_BITS
#define             ESAL_GE_INT_ENABLE_BITS                 ESAL_AR_INTERRUPTS_ENABLE_BITS


/************************************************************************/
/*                  Memory                                              */
/************************************************************************/

/* Define memory common read/write macros */
#define             ESAL_GE_MEM_READ8(addr)         *(volatile UINT8 *)(addr)
#define             ESAL_GE_MEM_READ16(addr)        *(volatile UINT16 *)(addr)
#define             ESAL_GE_MEM_READ32(addr)        *(volatile UINT32 *)(addr)
#define             ESAL_GE_MEM_WRITE8(addr,data)   *(volatile UINT8 *)(addr) = (UINT8)(data)
#define             ESAL_GE_MEM_WRITE16(addr,data)  *(volatile UINT16 *)(addr) = (UINT16)(data)
#define             ESAL_GE_MEM_WRITE32(addr,data)  *(volatile UINT32 *)(addr) = (UINT32)(data)

#if             (ESAL_TS_64BIT_SUPPORT == NU_TRUE)

#define             ESAL_GE_MEM_READ64(addr)        *(volatile UINT64 *)(addr)
#define             ESAL_GE_MEM_WRITE64(addr,data)  *(volatile UINT64 *)(addr) = (UINT64)(data)

#endif          /* ESAL_TS_64BIT_SUPPORT == NU_TRUE */

/* Defines for big endian and little endian */
#define             ESAL_BIG_ENDIAN                 0
#define             ESAL_LITTLE_ENDIAN              1

/* Defines used for common memory sizes */
#define             ESAL_GE_MEM_1K                  1024UL
#define             ESAL_GE_MEM_2K                  (ESAL_GE_MEM_1K * 2UL)
#define             ESAL_GE_MEM_4K                  (ESAL_GE_MEM_1K * 4UL)
#define             ESAL_GE_MEM_8K                  (ESAL_GE_MEM_1K * 8UL)
#define             ESAL_GE_MEM_16K                 (ESAL_GE_MEM_1K * 16UL)
#define             ESAL_GE_MEM_32K                 (ESAL_GE_MEM_1K * 32UL)
#define             ESAL_GE_MEM_64K                 (ESAL_GE_MEM_1K * 64UL)
#define             ESAL_GE_MEM_128K                (ESAL_GE_MEM_1K * 128UL)
#define             ESAL_GE_MEM_256K                (ESAL_GE_MEM_1K * 256UL)
#define             ESAL_GE_MEM_512K                (ESAL_GE_MEM_1K * 512UL)
#define             ESAL_GE_MEM_1M                  (ESAL_GE_MEM_1K * 1024UL)
#define             ESAL_GE_MEM_2M                  (ESAL_GE_MEM_1M * 2UL)
#define             ESAL_GE_MEM_4M                  (ESAL_GE_MEM_1M * 4UL)
#define             ESAL_GE_MEM_8M                  (ESAL_GE_MEM_1M * 8UL)
#define             ESAL_GE_MEM_16M                 (ESAL_GE_MEM_1M * 16UL)
#define             ESAL_GE_MEM_32M                 (ESAL_GE_MEM_1M * 32UL)
#define             ESAL_GE_MEM_64M                 (ESAL_GE_MEM_1M * 64UL)
#define             ESAL_GE_MEM_128M                (ESAL_GE_MEM_1M * 128UL)
#define             ESAL_GE_MEM_256M                (ESAL_GE_MEM_1M * 256UL)
#define             ESAL_GE_MEM_512M                (ESAL_GE_MEM_1M * 512UL)
#define             ESAL_GE_MEM_1G                  (ESAL_GE_MEM_1M * 1024UL)
#define             ESAL_GE_MEM_2G                  (ESAL_GE_MEM_1G * 2UL)
#define             ESAL_GE_MEM_3G                  (ESAL_GE_MEM_1G * 3UL)
#define             ESAL_GE_MEM_4G                  (ESAL_GE_MEM_1G * 4UL)

/* Check if architecture uses reverse bit-ordering (most-significant bit is bit 0
   instead of bit 31) */
#ifdef ESAL_AR_REVERSE_BIT_ORDERING

/* Macro used to make a 32-bit value with the specified bit set (reverse ordering) */
#define             ESAL_GE_MEM_32BIT_SET(bit_num)      (1UL<<(31-bit_num))

/* Macro used to make a 32-bit value with the specified bit clear (reverse ordering) */
#define             ESAL_GE_MEM_32BIT_CLEAR(bit_num)    ~(1UL<<(31-bit_num))

/* Macro used to get the value of the bits starting at start_bit up to
   end_bit (reverse ordering) */
#define             ESAL_GE_MEM_32BIT_VAL_GET(value,start_bit,end_bit)                              \
                        (UINT32)(((UINT32)(value) &                                                 \
                                 (ESAL_GE_MEM_32BIT_SET(31-start_bit) - 1 +                         \
                                 ESAL_GE_MEM_32BIT_SET(31-start_bit))) >>                           \
                                 (31-end_bit))

/* Macro used to test if the specified bit number is set
   (returns NU_TRUE if set / NU_FALSE if not set) (reverse ordering) */
#define             ESAL_GE_MEM_32BIT_TEST(value,bit_num)                                           \
                        (((UINT32)(value) & ESAL_GE_MEM_32BIT_SET(31-bit_num)) ? NU_TRUE : NU_FALSE)

#else

/* Macro used to make a 32-bit value with the specified bit set */
#define             ESAL_GE_MEM_32BIT_SET(bit_num)      (1UL<<(bit_num))

/* Macro used to make a 32-bit value with the specified bit clear */
#define             ESAL_GE_MEM_32BIT_CLEAR(bit_num)    ~(1UL<<(bit_num))

/* Macro used to get the value of the bits starting from start_bit up to
   end_bit */
#define             ESAL_GE_MEM_32BIT_VAL_GET(value,start_bit,end_bit)                  \
                        (UINT32)(((UINT32)(value) &                                     \
                                 (ESAL_GE_MEM_32BIT_SET(end_bit) - 1 +                  \
                                 ESAL_GE_MEM_32BIT_SET(end_bit))) >>                    \
                                 (start_bit))

/* Macro used to test if the specified bit number is set
   (returns NU_TRUE if set / NU_FALSE if not set) */
#define             ESAL_GE_MEM_32BIT_TEST(value,bit_num)                                           \
                        (((UINT32)(value) & ESAL_GE_MEM_32BIT_SET(bit_num)) ? NU_TRUE : NU_FALSE)

#endif  /* ESAL_AR_REVERSE_BIT_ORDERING */

/* Macro used to test if the specified bit mask is set within the given value
   (returns NU_TRUE if entire bit mask is set / NU_FALSE if entire bitmask not set) */
#define             ESAL_GE_MEM_32BIT_MASK_TEST(value,bit_mask)                                     \
                        (((UINT32)(value) & (bit_mask)) == (bit_mask) ? NU_TRUE : NU_FALSE)

/* Define for size of 4 unsigned longs */
#define             ESAL_GE_MEM_4_X_32BIT_SIZE      (UINT32)(sizeof(UINT32) << 2)

/* Define for size of 1 unsigned long */
#define             ESAL_GE_MEM_1_X_32BIT_SIZE      (UINT32)(sizeof(UINT32))

/* Define macros used to align pointers / check for alignment based on data pointer size */
#if (ESAL_TS_DATA_PTR_SIZE <= 16)

/* Macro used to check if a value is aligned to the required boundary.
   Returns NU_TRUE if aligned; NU_FALSE if not aligned
   NOTE:  The required alignment must be a power of 2 (2, 4, 8, 16, 32, etc) */
#define             ESAL_GE_MEM_ALIGNED_CHECK(value, req_align)                             \
                        (((UINT16)(value) & ((UINT16)(req_align) - (UINT16)1)) == (UINT16)0)

/* Macro used to align a data pointer to next address that meets the specified
   required alignment.
   NOTE:  The required alignment must be a power of 2 (2, 4, 8, 16, 32, etc) */
#define             ESAL_GE_MEM_PTR_ALIGN(ptr_addr, req_align)                              \
                        ((ESAL_GE_MEM_ALIGNED_CHECK(ptr_addr, req_align)) ? (VOID *)ptr_addr : \
                         (VOID *)(((UINT16)(ptr_addr) & (UINT16)(~((req_align) - 1))) + (UINT16)(req_align)))

#elif (ESAL_TS_DATA_PTR_SIZE <= 32)

/* Macro used to check if a value is aligned to the required boundary.
   Returns NU_TRUE if aligned; NU_FALSE if not aligned
   NOTE:  The required alignment must be a power of 2 (2, 4, 8, 16, 32, etc) */
#define             ESAL_GE_MEM_ALIGNED_CHECK(value, req_align)                             \
                        (((UINT32)(value) & ((UINT32)(req_align) - (UINT32)1)) == (UINT32)0)

/* Macro used to align a data pointer to next address that meets the specified
   required alignment.
   NOTE:  The required alignment must be a power of 2 (2, 4, 8, 16, 32, etc) */
#define             ESAL_GE_MEM_PTR_ALIGN(ptr_addr, req_align)                              \
                        ((ESAL_GE_MEM_ALIGNED_CHECK(ptr_addr, req_align)) ? (VOID *)ptr_addr : \
                         (VOID *)(((UINT32)(ptr_addr) & (UINT32)(~((req_align) - 1))) + (UINT32)(req_align)))

#elif (ESAL_TS_DATA_PTR_SIZE <= 64)

#if (ESAL_TS_64BIT_SUPPORT == NU_TRUE)

/* Macro used to check if a value is aligned to the required boundary.
   Returns NU_TRUE if aligned; NU_FALSE if not aligned
   NOTE:  The required alignment must be a power of 2 (2, 4, 8, 16, 32, etc) */
#define             ESAL_GE_MEM_ALIGNED_CHECK(value, req_align)                             \
                        (((UINT64)(value) & ((UINT64)(req_align) - (UINT64)1)) == (UINT64)0)

/* Macro used to align a data pointer to next address that meets the specified
   required alignment.
   NOTE:  The required alignment must be a power of 2 (2, 4, 8, 16, 32, etc) */
#define             ESAL_GE_MEM_PTR_ALIGN(ptr_addr, req_align)                              \
                        ((ESAL_GE_MEM_ALIGNED_CHECK(ptr_addr, req_align)) ? (VOID *)ptr_addr : \
                         (VOID *)(((UINT64)(ptr_addr) & (UINT64)(~((req_align) - 1))) + (UINT64)(req_align)))

#else

/* Generate error - toolset doesn't support 64-bit operations */
#error          Toolset does not support 64-bit operations (esal_ts_cfg.h)

#endif  /* ESAL_TS_64BIT_SUPPORT == NU_TRUE */

#endif  /* ESAL_TS_DATA_PTR_SIZE <= 16 */

/* Define memory clear */
#define ESAL_GE_MEM_Clear(x,y)              memset(x,0,y)
#define ESAL_GE_MEM_Set(x,y,z)              memset(x,y,z)
#define ESAL_GE_MEM_Copy(dst, src, size)    memcpy(dst, src, size)

/* Define enumerated type for cache types */
typedef enum
{
    ESAL_NOCACHE,
    ESAL_WRITEBACK,
    ESAL_WRITETHROUGH

} ESAL_GE_CACHE_TYPE;

/* Define enumerated type for memory types */
typedef enum
{
    ESAL_ROM,
    ESAL_RAM,
    ESAL_MEM_MAPPED,
    ESAL_IO_MAPPED,
    ESAL_SHARED_RAM,
    ESAL_TLB_MEM

} ESAL_GE_MEMORY_TYPE;

/* Define error value for memory related functions */
#define             ESAL_GE_MEM_ERROR                       0xFFFFFFFFUL

/* Define values for memory access types */
#define             ESAL_INST                               0x00000001UL
#define             ESAL_DATA                               0x00000002UL
#define             ESAL_INST_AND_DATA                      (ESAL_INST|ESAL_DATA)

/* Define generic structure used to define memory region */
typedef struct
{
    VOID                    *physical_start_addr;
    VOID                    *virtual_start_addr;
    UINT32                  size;
    ESAL_GE_CACHE_TYPE      cache_type;
    ESAL_GE_MEMORY_TYPE     mem_type;
    UINT32                  access_type;

} ESAL_GE_MEM_REGION;

/* Externally accessible global data */
extern  UINT32  ESAL_DP_MEM_Num_Regions;

/* Externally accessible global data */
extern const  ESAL_GE_MEM_REGION            ESAL_DP_MEM_Region_Data[ESAL_DP_MEM_NUM_REGIONS];

/* Define all memory related function prototypes */
UINT32              ESAL_GE_MEM_Remaining_Size_Get(VOID *start_addr);
VOID                *ESAL_GE_MEM_Next_Match_Find(VOID                *start_addr,
                                                 ESAL_GE_CACHE_TYPE  cache_type,
                                                 ESAL_GE_MEMORY_TYPE mem_type,
                                                 UINT32              access_type);
VOID                *ESAL_TS_MEM_First_Avail_Get(VOID);
VOID                ESAL_TS_MEM_BSS_Clear(VOID);
VOID                ESAL_TS_MEM_ROM_To_RAM_Copy(VOID);

/* Define core cache memory macros to do nothing if cache is not available */
#if (ESAL_CO_CACHE_AVAILABLE == NU_FALSE)

#define             ESAL_CO_MEM_CACHE_ALL_INVALIDATE()
#define             ESAL_CO_MEM_ICACHE_ALL_INVALIDATE()
#define             ESAL_CO_MEM_DCACHE_ALL_INVALIDATE()
#define             ESAL_CO_MEM_ICACHE_INVALIDATE(addr, size)          (VOID)((UINT32)addr + size)
#define             ESAL_CO_MEM_DCACHE_INVALIDATE(addr, size)          (VOID)((UINT32)addr + size)
#define             ESAL_CO_MEM_DCACHE_ALL_FLUSH_INVAL()
#define             ESAL_CO_MEM_DCACHE_FLUSH_INVAL(addr, size)         (VOID)((UINT32)addr + size)

#endif /* ESAL_CO_CACHE_AVAILABLE == NU_FALSE */

/* Define processor cache memory macros to do nothing if cache is not available */
#if (ESAL_PR_CACHE_AVAILABLE == NU_FALSE)

#define             ESAL_PR_MEM_CACHE_ALL_INVALIDATE()
#define             ESAL_PR_MEM_ICACHE_ALL_INVALIDATE()
#define             ESAL_PR_MEM_DCACHE_ALL_INVALIDATE()
#define             ESAL_PR_MEM_ICACHE_INVALIDATE(addr, size)          (VOID)((UINT32)addr + size)
#define             ESAL_PR_MEM_DCACHE_INVALIDATE(addr, size)          (VOID)((UINT32)addr + size)
#define             ESAL_PR_MEM_DCACHE_ALL_FLUSH_INVAL()
#define             ESAL_PR_MEM_DCACHE_FLUSH_INVAL(addr, size)         (VOID)((UINT32)addr + size)

#endif /* ESAL_PR_CACHE_AVAILABLE == NU_FALSE */

/* Generic defines used for cache related routines */
#define             ESAL_GE_MEM_CACHE_ALL_INVALIDATE()                \
                        ESAL_CO_MEM_CACHE_ALL_INVALIDATE();           \
                        ESAL_PR_MEM_CACHE_ALL_INVALIDATE()

#define             ESAL_GE_MEM_ICACHE_ALL_INVALIDATE()               \
                        ESAL_CO_MEM_ICACHE_ALL_INVALIDATE();          \
                        ESAL_PR_MEM_ICACHE_ALL_INVALIDATE()

#define             ESAL_GE_MEM_DCACHE_ALL_INVALIDATE()               \
                        ESAL_CO_MEM_DCACHE_ALL_INVALIDATE();          \
                        ESAL_PR_MEM_DCACHE_ALL_INVALIDATE()

#define             ESAL_GE_MEM_ICACHE_INVALIDATE(addr, size)         \
                        ESAL_CO_MEM_ICACHE_INVALIDATE(addr, size);    \
                        ESAL_PR_MEM_ICACHE_INVALIDATE(addr, size)

#define             ESAL_GE_MEM_DCACHE_INVALIDATE(addr, size)         \
                        ESAL_CO_MEM_DCACHE_INVALIDATE(addr, size);    \
                        ESAL_PR_MEM_DCACHE_INVALIDATE(addr, size)

#define             ESAL_GE_MEM_DCACHE_ALL_FLUSH_INVAL()              \
                        ESAL_CO_MEM_DCACHE_ALL_FLUSH_INVAL();         \
                        ESAL_PR_MEM_DCACHE_ALL_FLUSH_INVAL()

#define             ESAL_GE_MEM_DCACHE_FLUSH_INVAL(addr, size)        \
                        ESAL_CO_MEM_DCACHE_FLUSH_INVAL(addr, size);   \
                        ESAL_PR_MEM_DCACHE_FLUSH_INVAL(addr, size)

/**********************************************************************************/
/*                  Debug                                                         */
/**********************************************************************************/

/* Map generic opcode type to architecture specific opcode type */
typedef             ESAL_AR_DBG_OPCODE              ESAL_GE_DBG_OPCODE;
typedef             ESAL_AR_DBG_REG                 ESAL_GE_DBG_REG;

/* Define Stack Frame types */
typedef enum _esal_ge_dbg_stack_frame_type_enum
{
    ESAL_GE_DBG_STACK_FRAME_TYPE_THREAD,
    ESAL_GE_DBG_STACK_FRAME_TYPE_EXCEPTION

} ESAL_GE_DBG_STACK_FRAME_TYPE;

/* Map generic APIs to lower-level components */
#define             ESAL_GE_DBG_Reg_Read            ESAL_AR_DBG_Reg_Read
#define             ESAL_GE_DBG_Reg_Write           ESAL_AR_DBG_Reg_Write
#define             ESAL_GE_DBG_Opcode_Read         ESAL_AR_DBG_Opcode_Read
#define             ESAL_GE_DBG_Opcode_Write        ESAL_AR_DBG_Opcode_Write
#define             ESAL_GE_DBG_Opcode_Brk_Get      ESAL_AR_DBG_Opcode_Brk_Get
#define             ESAL_GE_DBG_Opcode_Nop_Get      ESAL_AR_DBG_Opcode_Nop_Get
#define             ESAL_GE_DBG_Step_Addr_Get       ESAL_AR_DBG_Step_Addr_Get
#define             ESAL_GE_DBG_Hardware_Step       ESAL_AR_DBG_Hardware_Step
#define             ESAL_GE_DBG_Get_Support_Flags   ESAL_AR_DBG_Get_Support_Flags
#define             ESAL_GE_DBG_Int_Read            ESAL_AR_DBG_Int_Read
#define             ESAL_GE_DBG_Int_Write           ESAL_AR_DBG_Int_Write
#define             ESAL_GE_DBG_Int_Enable          ESAL_AR_DBG_Int_Enable
#define             ESAL_GE_DBG_Int_Disable         ESAL_AR_DBG_Int_Disable

/* Provide means of causing an immediate breakpoint. */
#define             ESAL_GE_DBG_BREAK_EXECUTE       ESAL_TS_RTE_UNDEF_BRK_EXECUTE

/* Define all debugging related external variables */
extern INT          ESAL_GE_DBG_Debug_Operation;

/* Define all debugging related function prototypes */
VOID                ESAL_GE_DBG_Terminate(VOID);
VOID                ESAL_AR_DBG_Terminate(VOID);
INT                 ESAL_AR_DBG_Set_Protocol(UINT8  dbg_prot, UINT8*    stk_ptr, UINT8* pc);
INT                 ESAL_AR_DBG_Reg_Read(VOID *p_stack_frame, INT stack_frame_type, INT reg_no, ESAL_GE_DBG_REG *reg_val);
INT                 ESAL_AR_DBG_Reg_Write(VOID *p_stack_frame, INT stack_frame_type, INT reg_no, ESAL_GE_DBG_REG *reg_val);
INT                 ESAL_AR_DBG_Reg_Block_Read(VOID *p_stack_frame, INT stack_frame_type, VOID* reg_buff, UINT32* blk_size);
INT                 ESAL_AR_DBG_Reg_Block_Write(VOID *p_stack_frame, INT stack_frame_type, VOID* reg_buff);
INT                 ESAL_AR_DBG_Reg_Expedite_Read(VOID *p_stack_frame, INT stack_frame_type, VOID* reg_buff, UINT32* blk_size);
ESAL_GE_DBG_OPCODE  ESAL_AR_DBG_Opcode_Read(VOID *read_addr);
VOID                ESAL_AR_DBG_Opcode_Write(VOID *write_addr, ESAL_GE_DBG_OPCODE value);
ESAL_GE_DBG_OPCODE  ESAL_AR_DBG_Opcode_Nop_Get(VOID *addr);
ESAL_GE_DBG_OPCODE  ESAL_AR_DBG_Opcode_Brk_Get(VOID *addr);
VOID                *ESAL_AR_DBG_Step_Addr_Get(VOID *addr, VOID *p_stack_frame, INT stack_frame_type);
INT                 ESAL_AR_DBG_Hardware_Step(VOID * p_stack_frame, INT stack_frame_type);
INT                 ESAL_AR_DBG_Get_Support_Flags(UINT32 * flags);
INT                 ESAL_AR_DBG_Int_Read(VOID * p_stack_frame, INT stack_frame_type, UINT32 * int_state);
INT                 ESAL_AR_DBG_Int_Write(VOID * p_stack_frame, INT stack_frame_type, UINT32 int_state);
INT                 ESAL_AR_DBG_Int_Enable(VOID * p_stack_frame, INT stack_frame_type);
INT                 ESAL_AR_DBG_Int_Disable(VOID * p_stack_frame, INT stack_frame_type);

/**********************************************************************************/
/*                  Interrupt Controller                                          */
/**********************************************************************************/

/* Map generic APIs to lower-level component */
#define             ESAL_GE_INT_FAST_ALL_ENABLE         ESAL_AR_INT_FAST_ALL_ENABLE
#define             ESAL_GE_INT_FAST_ALL_DISABLE        ESAL_AR_INT_FAST_ALL_DISABLE
#define             ESAL_GE_INT_ALL_DISABLE             ESAL_AR_INT_ALL_DISABLE
#define             ESAL_GE_INT_ALL_RESTORE             ESAL_AR_INT_ALL_RESTORE
#define             ESAL_GE_INT_CONTROL_VARS            ESAL_AR_INT_CONTROL_VARS
#define             ESAL_GE_INT_BITS_SET                ESAL_AR_INT_BITS_SET

/* Define enumerated type for interrupt trigger types */
typedef enum
{
    ESAL_TRIG_NOT_SUPPORTED,
    ESAL_TRIG_RISING_EDGE,
    ESAL_TRIG_FALLING_EDGE,
    ESAL_TRIG_LEVEL_LOW,
    ESAL_TRIG_LEVEL_HIGH,
    ESAL_TRIG_RISING_FALLING_EDGES,
    ESAL_TRIG_HIGH_LOW_RISING_FALLING_EDGES

} ESAL_GE_INT_TRIG_TYPE;

/* Define constant to use if priorities being assigned to interrupt sources
   is not supported */
#define             ESAL_PRI_NOT_SUPPORTED              (INT)0x00000FFF

/* Define max vector ID if no off-chip interrupt controller */
#if (ESAL_DP_INTERRUPTS_AVAILABLE == NU_FALSE)

/* Max vector ID is processor delimiter */
#undef              ESAL_DP_INT_VECTOR_ID_DELIMITER
#define             ESAL_DP_INT_VECTOR_ID_DELIMITER         ESAL_PR_INT_VECTOR_ID_DELIMITER

#endif  /* ESAL_DP_INTERRUPTS_AVAILABLE == NU_FALSE */

/* Define macro used to get maximum ESAL interrupt vector ID */
#define             ESAL_GE_INT_MAX_VECTOR_ID_GET()     (ESAL_DP_INT_VECTOR_ID_DELIMITER-1)

/* Check if processor level macro exists to wait for an interrupt */
#ifdef              ESAL_PR_INT_WAIT

/* Set generic macro to use the processor specific macro to wait for an interrupt */
#define             ESAL_GE_INT_WAIT                    ESAL_PR_INT_WAIT

#else

/* Define generic way to wait for an interrupt */
#define             ESAL_GE_INT_WAIT()                  while(1){}

#endif  /* ESAL_PR_INT_WAIT */

/* Define all externally accessible, interrupt related function prototypes */
VOID                ESAL_GE_INT_All_Disable(VOID);
INT                 ESAL_GE_INT_Enable(INT vector_id,
                                       ESAL_GE_INT_TRIG_TYPE trigger_type,
                                       INT priority);
INT                 ESAL_GE_INT_Disable(INT vector_id);
INT                 ESAL_AR_INT_Enable(INT vector_id,
                                       ESAL_GE_INT_TRIG_TYPE trigger_type,
                                       INT priority);
INT                 ESAL_AR_INT_Disable(INT vector_id);
VOID                ESAL_PR_INT_All_Disable(VOID);
INT                 ESAL_PR_INT_Enable(INT vector_id,
                                       ESAL_GE_INT_TRIG_TYPE trigger_type,
                                       INT priority);
INT                 ESAL_PR_INT_Disable(INT vector_id);
VOID                ESAL_DP_INT_All_Disable(VOID);
INT                 ESAL_DP_INT_Enable(INT vector_id,
                                       ESAL_GE_INT_TRIG_TYPE trigger_type,
                                       INT priority);
INT                 ESAL_DP_INT_Disable(INT vector_id);

/**********************************************************************************/
/*                  Interrupt Service                                             */
/**********************************************************************************/

/* Externally referenced global data */
extern VOID         **(*ESAL_GE_ISR_OS_Entry)(INT vector, VOID *stack_ptr);
extern VOID         (*ESAL_GE_ISR_OS_Nested_Entry)(INT vector);
extern VOID         (*ESAL_GE_ISR_Interrupt_Handler[])(INT vector);
extern VOID         (*ESAL_GE_ISR_Exception_Handler[])(INT except_num, VOID *frame_ptr);
extern INT          ESAL_GE_ISR_Executing;
extern VOID         *ESAL_GE_ISR_Interrupt_Vector_Data[ESAL_DP_INT_VECTOR_ID_DELIMITER];
extern VOID         *ESAL_GE_ISR_Exception_Vector_Data[ESAL_AR_EXCEPT_VECTOR_ID_DELIMITER];
extern VOID         (*ESAL_GE_ISR_Execute_Hook)(INT vector);

/* Define macro used to return back to the OS after interrupt handling.  If the OS
   must be returned to instead of returning to the point of interrupt, this macro
   will perform this operation based on the architectural requirements.
   NOTE:  Some architectures require a "return from interrupt" to be execute in
          order for the execution state to be restored when returning to the
          OS.  This is configured in esal_ar_cfg.h */
#if (ESAL_AR_ISR_RTI_MANDATORY == NU_FALSE)

/* No special requirements for this architecture - simply expand this macro into
   a function call */
#define             ESAL_GE_ISR_OS_RETURN(os_return_func_ptr)                                   \
                        os_return_func_ptr()

#else

/* Define function prototype for architecture specific return from ISR function */
VOID                ESAL_AR_ISR_Return(VOID (*)(VOID));

/* This architecture must return to the OS using "return from interrupt" or equivalent
   method - expand this macro to call ESAL service to perform this function */
#define             ESAL_GE_ISR_OS_RETURN(os_return_func_ptr)                                   \
                        ESAL_AR_ISR_Return(os_return_func_ptr)

#endif  /* ESAL_AR_ISR_RTI_MANDATORY == NU_FALSE */

/* Define macro used to determine if within interrupt service routine */
#ifndef             ESAL_GE_ISR_EXECUTING
#define             ESAL_GE_ISR_EXECUTING()                     (ESAL_GE_ISR_Executing > 0)
#endif          /* !ESAL_GE_ISR_EXECUTING */

/* Define macros used to identify start / end of interrupt service routine */
#ifndef             ESAL_GE_ISR_START

#if (ESAL_AR_ISR_INCREMENT_IN_C == NU_TRUE)
#define             ESAL_GE_ISR_START()                         (ESAL_GE_ISR_Executing++)
#else 
#define             ESAL_GE_ISR_START()        
#endif          /*  ESAL_AR_ISR_INCREMENT_IN_C == NU_TRUE */ 
  
#endif          /* !ESAL_GE_ISR_START */

#ifndef             ESAL_GE_ISR_END
#define             ESAL_GE_ISR_END()                           ESAL_AR_INT_FAST_ALL_DISABLE();     \
                                                                (ESAL_GE_ISR_Executing--)
#endif          /* !ESAL_GE_ISR_END */

/* Define macros to get / set / execute interrupt service routine for ESAL interrupt vectors */
#define             ESAL_GE_ISR_EXECUTE_HOOK_SET(ptr)           ESAL_GE_ISR_Execute_Hook = (ptr)
#define             ESAL_GE_ISR_HANDLER_GET(num)                (ESAL_GE_ISR_Interrupt_Handler[(num)])
#define             ESAL_GE_ISR_HANDLER_SET(num,ptr)            ESAL_GE_ISR_Interrupt_Handler[(num)]=(ptr)

#if (CFG_NU_OS_SVCS_TRACE_CORE_TRACE_SUPPORT == NU_TRUE)

#define             ESAL_GE_ISR_HANDLER_EXECUTE(num)                                                \
                    {                                                                               \
                        /* Trace log */                                                             \
                        T_IDLE_EXIT();                                                              \
                                                                                                    \
                        /* Check if ESAL_GE_ISR_Wake_Up has been set */                             \
                        if (ESAL_GE_ISR_Execute_Hook != NU_NULL)                                    \
                        {                                                                           \
                            /* Execute ESAL_GE_ISR_Wake_Up function pointer */                      \
                            ESAL_GE_ISR_Execute_Hook(num);                                          \
                        }                                                                           \
                                                                                                    \
                        if ((num > ESAL_AR_INT_VECTOR_ID_DELIMITER)||(num == ESAL_GE_TMR_OS_VECTOR))\
                        {                                                                           \
                            /* Trace log */                                                         \
                            T_LISR_ENTRY(num);                                                      \
                                                                                                    \
                            /* Execute Interrupt Handler */                                         \
                            ESAL_GE_ISR_Interrupt_Handler[(num)](num);                              \
                                                                                                    \
                            /* Trace log */                                                         \
                            T_LISR_EXIT(num);                                                       \
                        }                                                                           \
                        else                                                                        \
                        {                                                                           \
                            /* Execute Interrupt Handler */                                         \
                            ESAL_GE_ISR_Interrupt_Handler[(num)](num);                              \
                        }                                                                           \
                    }
#else

#define             ESAL_GE_ISR_HANDLER_EXECUTE(num)                                                \
                    {                                                                               \
                        /* Check if ESAL_GE_ISR_Wake_Up has been set */                             \
                        if (ESAL_GE_ISR_Execute_Hook != NU_NULL)                                    \
                        {                                                                           \
                            /* Execute ESAL_GE_ISR_Wake_Up function pointer */                      \
                            ESAL_GE_ISR_Execute_Hook(num);                                          \
                        }                                                                           \
                        /* Execute Interrupt Handler */                                            	\
                        ESAL_GE_ISR_Interrupt_Handler[(num)](num);                                  \
                    }
#endif

/* Define macros for get / set exception handlers for ESAL exception vectors */
#define             ESAL_GE_EXCEPT_HANDLER_SET(num,ptr)         ESAL_GE_ISR_Exception_Handler[(num)]=(ptr)
#define             ESAL_GE_EXCEPT_HANDLER_GET(num)             (ESAL_GE_ISR_Exception_Handler[(num)])

/* Define macros to get / set data associated with each interrupt service routine vector */
#define             ESAL_GE_ISR_VECTOR_DATA_GET(vector)         ESAL_GE_ISR_Interrupt_Vector_Data[(vector)]
#define             ESAL_GE_ISR_VECTOR_DATA_SET(vector,data)    ESAL_GE_ISR_Interrupt_Vector_Data[(vector)]=(data)

/* Define macros for get / set data associated with each exception vector */
#define             ESAL_GE_EXCEPT_VECTOR_DATA_GET(vector)      ESAL_GE_ISR_Exception_Vector_Data[(vector)]
#define             ESAL_GE_EXCEPT_VECTOR_DATA_SET(vector,data) ESAL_GE_ISR_Exception_Vector_Data[(vector)]=(data)

/**********************************************************************************/
/*                  Run-time Environment                                          */
/**********************************************************************************/

/* Map generic APIs to lower-level component */
#define             ESAL_GE_RTE_SP_READ                         ESAL_TS_RTE_SP_READ
#define             ESAL_GE_RTE_Cxx_Info                        ESAL_TS_RTE_Cxx_Info
#ifndef             ESAL_TS_RTE_COMPILE_MEM_BARRIER
#define             ESAL_GE_RTE_COMPILE_MEM_BARRIER()
#else
#define             ESAL_GE_RTE_COMPILE_MEM_BARRIER()           ESAL_TS_RTE_COMPILE_MEM_BARRIER()
#endif

/* Define all run-time environment related function prototypes */
extern INT          (*ESAL_GE_RTE_Byte_Write)(INT);
extern INT          (*ESAL_GE_RTE_Byte_Read)(VOID);

/**********************************************************************************/
/*                  Stack                                                         */
/**********************************************************************************/

/* Define the maximum and minimum sizes of a stack frame */
#define             ESAL_GE_STK_MAX_FRAME_SIZE          sizeof(ESAL_AR_STK)
#define             ESAL_GE_STK_MIN_FRAME_SIZE          sizeof(ESAL_TS_STK)


/* Map generic APIs to lower-level component */
#define             ESAL_GE_STK_Unsolicited_Set         ESAL_AR_STK_Unsolicited_Set
#define             ESAL_GE_STK_Unsolicited_Restore     ESAL_AR_STK_Unsolicited_Restore
#define             ESAL_GE_STK_SYSTEM_SIZE             ESAL_AR_SYSTEM_STACK_SIZE
#define             ESAL_GE_STK_Solicited_Set           ESAL_TS_STK_Solicited_Set
#define             ESAL_GE_STK_Solicited_Restore       ESAL_TS_STK_Solicited_Restore
#define             ESAL_GE_STK_Solicited_Switch        ESAL_TS_STK_Solicited_Switch

/* Define all externally accessible, stack related function prototypes */
VOID                *ESAL_GE_STK_System_SP_Start_Get(VOID);
VOID                *ESAL_GE_STK_System_SP_End_Get(VOID);
VOID                *ESAL_AR_STK_Unsolicited_Set(VOID *start_addr,
                                                 VOID *end_addr,
                                                 VOID (*entry_function)(VOID));
VOID                ESAL_AR_STK_Unsolicited_Restore(VOID *stack_ptr);
VOID                ESAL_AR_STK_Unsolicited_Switch(VOID **stack_ptr);
VOID                *ESAL_TS_STK_Solicited_Set(VOID *start_addr,
                                               VOID *end_addr,
                                               VOID (*entry_function)(VOID));
VOID                ESAL_TS_STK_Solicited_Restore(VOID *stack_ptr);
VOID                ESAL_TS_STK_Solicited_Switch(VOID *call_back_param,
                                                 VOID (*call_back)(VOID),
                                                 VOID **stack_ptr);
VOID                ESAL_AR_STK_Startup_SP_Set(VOID);

/* Define externally accessed data */
extern VOID         (*ESAL_GE_STK_Unsol_Switch_OS_Entry)(VOID);
extern VOID         *ESAL_GE_STK_System_SP;
extern INT          ESAL_GE_STK_Unsol_Switch_Req;

/* Map generic system stack switch to appropriate ESAL function */
#define             ESAL_GE_STK_SYSTEM_SP_SET()         ESAL_TS_RTE_SP_WRITE(ESAL_GE_STK_System_SP)

/* Define macros to get offsets of various registers within the stack frame structures */
#define             ESAL_GE_STK_UNSOL_OFFSET(reg_name)  ((INT)&(((ESAL_AR_STK *)NU_NULL)->reg_name))
#define             ESAL_GE_STK_SOL_OFFSET(reg_name)    ((INT)&(((ESAL_TS_STK *)NU_NULL)->reg_name))

/* Define macro to set / clear flag showing an unsolicited switch is required */
#define             ESAL_GE_STK_UNSOL_SWITCH_ENABLE()   ESAL_GE_STK_Unsol_Switch_Req = NU_TRUE
#define             ESAL_GE_STK_UNSOL_SWITCH_DISABLE()  ESAL_GE_STK_Unsol_Switch_Req = NU_FALSE

/* Defines for stack types */
#define             ESAL_GE_STK_TS_TYPE                 0
#define             ESAL_GE_STK_AR_TYPE                 1

/* Define a generic alignment mask used to obtain a specified toolset required alignment */
#define             ESAL_GE_STK_ALIGN_MASK              (~(ESAL_TS_REQ_STK_ALIGNMENT - 1))

/* Define generic macro (based on architecture integer size) to read the
   stack type from top of stack */
#if             (ESAL_TS_INTEGER_SIZE == 16)

/* Read 16 bit stack type */
#define             ESAL_GE_STK_TYPE_GET(stack_ptr)     ESAL_GE_MEM_READ16(stack_ptr)

#elif           (ESAL_TS_INTEGER_SIZE == 32)

/* Read 32 bit stack type */
#define             ESAL_GE_STK_TYPE_GET(stack_ptr)     ESAL_GE_MEM_READ32(stack_ptr)

#elif           (ESAL_TS_INTEGER_SIZE == 64)

#if             (ESAL_TS_64BIT_SUPPORT == NU_TRUE)

/* Read 64 bit stack type */
#define             ESAL_GE_STK_TYPE_GET(stack_ptr)     ESAL_GE_MEM_READ64(stack_ptr)

#else

/* Generate error - tools don't support 64 bit data type */
#error          Toolset does not support 64-bit types (esal_ts_cfg.h)

#endif  /* ESAL_TS_64BIT_SUPPORT == NU_TRUE */

#else

/* Generate error - no valid method to retrieve stack type */
#error          No valid method to get stack type (esal_ge_stk_defs.h)

#endif  /* ESAL_TS_INTEGER_SIZE */

/* Define macro used to align stack end address based on data pointer size */
#if (ESAL_TS_DATA_PTR_SIZE <= 16)

/* Generic macro to align stack end address when stack grows down */
#define             ESAL_GE_STK_ALIGN(end_stk_addr)                                 \
                                (VOID *)((UINT16)end_stk_addr & (UINT16)ESAL_GE_STK_ALIGN_MASK)

#elif (ESAL_TS_DATA_PTR_SIZE <= 32)

/* Generic macro to align stack end address when stack grows down */
#define             ESAL_GE_STK_ALIGN(end_stk_addr)                                 \
                                (VOID *)((UINT32)end_stk_addr & (UINT32)ESAL_GE_STK_ALIGN_MASK)

#elif (ESAL_TS_DATA_PTR_SIZE <= 64)

#if (ESAL_TS_64BIT_SUPPORT == NU_TRUE)

/* Generic macro to align stack end address when stack grows down */
#define             ESAL_GE_STK_ALIGN(end_stk_addr)                                 \
                                (VOID *)((UINT64)end_stk_addr & (UINT64)ESAL_GE_STK_ALIGN_MASK)

#else

/* Generate error - toolset doesn't support 64-bit operations */
#error          Toolset does not support 64-bit operations (esal_ts_cfg.h)

#endif  /* ESAL_TS_64BIT_SUPPORT == NU_TRUE */

#endif  /* ESAL_TS_DATA_PTR_SIZE */

/* Check if toolset has specific macro for preventing return optimizations */
#ifdef              ESAL_TS_STK_NO_RETURN

/* Use toolset specific method for generic macro */
#define             ESAL_GE_STK_NO_RETURN               ESAL_TS_STK_NO_RETURN

#else

/* Define generic method to prevent return optimizations */
#define             ESAL_GE_STK_NO_RETURN()             while(1){}

#endif  /* ESAL_TS_STK_NO_RETURN */

/* Check if toolset solicited stack reset exists */
#ifndef         ESAL_TS_STK_SOLICITED_RESET

/* Define the solicited stack reset to just use the solicited stack set
   routine */
#define             ESAL_GE_STK_SOLICITED_RESET         ESAL_TS_STK_Solicited_Set

#else

/* Define the solicited stack reset to just use the solicited stack reset
   routine */
#define             ESAL_GE_STK_SOLICITED_RESET         ESAL_TS_STK_SOLICITED_RESET

#endif  /* !ESAL_TS_STK_SOLICITED_RESET */

/**********************************************************************************/
/*                  Timer                                                         */
/**********************************************************************************/

/* Define a generic macro for the OS clock rate and prescaler. */
#define             ESAL_GE_TMR_OS_CLOCK_RATE       ESAL_PR_TMR_OS_CLOCK_RATE
#define             ESAL_GE_TMR_OS_CLOCK_PRESCALE   ESAL_PR_TMR_OS_CLOCK_PRESCALE

/* Generic macro to calculate timer count value for timer with a given clock rate,
   clock prescale and period.
   NOTE:  This macro will work for count-down (underflow) timers and match timers
          (count-up or count-down timers).  If using a count-up timer that generates
          an interrupt upon overflow, the value produced by this macro must be
          subtracted from the maximum timer count. */
#define             ESAL_GE_TMR_COUNT_CALC(clock_rate, clock_prescale, ticksPerSec)         \
                                           (UINT32)( ((UINT32)clock_rate) /             \
                                                     ((UINT32)clock_prescale)  /        \
                                                      ((UINT32)ticksPerSec) )

/* Define all externally accessible, timer related function prototypes */
VOID                ESAL_GE_TMR_OS_Timer_Start(UINT32 ticks_per_sec);
VOID                ESAL_GE_TMR_OS_ISR_Register(VOID (*isr_func_ptr)(INT));
VOID                ESAL_AR_TMR_OS_Timer_Start(UINT32 ticks_per_sec);
VOID                ESAL_PR_TMR_OS_Timer_Start(UINT32 ticks_per_sec);

/* Defines for counter types (count-down or count-up) */
#define             ESAL_COUNT_DOWN             0
#define             ESAL_COUNT_UP               1

/**********************
* ARCHITECTURE OS TIMER
**********************/
#if (ESAL_AR_OS_TIMER_USED == NU_TRUE)

/* Define a generic macro that reads the architecture OS timer hardware count */
#define             ESAL_GE_TMR_OS_COUNT_READ()             ESAL_AR_TMR_OS_COUNT_READ()

/* Define generic macro for count direction using the architecture settings */
#define             ESAL_GE_TMR_OS_COUNT_DIR                ESAL_AR_TMR_OS_COUNT_DIR

/* Define generic macro for OS timer end-of-interrupt */
#define             ESAL_GE_TMR_OS_TIMER_EOI                ESAL_AR_TMR_OS_TIMER_EOI

/* Define generic macro for OS timer vector number */
#define             ESAL_GE_TMR_OS_VECTOR                   ESAL_AR_TMR_OS_VECTOR

/* Define generic macro for OS timer pending */
#define             ESAL_GE_TMR_OS_PENDING()                ESAL_AR_TMR_PENDING()

/* Define generic macros for PMS timer functionality. */
#define             ESAL_GE_TMR_PMS_COUNT_METHOD            ESAL_AR_TMR_PMS_COUNT_METHOD
#define             ESAL_GE_TMR_PMS_IS_TIMER_INT_PENDING()  ESAL_AR_TMR_PMS_IS_TIMER_INT_PENDING()
#define             ESAL_GE_TMR_PMS_GET_HW_TICK_CNT_VALUE() ESAL_AR_TMR_PMS_GET_HW_TICK_CNT_VALUE()

#if (ESAL_GE_TMR_PMS_COUNT_METHOD == ESAL_COUNT_UP)
/* Define generic macro for OS interval setting */
#define             ESAL_GE_TMR_PMS_SET_HW_TICK_INTERVAL(interval) ESAL_AR_TMR_PMS_SET_HW_TICK_INTERVAL(interval)
#endif

#if (ESAL_GE_TMR_PMS_COUNT_METHOD == ESAL_COUNT_DOWN)
/* Define for setting the processor tick value */
#define             ESAL_GE_TMR_PMS_SET_HW_TICK_VALUE(value) ESAL_AR_TMR_PMS_SET_HW_TICK_VALUE(value)
#endif

#ifdef ESAL_AR_TMR_PMS_ADJUST_HW_TICK_VALUE
/* Define for adjusting the processor tick value */
#define             ESAL_GE_TMR_PMS_ADJUST_HW_TICK_VALUE(adjustment) ESAL_AR_TMR_PMS_ADJUST_HW_TICK_VALUE(adjustment)
#endif

#else

/**********************
* PROCESSOR OS TIMER
**********************/

/* Define a generic macro that reads the processor OS timer hardware count */
#define             ESAL_GE_TMR_OS_COUNT_READ()             ESAL_PR_TMR_OS_COUNT_READ()

/* Define generic macro for count direction using the processor settings */
#define             ESAL_GE_TMR_OS_COUNT_DIR                ESAL_PR_TMR_OS_COUNT_DIR

/* Define generic macro for OS timer end-of-interrupt */
#define             ESAL_GE_TMR_OS_TIMER_EOI                ESAL_PR_TMR_OS_TIMER_EOI

/* Define generic macro for OS timer vector number */
#define             ESAL_GE_TMR_OS_VECTOR                   ESAL_PR_TMR_OS_VECTOR

/* Define generic macro for OS timer pending */
#define             ESAL_GE_TMR_OS_PENDING()                ESAL_PR_TMR_PENDING()

/* Define generic macros for PMS timer functionality. */
#define             ESAL_GE_TMR_PMS_COUNT_METHOD            ESAL_PR_TMR_PMS_COUNT_METHOD
#define             ESAL_GE_TMR_PMS_IS_TIMER_INT_PENDING()  ESAL_PR_TMR_PMS_IS_TIMER_INT_PENDING()
#define             ESAL_GE_TMR_PMS_GET_HW_TICK_CNT_VALUE() ESAL_PR_TMR_PMS_GET_HW_TICK_CNT_VALUE()
#if (ESAL_GE_TMR_PMS_COUNT_METHOD == ESAL_COUNT_UP)
/* Define generic macro for OS interval setting */
#define             ESAL_GE_TMR_PMS_SET_HW_TICK_INTERVAL(interval) ESAL_PR_TMR_PMS_SET_HW_TICK_INTERVAL(interval)
#endif

#if (ESAL_GE_TMR_PMS_COUNT_METHOD == ESAL_COUNT_DOWN)
/* Define for setting the processor tick value */
#define             ESAL_GE_TMR_PMS_SET_HW_TICK_VALUE(value) ESAL_PR_TMR_PMS_SET_HW_TICK_VALUE(value)
#endif

#ifdef ESAL_PR_TMR_PMS_ADJUST_HW_TICK_VALUE
/* Define for adjusting the processor tick value */
#define             ESAL_GE_TMR_PMS_ADJUST_HW_TICK_VALUE(adjustment) ESAL_PR_TMR_PMS_ADJUST_HW_TICK_VALUE(adjustment)
#endif

#endif  /* ESAL_AR_OS_TIMER_USED == NU_TRUE */

#ifdef              __cplusplus

/* End of C declarations */
}

#endif  /* __cplusplus */

#endif      /* ESAL_EXTR_H */

