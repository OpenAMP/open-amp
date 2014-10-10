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
* FILE NAME
*
*       eqm.h
*
* COMPONENT
*
*       Event Queue Manager
*
* DESCRIPTION
*
*       This file contains Event Queue Manager information.
*
* DATA STRUCTURES
*
*       EQM_EVENT                       Structure to hold the event type.
*                                       If user wants to pass the event
*                                       data as well user will create a
*                                       new structure using this structure
*                                       as template and add data members
*                                       to his own structure. Obviously
*                                       the first parameter of his
*                                       structure would be event type.
*       EQM_EVENT_NODE                  Structure to store event along
*                                       with data
*       EQM_EVENT_QUEUE                 Structure to hold the event queue
*                                       attributes.
*
*************************************************************************/
#ifndef EQM_H
#define EQM_H


#ifdef __cplusplus
extern  "C" {                               /* C declarations in C++   */
#endif /* _cplusplus */

/* Datatypes */
typedef     UINT32                  EQM_EVENT_ID;
typedef     UINT32                  EQM_EVENT_HANDLE;

typedef struct  EQM_EVENT_STRUCT
{
    /* Event type. */
    UINT32  eqm_event_type;

} EQM_EVENT;

typedef struct  EQM_EVENT_NODE_STRUCT
{
    /* Unique ID assigned to a posted event. */
    EQM_EVENT_ID  eqm_event_id;

    /* Size of the associated event data. */
    UINT16        eqm_event_data_size;

    /* Associated event data. */
    UINT8         *eqm_event_data;

} EQM_EVENT_NODE;

typedef struct  EQM_EVENT_QUEUE_STRUCT
{
    /* Maximum number of events stored in event queue */
    UINT32              eqm_max_events;

    /* Maximum size of event data */
    UINT16              eqm_max_event_data_size;

    /* The ID that will be assigned to the incoming event in the system.
       '0' will not be assigned to any event. */
    EQM_EVENT_ID        eqm_current_event_id;

    /* eqm_buffer_index is the array index that is used as an handle to avoid
       searching overhead while reading the data associated with the event. */
    EQM_EVENT_HANDLE    eqm_buffer_index;

    /* eqm_event_data_buffer is an array of EQM_Event_NODE. Each element of
       the array will hold a new event. */
    EQM_EVENT_NODE      *eqm_event_data_buffer;

    /* EQM_Event_Group is used to signal the event to the waiting
       components. */
    NU_EVENT_GROUP      eqm_event_group;

} EQM_EVENT_QUEUE;


STATUS  NU_EQM_Create       (EQM_EVENT_QUEUE *event_queue_ptr,
                                UINT32 queue_size,
                                UINT16 max_event_data_size,
                                NU_MEMORY_POOL *memory_pool_ptr);
STATUS  NU_EQM_Delete       (EQM_EVENT_QUEUE *event_queue_ptr);
STATUS  NU_EQM_Post_Event   (EQM_EVENT_QUEUE *event_queue_ptr,
                                EQM_EVENT *event_ptr, UINT16 event_data_size,
                                EQM_EVENT_ID *posted_event_id_ptr);
STATUS  NU_EQM_Wait_Event   (EQM_EVENT_QUEUE *event_queue_ptr,
                                UINT32 requested_events_mask,
                                UINT32 *recvd_event_type_ptr,
                                EQM_EVENT_ID *recvd_event_id_ptr,
                                EQM_EVENT_HANDLE *recvd_event_handle_ptr);
STATUS  NU_EQM_Get_Event_Data(EQM_EVENT_QUEUE *event_queue_ptr,
                                EQM_EVENT_ID event_id,
                                EQM_EVENT_HANDLE event_handle,
                                EQM_EVENT *event_ptr);


#ifdef __cplusplus
}
#endif /* _cplusplus */

#endif /* EQM_H */
