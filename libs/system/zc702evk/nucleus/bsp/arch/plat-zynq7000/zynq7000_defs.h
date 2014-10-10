/***********************************************************************
*
*             Copyright 2012 Mentor Graphics Corporation
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
*       zynq7000_defs.h
*
*   DESCRIPTION
*
*       This file contains required configuration settings for the
*       given processor.  These configuration settings are used by
*       generic ESAL components and can be used by external components.
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

#ifndef         ZYNQ7000_DEFS_H
#define         ZYNQ7000_DEFS_H

/* Define processor execution endianess
   (ESAL_LITTLE_ENDIAN or ESAL_BIG_ENDIAN) */
#define         ESAL_PR_ENDIANESS                       ESAL_LITTLE_ENDIAN

/* Define processor cache availability
   NOTE:  A differentiation is made in ESAL between cache that
          is contained on a processor and cache that is
          inherent as part of a core (L2 vs L1 cache). */
#define         ESAL_PR_CACHE_AVAILABLE                 NU_FALSE

/* Define if an interrupt controller exists on the processor and
   controlling / handling of interrupts from this interrupt controller must
   be accommodated for.  Setting this to NU_FALSE means processor level interrupts
   will NOT be controlled or handled.  Setting this to NU_TRUE means processor level
   interrupts will be controlled and handled */
#define         ESAL_PR_INTERRUPTS_AVAILABLE            NU_TRUE

/* Define if Interrupt Affinity support is available */
#define         ESAL_PR_INT_AFFINITY_AVAILABLE          NU_TRUE

#ifndef         ESAL_CO_PERIPH_BASE
#define         ESAL_CO_PERIPH_BASE                     0xF8F00000
#endif

/* Define EB primary interrupt controller base registers */
#define         ESAL_PR_INT_GIC_CPU_BASE                (ESAL_CO_PERIPH_BASE + 0x00000100)
#define         ESAL_PR_INT_GIC_DIST_BASE               (ESAL_CO_PERIPH_BASE + 0x00001000)

/* CPU Interface Register Offsets */
#define         ESAL_PR_INT_GIC_CPU_CTRL                0x00
#define         ESAL_PR_INT_GIC_CPU_PRIORITY            0x04
#define         ESAL_PR_INT_GIC_CPU_POINT               0x08
#define         ESAL_PR_INT_GIC_CPU_ACK                 0x0c
#define         ESAL_PR_INT_GIC_CPU_ENDINT              0x10
#define         ESAL_PR_INT_GIC_CPU_RUNNING             0x14
#define         ESAL_PR_INT_IC_CPU_HIGHEST_PENDING      0x18
#define         ESAL_PR_INT_IC_CPU_NON_SECURE_POINT     0x1C
#define         ESAL_PR_INT_IC_CPU_IMPLEMENTOR          0xFC

/* Distribution Register Offsets */
#define         ESAL_PR_INT_GIC_DIST_CTRL               0x000
#define         ESAL_PR_INT_GIC_DIST_CTR                0x004
#define         ESAL_PR_INT_GIC_DIST_ISR                0x080
#define         ESAL_PR_INT_GIC_DIST_ENABLE_SET         0x100
#define         ESAL_PR_INT_GIC_DIST_ENABLE_CLEAR       0x180
#define         ESAL_PR_INT_GIC_DIST_PENDING_SET        0x200
#define         ESAL_PR_INT_GIC_DIST_PENDING_CLEAR      0x280
#define         ESAL_PR_INT_GIC_DIST_ACTIVE_BIT         0x300
#define         ESAL_PR_INT_GIC_DIST_PRI                0x400
#define         ESAL_PR_INT_GIC_DIST_TARGET             0x800
#define         ESAL_PR_INT_GIC_DIST_CONFIG             0xC00
#define         ESAL_PR_INT_GIC_DIST_PPI_STATUS         0xD00
#define         ESAL_PR_INT_GIC_DIST_SPI_STATUS         0xD04
#define         ESAL_PR_INT_GIC_DIST_SOFTINT            0xF00

/* Define value to disable all interrupts */
#define         ESAL_PR_INT_IRQ_DISABLE_ALL             0x00000000

/* Define value to enable interrupts on cpu */
#define         ESAL_PR_INT_CPU_ENABLE                  0x00000001
#define         ESAL_PR_INT_DIST_ENABLE                 0x00000001

