/***********************************************************************
*
*             Copyright 2013 Mentor Graphics Corporation
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
*       platform.h
*
*   DESCRIPTION
*
*       This file is a wrapper header file that includes all the necessary
*       header files for the configured platform
*
*   DATA STRUCTURES
*
*       None
*
*   DEPENDENCIES
*
*       nucleus_gen_cfg.h
*       zynq7000_defs.h
*       zc702evk_defs.h
*       zedboard.h
*
***********************************************************************/

#ifndef PLATFORM_H
#define PLATFORM_H

/* Include configuration header file */
#include    "nucleus_gen_cfg.h"

/* Include header file for Xilinx ZYNQ-7000 architecture */
#include    "zynq7000_defs.h"

#ifdef  CFG_NU_BSP_ZC702EVK_ENABLE

/* Include header file for Xilinx ZC702 EVK Board */
#include  "bsp/zc702evk_defs.h"

#endif  /* CFG_NU_BSP_ZC702EVK_ENABLE */

#ifdef  CFG_NU_BSP_ZEDBOARD_ENABLE

/* Include header file for Xilinx ZC702 EVK Board */
#include  "bsp/zedboard.h"

#endif  /* CFG_NU_BSP_ZEDBOARD_ENABLE */


#endif  /* PLATFORM_H */
