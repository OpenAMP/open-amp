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
*  DESCRIPTION
*  
*       This file contains services constants common to both the
*       application and the actual Nucleus Services.
*
***********************************************************************/

/* Check to see if this file has been included already.  */

#ifndef         NU_SERVICES
#ifdef          __cplusplus
/* C declarations in C++     */
extern          "C" {
#endif
#define         NU_SERVICES


/**********************************************************************/
/*                         POSIX Services                             */
/**********************************************************************/
#ifdef CFG_NU_OS_SVCS_POSIX_ENABLE
#include        "services/posix.h"
#include        "services/stdint.h"
#include        "services/compiler.h"
#include        "services/config.h"
#include        "services/dirent.h"
#include        "services/errno.h"
#include        "services/posixinit.h"
#include        "services/fcntl.h"
#include        "services/file_util.h"
#include        "services/net/if.h"
#include        "services/inet/in.h"
#include        "services/netinet/in.h"
#include        "services/arpa/inet.h"
#include        "services/inttypes.h"
#ifdef CFG_NU_OS_NET_IPV6_ENABLE
#include        "services/inv6.h"
#include        "services/inv6_extr.h"
#endif /*CFG_NU_OS_NET_IPV6_ENABLE*/
#include        "services/limits.h"
#include        "services/sys/mman.h"
#include        "services/mqueue.h"
#include        "services/netdb.h"
#include        "services/aio.h"
#include        "services/aio_helper.h"
#include        "services/pdir.h"
#include        "services/pfile.h"
#include        "services/pfile_error.h"
#include        "services/pfileres.h"
#include        "services/pnet.h"
#include        "services/pnet_sr.h"
#include        "services/pprocres.h"
#include        "services/pres_defs.h"
#include        "services/psx_defs.h"
#include        "services/psx_extr.h"
#include        "services/ptdflt.h"
#include        "services/pthread.h"
#include        "services/putil.h"
#include        "services/rtldflt.h"
#include        "services/sched.h"
#include        "services/sys/select.h"
#include        "services/semaphore.h"
#include        "services/setjmp.h"
#include        "services/sigdflt.h"
#include        "services/signal.h"
#include        "services/sys/socket.h"
#include        "services/sys/stat.h"
#include        "services/stddef.h"
#include        "services/stdio.h"
#include        "services/stdiodflt.h"
#include        "services/stdlib.h"
#include        "services/string.h"
#include        "services/strings.h"
#include        "services/stropts.h"
#include        "services/sys/time.h"
#include        "services/sys/types.h"
#include        "services/sys/uio.h"
#include        "services/sys/un.h"
#include        "services/unistd.h"
#include        "services/utime.h"
#include        "services/sys/utsname.h"
#include        "services/xtype.h"
#include        "services/convert.h"
#include        "services/ctype.h"
#include        "services/fenv.h"
#include        "services/locale.h"
#include        "services/math.h"
#endif /* CFG_NU_OS_SVCS_POSIX_ENABLE */


/**********************************************************************/
/*                        CPU Include Files                           */
/**********************************************************************/
#include        "services/cpu_dvfs.h"
#include        "services/cpu_idle.h"
#include        "services/cpu_selfrefresh.h"

/**********************************************************************/
/*                           CXX Services                             */
/**********************************************************************/
#ifdef CFG_NU_OS_SVCS_CXX_ENABLE
#include        "services/cxx_rte.h"
#endif /* CFG_NU_OS_SVCS_CXX_ENABLE */


/**********************************************************************/
/*                         DEBUG Services                             */
/**********************************************************************/
#ifdef CFG_NU_OS_SVCS_DBG_ENABLE
#include        "services/dbg.h"
#include        "services/dbg_cfg.h"
#include        "services/dbg_com.h"
#include        "services/dbg_com_serial.h"
#include        "services/dbg_com_tcp.h"
#include        "services/dbg_eng.h"
#include        "services/dbg_eng_api.h"
#include        "services/dbg_eng_bkpt.h"
#include        "services/dbg_eng_evt.h"
#include        "services/dbg_eng_exec.h"
#include        "services/dbg_eng_mem.h"
#include        "services/dbg_eng_reg.h"
#include        "services/dbg_extr.h"
#include        "services/dbg_mem.h"
#include        "services/dbg_os.h"
#include        "services/dbg_rsp.h"
#include        "services/dbg_rsp_defs.h"
#include        "services/dbg_rsp_extr.h"
#include        "services/dbg_rsp_thd.h"
#include        "services/dbg_rsp_tmr.h"
#include        "services/dbg_rsp_utils.h"
#include        "services/dbg_set.h"
#include        "services/dbg_str.h"
#include        "services/dbg_sts.h"
#endif /* CFG_NU_OS_SVCS_DBG_ENABLE */

#ifdef CFG_NU_OS_SVCS_DBG_ADV_ENABLE
#include        "services/dbg_adv_extr.h"
#endif /* CFG_NU_OS_SVCS_DBG_ADV_ENABLE */

/**********************************************************************/
/*                      Trace and profile service                     */
/**********************************************************************/
#include        "services/nu_trace.h"

/**********************************************************************/
/*                         Power Services                             */
/**********************************************************************/
#ifdef CFG_NU_OS_SVCS_PWR_ENABLE
#include        "services/power_core.h"
#endif /* CFG_NU_OS_SVCS_PWR_ENABLE */


/**********************************************************************/
/*                        Registry Services                           */
/**********************************************************************/
#ifdef CFG_NU_OS_SVCS_REGISTRY_ENABLE
#include        "services/reg_api.h"
#include        "services/reg_impl.h"
#include        "services/reg_impl_mem_node.h"
#include        "services/reg_status.h"
#endif /* CFG_NU_OS_SVCS_REGISTRY_ENABLE */


/**********************************************************************/
/*                   Runlevel Init Services                           */
/**********************************************************************/
#ifdef CFG_NU_OS_SVCS_INIT_ENABLE
#include        "services/runlevel_init.h"
#endif /* CFG_NU_OS_SVCS_INIT_ENABLE */


/**********************************************************************/
/*                        Syslog Services                             */
/**********************************************************************/
#ifdef CFG_NU_OS_SVCS_SYSLOG_ENABLE
#include        "services/syslog_extern.h"
#endif /* CFG_NU_OS_SVCS_SYSLOG_ENABLE */


/**********************************************************************/
/*                         SHELL Services                             */
/**********************************************************************/
#ifdef CFG_NU_OS_SVCS_SHELL_ENABLE
#include        "services/shell_extern.h"
#endif /* CFG_NU_OS_SVCS_SHELL_ENABLE */

#ifdef          __cplusplus

/* End of C declarations */
}

#endif  /* __cplusplus */

#endif  /* !NU_SERVICES */