/* Define Interrupt Ack Mask */
#define         ESAL_PR_INT_ACK_MASK                    0x000003FF

/* Define Spurious Int value */
#define         ESAL_PR_INT_SPURIOUS_INT               1023

/* Define ESAL interrupt vector IDs for this processor.
   These IDs match up with processor interrupts.
   Values correspond to the index of entries in ESAL_GE_ISR_Interrupt_Handler[].
   Names are of the form ESAL_PR_<Name>_INT_VECTOR_ID, where <Name> comes
   directly from the hardware documentation */

/* Private Software Generated Interrupts (SGI). */
#define     ESAL_PR_SOFTWARE0_INT_VECTOR_ID         (ESAL_AR_INT_VECTOR_ID_DELIMITER + 0)
#define     ESAL_PR_SOFTWARE1_INT_VECTOR_ID         (ESAL_AR_INT_VECTOR_ID_DELIMITER + 1)
#define     ESAL_PR_SOFTWARE2_INT_VECTOR_ID         (ESAL_AR_INT_VECTOR_ID_DELIMITER + 2)
#define     ESAL_PR_SOFTWARE3_INT_VECTOR_ID         (ESAL_AR_INT_VECTOR_ID_DELIMITER + 3)
#define     ESAL_PR_SOFTWARE4_INT_VECTOR_ID         (ESAL_AR_INT_VECTOR_ID_DELIMITER + 4)
#define     ESAL_PR_SOFTWARE5_INT_VECTOR_ID         (ESAL_AR_INT_VECTOR_ID_DELIMITER + 5)
#define     ESAL_PR_SOFTWARE6_INT_VECTOR_ID         (ESAL_AR_INT_VECTOR_ID_DELIMITER + 6)
#define     ESAL_PR_SOFTWARE7_INT_VECTOR_ID         (ESAL_AR_INT_VECTOR_ID_DELIMITER + 7)
#define     ESAL_PR_SOFTWARE8_INT_VECTOR_ID         (ESAL_AR_INT_VECTOR_ID_DELIMITER + 8)
#define     ESAL_PR_SOFTWARE9_INT_VECTOR_ID         (ESAL_AR_INT_VECTOR_ID_DELIMITER + 9)
#define     ESAL_PR_SOFTWARE10_INT_VECTOR_ID        (ESAL_AR_INT_VECTOR_ID_DELIMITER + 10)
#define     ESAL_PR_SOFTWARE11_INT_VECTOR_ID        (ESAL_AR_INT_VECTOR_ID_DELIMITER + 11)
#define     ESAL_PR_SOFTWARE12_INT_VECTOR_ID        (ESAL_AR_INT_VECTOR_ID_DELIMITER + 12)
#define     ESAL_PR_SOFTWARE13_INT_VECTOR_ID        (ESAL_AR_INT_VECTOR_ID_DELIMITER + 13)
#define     ESAL_PR_SOFTWARE14_INT_VECTOR_ID        (ESAL_AR_INT_VECTOR_ID_DELIMITER + 14)
#define     ESAL_PR_SOFTWARE15_INT_VECTOR_ID        (ESAL_AR_INT_VECTOR_ID_DELIMITER + 15)

/* Reserved Interrupt Vectors. */
#define     ESAL_PR_RESERVED16_INT_VECTOR_ID         (ESAL_AR_INT_VECTOR_ID_DELIMITER + 16)
#define     ESAL_PR_RESERVED17_INT_VECTOR_ID         (ESAL_AR_INT_VECTOR_ID_DELIMITER + 17)
#define     ESAL_PR_RESERVED18_INT_VECTOR_ID         (ESAL_AR_INT_VECTOR_ID_DELIMITER + 18)
#define     ESAL_PR_RESERVED19_INT_VECTOR_ID         (ESAL_AR_INT_VECTOR_ID_DELIMITER + 19)
#define     ESAL_PR_RESERVED20_INT_VECTOR_ID         (ESAL_AR_INT_VECTOR_ID_DELIMITER + 20)
#define     ESAL_PR_RESERVED21_INT_VECTOR_ID         (ESAL_AR_INT_VECTOR_ID_DELIMITER + 21)
#define     ESAL_PR_RESERVED22_INT_VECTOR_ID         (ESAL_AR_INT_VECTOR_ID_DELIMITER + 22)
#define     ESAL_PR_RESERVED23_INT_VECTOR_ID         (ESAL_AR_INT_VECTOR_ID_DELIMITER + 23)
#define     ESAL_PR_RESERVED24_INT_VECTOR_ID         (ESAL_AR_INT_VECTOR_ID_DELIMITER + 24)
#define     ESAL_PR_RESERVED25_INT_VECTOR_ID         (ESAL_AR_INT_VECTOR_ID_DELIMITER + 25)
#define     ESAL_PR_RESERVED26_INT_VECTOR_ID         (ESAL_AR_INT_VECTOR_ID_DELIMITER + 26)

