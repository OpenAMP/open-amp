/*************************************************************************
*
*              Copyright 2010 Mentor Graphics Corporation
*                         All Rights Reserved.
*
* THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION WHICH IS
* THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS LICENSORS AND IS
* SUBJECT TO LICENSE TERMS.
*
*************************************************************************/

/*************************************************************************
*
*   DESCRIPTION
*
*       This file contains function prototypes of all functions
*       accessible to other components.
*
*************************************************************************/

/* Check to see if the file has been included already.  */
#ifndef DEV_MGR_H
#define DEV_MGR_H

#include        <string.h>

#ifdef          __cplusplus

/* C declarations in C++     */
extern          "C" {

#endif

#ifndef CFG_NU_OS_KERN_DEVMGR_MAX_DEV_LABEL_CNT
#define CFG_NU_OS_KERN_DEVMGR_MAX_DEV_LABEL_CNT         1
#endif

#ifndef CFG_NU_OS_KERN_DEVMGR_DISCOVERY_TASK_MAX_ID_CNT
#define CFG_NU_OS_KERN_DEVMGR_DISCOVERY_TASK_MAX_ID_CNT            1
#endif

#ifndef CFG_NU_OS_KERN_DEVMGR_DISCOVERY_TASK_ENABLE
#define CFG_NU_OS_KERN_DEVMGR_DISCOVERY_TASK_ENABLE NU_FALSE
#endif

#ifndef CFG_NU_OS_KERN_DEVMGR_MAX_DEVICE_LISTENERS
#define CFG_NU_OS_KERN_DEVMGR_MAX_DEVICE_LISTENERS      1
#endif

#ifndef CFG_NU_OS_KERN_DEVMGR_MAX_DEV_SESSION_CNT
#define CFG_NU_OS_KERN_DEVMGR_MAX_DEV_SESSION_CNT       1
#endif

/***** BEGIN User Configurable Parameters Section *****/

/* Maximum number of labels a device can have */
#define DV_MAX_DEV_LABEL_CNT    CFG_NU_OS_KERN_DEVMGR_MAX_DEV_LABEL_CNT

/* Maximum number of device ids that can be returned by DVI_Device_Discovery_Task */
#define DV_DISCOVERY_TASK_MAX_ID_CNT CFG_NU_OS_KERN_DEVMGR_DISCOVERY_TASK_MAX_ID_CNT

/* Device manager discovery task enable. */
#define DV_DEV_DISCOVERY_TASK_ENB   CFG_NU_OS_KERN_DEVMGR_DISCOVERY_TASK_ENABLE

/* Limit for maximum device listeners. */
#define DV_MAX_DEVICE_LISTENERS     CFG_NU_OS_KERN_DEVMGR_MAX_DEVICE_LISTENERS

/***** END User Configurable Parameters Section *****/

/* Define common IOCTL constants*/
#define DV_IOCTL0                   0

#define DV_IOCTL_INVALID_LENGTH     -1
#define DV_IOCTL_INVALID_MODE       -2
#define DV_IOCTL_INVALID_CMD        -3

/* Define illegal dev id, session id and dev handle flag values */
#define DV_INVALID_DEV    ((DV_DEV_ID)-1)
#define DV_INVALID_HANDLE ((DV_DEV_HANDLE)-1)
#define DV_INVALID_SES    ((INT32)-1)

/* Define system errors */
#define         DV_INVALID_INPUT_PARAMS          -1
#define         DV_DEV_LIST_TOO_SMALL            -2
#define         DV_LABEL_LIST_TOO_SMALL          -3
#define         DV_REG_LIST_TOO_SMALL            -4
#define         DV_UNREG_LIST_TOO_SMALL          -5
#define         DV_INVALID_REG_PARAMS            -6
#define         DV_INVALID_UNREG_PARAMS          -7
#define         DV_NO_AVAILABLE_DEV_ID           -8
#define         DV_NO_AVAILABLE_SESSION          -9
#define         DV_DEV_NOT_REGISTERED            -10
#define         DV_SESSION_NOT_OPEN              -11
#define         DV_LABEL_NOT_FOUND               -12
#define         DV_UNEXPECTED_ERROR              -13
#define         DV_MAX_LISTENER_LIMIT_EXCEEDED   -14

/* Define a macro to compare two labels */
#define DV_COMPARE_LABELS(label1,label2) (memcmp((VOID*)label1, (VOID*)label2, 16) == 0)

/* Define a macro to calculate the size of the label array */
#define DV_GET_LABEL_COUNT(label_array)  ((INT)(sizeof(label_array)/sizeof(DV_DEV_LABEL)))

/* Define a macro to clear a label */
#define DV_CLEAR_LABEL(label)            ((VOID)memset(label, 0, 16))


/* Device Manager Device ID definition */
typedef INT32 DV_DEV_ID;

/* Device Manager Device Handle definition */
typedef INT32 DV_DEV_HANDLE;

/* Device Label definition */
typedef struct _dv_dev_label_struct
{
    UINT8 data[16];

} DV_DEV_LABEL;

/* Typedefs for driver open, close, read, write and ioctl functions. */
typedef STATUS  (*DV_DRV_OPEN_FUNCTION)(VOID* instance_handle, DV_DEV_LABEL label_list[], INT label_cnt, VOID* *session_handle_ptr);
typedef STATUS  (*DV_DRV_CLOSE_FUNCTION)(VOID* session_handle);
typedef STATUS  (*DV_DRV_READ_FUNCTION)(VOID* session_handle, VOID *buffer, UINT32 numbyte, OFFSET_T byte_offset, UINT32 *bytes_read_ptr);
typedef STATUS  (*DV_DRV_WRITE_FUNCTION)(VOID* session_handle, const VOID *buffer, UINT32 numbyte, OFFSET_T byte_offset, UINT32 *bytes_written_ptr);
typedef STATUS  (*DV_DRV_IOCTL_FUNCTION)(VOID* session_handle, INT ioctl_num, VOID* ioctl_data, INT ioctl_data_len);

/* Typedefs for device register/unregister listeners. */
typedef STATUS  (*DEV_REGISTER_CALLBACK)(DV_DEV_ID, VOID *);
typedef STATUS  (*DEV_UNREGISTER_CALLBACK)(DV_DEV_ID, VOID *);
typedef UINT32  DV_LISTENER_HANDLE;

/* Application level structure for DVC_Dev_Register() */
typedef struct _dv_drv_functions_struct
{
    DV_DRV_OPEN_FUNCTION   drv_open_ptr;
    DV_DRV_CLOSE_FUNCTION  drv_close_ptr;
    DV_DRV_READ_FUNCTION   drv_read_ptr;
    DV_DRV_WRITE_FUNCTION  drv_write_ptr;
    DV_DRV_IOCTL_FUNCTION  drv_ioctl_ptr;

} DV_DRV_FUNCTIONS;

/* Application level structure for DV_IOCTL0 */
typedef struct _dv_ioctl0_struct
{
    DV_DEV_LABEL  label;
    INT           base;

} DV_IOCTL0_STRUCT;

/* Application level structure for DVC_Reg_Change_Check() */
typedef struct _dv_app_registry_change_struct
{
    INT             max_id_cnt;
    INT             registry_changes;
    DV_DEV_LABEL   *dev_label_list_ptr;
    INT             dev_label_cnt;
    DV_DEV_ID      *known_id_list_ptr;
    INT            *known_id_cnt_ptr;
    DV_DEV_ID      *reg_id_list_ptr;
    INT            *reg_id_cnt_ptr;
    DV_DEV_ID      *unreg_id_list_ptr;
    INT            *unreg_id_cnt_ptr;

} DV_APP_REGISTRY_CHANGE;

/* Device listener definition */
typedef struct _dv_dev_listener_struct
{
    DV_DEV_LABEL            dev_label_list[DV_MAX_DEV_LABEL_CNT];
    DV_DEV_ID               known_id_list[DV_DISCOVERY_TASK_MAX_ID_CNT];
    INT                     known_id_cnt;
    INT                     dev_label_cnt;
    DEV_REGISTER_CALLBACK   device_register_callback;
    DEV_UNREGISTER_CALLBACK device_unregister_callback;
    VOID                   *context;

} DV_DEV_LISTENER;


/* Core processing functions.  */

STATUS  DVC_Dev_Register (VOID* instance_handle,
                          DV_DEV_LABEL dev_label_list[], INT dev_label_cnt,
                          DV_DRV_FUNCTIONS *drv_functions_ptr,
                          DV_DEV_ID *dev_id_ptr);

STATUS  DVC_Dev_Unregister (DV_DEV_ID dev_id,
                            VOID* *instance_handle_ptr);

STATUS  DVC_Dev_ID_Open (DV_DEV_ID dev_id,
                         DV_DEV_LABEL dev_label_list[], INT dev_label_cnt,
                         DV_DEV_HANDLE *dev_handle_ptr);

STATUS  DVC_Dev_Open (DV_DEV_LABEL *dev_name_ptr,
                      DV_DEV_HANDLE *dev_handle_ptr);

STATUS  DVC_Dev_Close (DV_DEV_HANDLE dev_handle);

STATUS  DVC_Dev_Read (DV_DEV_HANDLE dev_handle,
                      VOID *buffer_ptr,
                      UINT32 numbyte,
                      OFFSET_T byte_offset,
                      UINT32 *bytes_read_ptr);

STATUS  DVC_Dev_Write (DV_DEV_HANDLE dev_handle,
                       VOID *buffer_ptr,
                       UINT32 numbyte,
                       OFFSET_T byte_offset,
                       UINT32 *bytes_written_ptr);

STATUS  DVC_Dev_Ioctl (DV_DEV_HANDLE dev_handle,
                       INT ioctl_num,
                       VOID* ioctl_data,
                       INT ioctl_data_len);

STATUS DVC_Reg_Change_Notify(DV_DEV_LABEL             dev_label_list[],
                             INT                      dev_label_cnt,
                             DEV_REGISTER_CALLBACK    register_cb,
                             DEV_UNREGISTER_CALLBACK  unregister_cb,
                             VOID                    *context,
                             DV_LISTENER_HANDLE      *listener_id);

STATUS  DVC_Reg_Change_Check (DV_APP_REGISTRY_CHANGE *app_struct_ptr,
                              UNSIGNED suspend);

STATUS  DVC_Dev_ID_Get (DV_DEV_LABEL dev_label_list[], INT dev_label_cnt,
                        DV_DEV_ID dev_id_list[], INT *dev_id_cnt_ptr);

STATUS  DVC_Dev_Labels_Get (DV_DEV_ID dev_id,
                            DV_DEV_LABEL dev_label_list[], INT *dev_label_cnt_ptr);

/* Supplemental processing functions.  */

STATUS  DVS_Label_Append (DV_DEV_LABEL new_label_list[], INT new_label_max,
                          DV_DEV_LABEL old_label_list[], INT old_label_cnt,
                          DV_DEV_LABEL app_label_list[], INT app_label_cnt);

STATUS  DVS_Label_Append_Instance (DV_DEV_LABEL new_label_list[], INT new_label_max,
                                   DV_DEV_LABEL old_label_list[], INT old_label_cnt,
                                   const CHAR* key);

STATUS  DVS_Label_Copy (DV_DEV_LABEL to_label_list[], INT to_label_max,
                        DV_DEV_LABEL from_label_list[], INT from_label_cnt);

STATUS  DVS_Label_List_Contains (DV_DEV_LABEL label_list[], INT label_cnt,
                                 DV_DEV_LABEL search_label);

STATUS  DVS_Label_Remove (DV_DEV_LABEL label_list[], INT label_cnt,
                          DV_DEV_LABEL remove_label);

STATUS  DVS_Label_Replace (DV_DEV_LABEL label_list[], INT label_cnt,
                           DV_DEV_LABEL search_label, DV_DEV_LABEL new_label);


#ifdef          __cplusplus
}
#endif /* _cplusplus */

#endif