/* Private Peripheral Interrupts (PPI). */
#define     ESAL_PR_GLOBTIMER_INT_VECTOR_ID         (ESAL_AR_INT_VECTOR_ID_DELIMITER + 27)
#define     ESAL_PR_NFIQ_INT_VECTOR_ID              (ESAL_AR_INT_VECTOR_ID_DELIMITER + 28)
#define     ESAL_PR_PRIVTIMER_INT_VECTOR_ID         (ESAL_AR_INT_VECTOR_ID_DELIMITER + 29)
#define     ESAL_PR_PRIVWATCHDOG_INT_VECTOR_ID      (ESAL_AR_INT_VECTOR_ID_DELIMITER + 30)
#define     ESAL_PR_NIRQ_INT_VECTOR_ID              (ESAL_AR_INT_VECTOR_ID_DELIMITER + 31)

/* Shared Peripheral Interrupts (SPI). */
#define     ESAL_PR_CPU0_INT_VECTOR_ID              (ESAL_AR_INT_VECTOR_ID_DELIMITER + 32)
#define     ESAL_PR_CPU1_INT_VECTOR_ID              (ESAL_AR_INT_VECTOR_ID_DELIMITER + 33)
#define     ESAL_PR_L2CACHE_INT_VECTOR_ID           (ESAL_AR_INT_VECTOR_ID_DELIMITER + 34)
#define     ESAL_PR_OCM_INT_VECTOR_ID               (ESAL_AR_INT_VECTOR_ID_DELIMITER + 35)
#define     ESAL_PR_RESERVED36_INT_VECTOR_ID        (ESAL_AR_INT_VECTOR_ID_DELIMITER + 36)
#define     ESAL_PR_PMU0_INT_VECTOR_ID              (ESAL_AR_INT_VECTOR_ID_DELIMITER + 37)
#define     ESAL_PR_PMU1_INT_VECTOR_ID              (ESAL_AR_INT_VECTOR_ID_DELIMITER + 38)
#define     ESAL_PR_XADC_INT_VECTOR_ID              (ESAL_AR_INT_VECTOR_ID_DELIMITER + 39)
#define     ESAL_PR_DVI_INT_VECTOR_ID               (ESAL_AR_INT_VECTOR_ID_DELIMITER + 40)
#define     ESAL_PR_SWDT_INT_VECTOR_ID              (ESAL_AR_INT_VECTOR_ID_DELIMITER + 41)
#define     ESAL_PR_TTC00_INT_VECTOR_ID             (ESAL_AR_INT_VECTOR_ID_DELIMITER + 42)
#define     ESAL_PR_TTC01_INT_VECTOR_ID             (ESAL_AR_INT_VECTOR_ID_DELIMITER + 43)
#define     ESAL_PR_RESERVED44_INT_VECTOR_ID        (ESAL_AR_INT_VECTOR_ID_DELIMITER + 44)
#define     ESAL_PR_DMACABORT_INT_VECTOR_ID         (ESAL_AR_INT_VECTOR_ID_DELIMITER + 45)
#define     ESAL_PR_DMAC0_INT_VECTOR_ID             (ESAL_AR_INT_VECTOR_ID_DELIMITER + 46)
#define     ESAL_PR_DMAC1_INT_VECTOR_ID             (ESAL_AR_INT_VECTOR_ID_DELIMITER + 47)
#define     ESAL_PR_DMAC2_INT_VECTOR_ID             (ESAL_AR_INT_VECTOR_ID_DELIMITER + 48)
#define     ESAL_PR_DMAC3_INT_VECTOR_ID             (ESAL_AR_INT_VECTOR_ID_DELIMITER + 49)
#define     ESAL_PR_SMC_INT_VECTOR_ID               (ESAL_AR_INT_VECTOR_ID_DELIMITER + 50)
#define     ESAL_PR_QUADSPI_INT_VECTOR_ID           (ESAL_AR_INT_VECTOR_ID_DELIMITER + 51)
#define     ESAL_PR_GPIO_INT_VECTOR_ID              (ESAL_AR_INT_VECTOR_ID_DELIMITER + 52)
#define     ESAL_PR_USB0_INT_VECTOR_ID              (ESAL_AR_INT_VECTOR_ID_DELIMITER + 53)
#define     ESAL_PR_ETHERNET0_INT_VECTOR_ID         (ESAL_AR_INT_VECTOR_ID_DELIMITER + 54)
#define     ESAL_PR_ETH0WAKEUP_INT_VECTOR_ID        (ESAL_AR_INT_VECTOR_ID_DELIMITER + 55)
#define     ESAL_PR_SDIO0_INT_VECTOR_ID             (ESAL_AR_INT_VECTOR_ID_DELIMITER + 56)
#define     ESAL_PR_I2C0_INT_VECTOR_ID              (ESAL_AR_INT_VECTOR_ID_DELIMITER + 57)
#define     ESAL_PR_SPI0_INT_VECTOR_ID              (ESAL_AR_INT_VECTOR_ID_DELIMITER + 58)
#define     ESAL_PR_UART0_INT_VECTOR_ID             (ESAL_AR_INT_VECTOR_ID_DELIMITER + 59)
#define     ESAL_PR_CAN0_INT_VECTOR_ID              (ESAL_AR_INT_VECTOR_ID_DELIMITER + 60)
#define     ESAL_PR_FPGA0_INT_VECTOR_ID             (ESAL_AR_INT_VECTOR_ID_DELIMITER + 61)
#define     ESAL_PR_FPGA1_INT_VECTOR_ID             (ESAL_AR_INT_VECTOR_ID_DELIMITER + 62)
#define     ESAL_PR_FPGA2_INT_VECTOR_ID             (ESAL_AR_INT_VECTOR_ID_DELIMITER + 63)
#define     ESAL_PR_FPGA3_INT_VECTOR_ID             (ESAL_AR_INT_VECTOR_ID_DELIMITER + 64)
#define     ESAL_PR_FPGA4_INT_VECTOR_ID             (ESAL_AR_INT_VECTOR_ID_DELIMITER + 65)
#define     ESAL_PR_FPGA5_INT_VECTOR_ID             (ESAL_AR_INT_VECTOR_ID_DELIMITER + 66)
#define     ESAL_PR_FPGA6_INT_VECTOR_ID             (ESAL_AR_INT_VECTOR_ID_DELIMITER + 67)
#define     ESAL_PR_FPGA7_INT_VECTOR_ID             (ESAL_AR_INT_VECTOR_ID_DELIMITER + 68)
#define     ESAL_PR_TTC10_INT_VECTOR_ID             (ESAL_AR_INT_VECTOR_ID_DELIMITER + 69)
#define     ESAL_PR_TTC11_INT_VECTOR_ID             (ESAL_AR_INT_VECTOR_ID_DELIMITER + 70)
#define     ESAL_PR_TTC12_INT_VECTOR_ID             (ESAL_AR_INT_VECTOR_ID_DELIMITER + 71)
#define     ESAL_PR_DMAC4_INT_VECTOR_ID             (ESAL_AR_INT_VECTOR_ID_DELIMITER + 72)
#define     ESAL_PR_DMAC5_INT_VECTOR_ID             (ESAL_AR_INT_VECTOR_ID_DELIMITER + 73)
#define     ESAL_PR_DMAC6_INT_VECTOR_ID             (ESAL_AR_INT_VECTOR_ID_DELIMITER + 74)
#define     ESAL_PR_DMAC7_INT_VECTOR_ID             (ESAL_AR_INT_VECTOR_ID_DELIMITER + 75)
#define     ESAL_PR_USB1_INT_VECTOR_ID              (ESAL_AR_INT_VECTOR_ID_DELIMITER + 76)
#define     ESAL_PR_ETHERNET1_INT_VECTOR_ID         (ESAL_AR_INT_VECTOR_ID_DELIMITER + 77)
#define     ESAL_PR_ETH1WAKEUP_INT_VECTOR_ID        (ESAL_AR_INT_VECTOR_ID_DELIMITER + 78)
#define     ESAL_PR_SDIO1_INT_VECTOR_ID             (ESAL_AR_INT_VECTOR_ID_DELIMITER + 79)
#define     ESAL_PR_I2C1_INT_VECTOR_ID              (ESAL_AR_INT_VECTOR_ID_DELIMITER + 80)
#define     ESAL_PR_SPI1_INT_VECTOR_ID              (ESAL_AR_INT_VECTOR_ID_DELIMITER + 81)
#define     ESAL_PR_UART1_INT_VECTOR_ID             (ESAL_AR_INT_VECTOR_ID_DELIMITER + 82)
#define     ESAL_PR_CAN1_INT_VECTOR_ID              (ESAL_AR_INT_VECTOR_ID_DELIMITER + 83)
#define     ESAL_PR_FPGA8_INT_VECTOR_ID             (ESAL_AR_INT_VECTOR_ID_DELIMITER + 84)
#define     ESAL_PR_FPGA9_INT_VECTOR_ID             (ESAL_AR_INT_VECTOR_ID_DELIMITER + 85)
#define     ESAL_PR_FPGA10_INT_VECTOR_ID            (ESAL_AR_INT_VECTOR_ID_DELIMITER + 86)
#define     ESAL_PR_FPGA11_INT_VECTOR_ID            (ESAL_AR_INT_VECTOR_ID_DELIMITER + 87)
#define     ESAL_PR_FPGA12_INT_VECTOR_ID            (ESAL_AR_INT_VECTOR_ID_DELIMITER + 88)
#define     ESAL_PR_FPGA13_INT_VECTOR_ID            (ESAL_AR_INT_VECTOR_ID_DELIMITER + 89)
#define     ESAL_PR_FPGA14_INT_VECTOR_ID            (ESAL_AR_INT_VECTOR_ID_DELIMITER + 90)
#define     ESAL_PR_FPGA15_INT_VECTOR_ID            (ESAL_AR_INT_VECTOR_ID_DELIMITER + 91)
#define     ESAL_PR_PARITY_INT_VECTOR_ID            (ESAL_AR_INT_VECTOR_ID_DELIMITER + 92)

/* Define the last ESAL interrupt vector ID for this processor + 1 */
#define         ESAL_PR_INT_VECTOR_ID_DELIMITER (ESAL_PR_PARITY_INT_VECTOR_ID + 1)

/* Define number of GIC priority registers */
#define     ESAL_PR_ISR_GIC_NUM_PRI_REG             16

/* Define number of GIC target registers */
#define     ESAL_PR_ISR_GIC_NUM_TARGET_REG          16

/* Define value to disable all interrupts */
#define     ESAL_PR_INT_DISABLE                     0x00000000

/* Define value to clear interrupt registers */
#define     ESAL_PR_INT_CLEAR                       0xFFFFFFFF

/* Define value to route interrupts to Core Tile 1 */
#define     ESAL_PR_ROUTE_CORE_TILE                 0x00000000

/* Define base address for status and system control registers */
#define     ESAL_PR_ISR_SSC_BASE                    0x10000000

/* Number of interrupts implemented on the EB */
#define     NO_OF_INTERRUPTS_IMPLEMENTED            96

/* Location to install the hardware (exception/interrupt) vector table when
   executing from RAM */
#if (CFG_NU_BSP_ZC702EVK_AMP_REMOTE == 1)
#define     ESAL_PR_ISR_VECTOR_TABLE_DEST_ADDR_RAM  0x00000000
#else
#define     ESAL_PR_ISR_VECTOR_TABLE_DEST_ADDR_RAM  0x10000000
#endif

/* Define timer register base address */
#define         ESAL_PR_TMR_TIMER_BASE_ADDR             (ESAL_CO_PERIPH_BASE + 0x00000600) /* Private Timer base address */

/* Define timer interrupt priority (0 - highest / 15 - lowest) */
#define         ESAL_PR_TMR_PRIORITY                    0

/* Define timer register offsets */
#define         ESAL_PR_TMR_TIMER_LOAD_OFFSET           0x00
#define         ESAL_PR_TMR_TIMER_VALUE_OFFSET          0x04
#define         ESAL_PR_TMR_TIMER_CTRL_OFFSET           0x08
#define         ESAL_PR_TMR_TIMER_INTCLR_OFFSET         0x0C

/* Bit defines for timer clear register */
#define         ESAL_PR_TMR_TIMER_CLR                   ESAL_GE_MEM_32BIT_SET(0)

/* Bit defines for timer control register */
#define         ESAL_PR_TMR_TIMER_CTRL_ENABLE           ESAL_GE_MEM_32BIT_SET(0)
#define         ESAL_PR_TMR_TIMER_CTRL_AUTO             ESAL_GE_MEM_32BIT_SET(1)
#define         ESAL_PR_TMR_TIMER_CTRL_IT               ESAL_GE_MEM_32BIT_SET(2)

/* Initialization values */
#define         ESAL_PR_TMR_TIMER_CTRL_INIT_VALUE       (ESAL_PR_TMR_TIMER_CTRL_ENABLE |    \
                                                         ESAL_PR_TMR_TIMER_CTRL_AUTO   |    \
                                                         ESAL_PR_TMR_TIMER_CTRL_IT)

#define         ESAL_PR_SYS_CTRL_OFFSET                 0x1000
#define         ESAL_PR_TMR_ENABLE                      0x00E2


/* Define the clock rate for the OS timer. This value should hold
   this relation MAIN_CPU_CLOCK =  ESAL_PR_TMR_OS_CLOCK_RATE * N
   where N should be greater or equal to 2. */

#define         ESAL_PR_TMR_OS_CLOCK_RATE               333000000UL

/* Define the clock prescaler for the OS timer
   NOTE:  The prescaler is used to adjust the processor clock rate to a lower clock
          rate suitable for the timer */
#define         ESAL_PR_TMR_OS_CLOCK_PRESCALE           1

/* The following definitions / macros / etc are only used if the processor
   is configured (this is only if the architecture is NOT configured to
   contain the OS timer in esal_ar_cfg.h) to use a processor level timer
   for the OS timer. */
#if (ESAL_AR_OS_TIMER_USED == NU_FALSE)

/* Define the processor OS timer type (count-down or count-up) */
#define         ESAL_PR_TMR_OS_COUNT_DIR                ESAL_COUNT_DOWN

/* Define for the processor OS timer interrupt vector */
#define         ESAL_PR_TMR_OS_VECTOR                   ESAL_PR_PRIVTIMER_INT_VECTOR_ID

/* Define a macro to read the processor OS timer hardware count.  The
   resultant size of the count must be 32-bits, regardless of the actual
   size of the timer used (8-bit, 16-bit, 32-bit, etc). */
#define         ESAL_PR_TMR_OS_COUNT_READ()                                                 \
                    ESAL_GE_MEM_READ32(ESAL_PR_TMR_TIMER_BASE_ADDR + ESAL_PR_TMR_TIMER_VALUE_OFFSET)

/* Define the EOI logic for the processor OS timer */
#define         ESAL_PR_TMR_OS_TIMER_EOI(vector)                                            \
                    ESAL_GE_MEM_WRITE32(ESAL_PR_TMR_TIMER_BASE_ADDR +                       \
                                        ESAL_PR_TMR_TIMER_INTCLR_OFFSET,                    \
                                        ESAL_PR_TMR_TIMER_CLR);

/* Define the logic for checking if interrupt pending */
#define         ESAL_PR_TMR_PENDING()                                                       \
                    (ESAL_GE_MEM_READ32(ESAL_PR_TMR_TIMER_BASE_ADDR +                       \
                     ESAL_PR_TMR_TIMER_INTCLR_OFFSET) & 0x1)


#endif  /* ESAL_PR_OS_TIMER_USED == NU_TRUE */

/* Define SCU register base address and offsets */
#define         ESAL_PR_MEM_SCU_BASE            (ESAL_CO_PERIPH_BASE)
#define         ESAL_PR_MEM_SCU_CTRL            0x00
#define         ESAL_PR_MEM_SCU_INVAL           0x0C

/* Define SCU register bits */
#define         ESAL_PR_MEM_SCU_CTRL_EN_BIT     ESAL_GE_MEM_32BIT_SET(0)
#define         ESAL_PR_MEM_SCU_INVAL_ALL       0xFFFFFFFF

/* Define auxiliary control unit SMP bit */
#define         ESAL_PR_MEM_AUX_CTRL_SMP_BIT    ESAL_GE_MEM_32BIT_SET(6)

#if (ESAL_PR_CACHE_AVAILABLE == NU_TRUE)

/* This macro invalidates all of the cache at the processor level. */
#define         ESAL_PR_MEM_CACHE_ALL_INVALIDATE()                          \
                {                                                           \
                }

/* This macro invalidates all of the instruction cache at the processor level. */
#define         ESAL_PR_MEM_ICACHE_ALL_INVALIDATE()                         \
                {                                                           \
                }

/* This macro invalidates all of the data cache at the processor level. */
#define         ESAL_PR_MEM_DCACHE_ALL_INVALIDATE()                         \
                {                                                           \
                }

/* This macro invalidates all instruction cache for the specified address
   range at the processor level. */
#define         ESAL_PR_MEM_ICACHE_INVALIDATE(addr, size)                   \
                {                                                           \
                }

/* This macro invalidates all data cache for the specified address
   range at the processor level. */
#define         ESAL_PR_MEM_DCACHE_INVALIDATE(addr, size)                   \
                {                                                           \
                }

/* This macro flushes all data cache to physical memory (writeback cache)
   and invalidates all data cache entries at the processor level. */
#define         ESAL_PR_MEM_DCACHE_ALL_FLUSH_INVAL()                        \
                {                                                           \
                }

/* This macro flushes all data cache to physical memory (writeback cache)
   for the given address range, then invalidates all data cache entries
   at the processor level. */
#define         ESAL_PR_MEM_DCACHE_FLUSH_INVAL(addr, size)                  \
                {                                                           \
                }

#endif  /* ESAL_PR_CACHE_AVAILABLE == NU_TRUE */

/* L2Cpl310 L2 cache controller base address. */
#define         ESAL_PR_L2CPL310_BASE                   0xF8F02000

/* L2Cpl310 L2 cache controller register offsets. */
#define         ESAL_PR_L2CPL310_REG1_CONTROL           0x100
#define         ESAL_PR_L2CPL310_REG1_AUX_CONTROL       0x104

/* L2Cpl310 L2 cache controller control register bit defines . */
#define         ESAL_PR_L2CPL310_REG1_CONTROL_ENABLE    0x1


#if (NU_SMP_CPU_COUNT > 1)

/* Define macros */
#define         ESAL_PR_SMP_CPU_ID_GET                      ESAL_PR_SMP_CPU_ID_Get

/* Inter-Processor Interrupt Vector Number */
#define         ESAL_PR_SMP_IPI_VECTOR_ID                   ESAL_PR_PRIVATE0_INT_VECTOR_ID
#define         ESAL_PR_SMP_IPI_TRIG                        ESAL_TRIG_LEVEL_HIGH
#define         ESAL_PR_SMP_IPI_PRIORITY                    0xE

/* IPI target filter, options */
#define         ESAL_PR_IPI_TARGET_MASK                     0
#define         ESAL_PR_IPI_TARGET_ALL_OTHERS               1
#define         ESAL_PR_IPI_TARGET_SELF                     2
#define         ESAL_PR_SMP_IPI_Clear(cpu_num)
VOID            ESAL_PR_SMP_IPI_Send (UINT32* cpu_mask);
UINT32          ESAL_PR_SMP_CPU_ID_Get(VOID);

#else /* For uni-core processor */
#define         ESAL_PR_SMP_CPU_ID_GET                      0
#define         ESAL_PR_SMP_IPI_Send(x)

#endif /* (NU_SMP_CPU_COUNT > 1) */

/* Define macros for interrupt affinity */
#define         ESAL_PR_NUM_PVT_INTS                        32


/*
 * MAX_TICKS_PER_INTERVAL is the maximum multiple of system ticks
 * to which the tick hardware timer can be set to.
 * MAX_TICKS_PER_INTERVAL = 2^(number of bits in tick counter)-1 / (counts per tick)
*/
#define MAX_TICKS_PER_INTERVAL          2147

/* Define for setting the processor tick value */
#define         ESAL_PR_TMR_TICK_VALUE_SET(value)                                   \
                    ESAL_GE_MEM_WRITE32((ESAL_PR_TMR_TIMER_BASE_ADDR +              \
                                         ESAL_PR_TMR_TIMER_LOAD_OFFSET), value)

/* Define the logic for setting the processor timer tick value */
#define         ESAL_PR_TMR_PMS_SET_HW_TICK_VALUE(interval)                            \
                    ESAL_GE_MEM_WRITE32((ESAL_PR_TMR_TIMER_BASE_ADDR + ESAL_PR_TMR_TIMER_LOAD_OFFSET), interval)

/* Define method for which PMS will work with the counter.  In most cases this will
   match the OS timer direction.  On some rarer cases it may be needed to differ, such
   cases include timers that don't start at 0 but count up. */
#define ESAL_PR_TMR_PMS_COUNT_METHOD        ESAL_PR_TMR_OS_COUNT_DIR

/*
 * ESAL_PR_TMR_PMS_IS_TIMER_INT_PENDING() checks whether a hardware tick timer interrupt is
 * pending at this time.
 * It is used to check if a race condition occurred, CPU woke up due to
 * other HW interrupt but a tick occurred between the interrupt and any
 * hardware tick counter sampling.
 */
#define ESAL_PR_TMR_PMS_IS_TIMER_INT_PENDING()  ESAL_GE_TMR_OS_PENDING()

/*
 * ESAL_PR_TMR_PMS_SET_HW_TICK_INTERVAL(interval) sets the hardware tick timer interval
 * It is used and required only for UP counting hardware timer counters.
 */
#if(ESAL_PR_TMR_PMS_COUNT_METHOD == ESAL_COUNT_UP)
#define ESAL_PR_TMR_PMS_SET_HW_TICK_INTERVAL(interval)
#endif

/*
 * ESAL_PR_TMR_PMS_GET_HW_TICK_CNT_VALUE() reads the current hardware tick timer counter value
 * This typically can be left mapped to ESAL_GE_TMR_OS_COUNT_READ
 */
#define ESAL_PR_TMR_PMS_GET_HW_TICK_CNT_VALUE()         ESAL_GE_TMR_OS_COUNT_READ()

/* Define for adjusting the processor tick value */
#define         ESAL_PR_TMR_TICK_ADJUST_SET(adjustment)                             \
                {                                                                   \
                    UINT32 _temp_ticks = ESAL_PR_TMR_PMS_GET_HW_TICK_CNT_VALUE();   \
                    /* Clear any pending timer interrupts */                        \
                    ESAL_GE_MEM_WRITE32(ESAL_PR_TMR_TIMER_BASE_ADDR +               \
                    ESAL_PR_TMR_TIMER_INTCLR_OFFSET, ESAL_PR_TMR_TIMER_CLR);        \
                    ESAL_GE_MEM_WRITE32((ESAL_PR_TMR_TIMER_BASE_ADDR +              \
                    ESAL_PR_TMR_TIMER_VALUE_OFFSET), (_temp_ticks + (adjustment))); \
                }

/*
 * ESAL_PR_TMR_PMS_ADJUST_HW_TICK_VALUE(adjustment) adjust the current hardware tick timer value
 * by the adjustment value. POSITIVE OR NEGATIVE adjustments must be handled.
 * ESAL_PR_TMR_PMS_ADJUST_HW_TICK_VALUE is used only with DOWN counting hardware timer counters
 * and is the preferred method for tick suppression (vs. ESAL_PMS_SET_HW_TICK_VALUE)
 */
#if(ESAL_PR_TMR_PMS_COUNT_METHOD == ESAL_COUNT_DOWN)
#define ESAL_PR_TMR_PMS_ADJUST_HW_TICK_VALUE(adjustment) ESAL_PR_TMR_TICK_ADJUST_SET(adjustment)
#endif

/* This macro sets the current hardware tick timer counter value
 * It is used and required only for DOWN counting hardware timer counters
 * and only if ESAL_PR_TMR_PMS_ADJUST_HW_TICK_VALUE is not defined.
 * ESAL_PR_TMR_PMS_SET_HW_TICK_VALUE should only be used if ESAL_PMS_ADJUST_HW_TICK function
 * in unachievable because it potentially introduces small tick drift
 * when the software does read-modify-write adjustments to the counter value.
 */

#if(ESAL_PR_TMR_PMS_COUNT_METHOD == ESAL_COUNT_DOWN)
#ifndef ESAL_PR_TMR_PMS_ADJUST_HW_TICK_VALUE
#define ESAL_PR_TMR_PMS_SET_HW_TICK_VALUE(value)        ESAL_PR_TMR_TICK_VALUE_SET(value)
#endif  /*ESAL_PR_TMR_PMS_ADJUST_HW_TICK_VALUE*/
#endif  /*(ESAL_PR_TMR_PMS_COUNT_METHOD == ESAL_COUNT_DOWN)*/


#endif /* ZYNQ7000_DEFS_H */
