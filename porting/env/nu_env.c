/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of Mentor Graphics Corporation nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

 /**************************************************************************
 * FILE NAME
 *
 *       nu_env.c
 *
 * COMPONENT
 *
 *         OpenAMP stack.
 *
 * DESCRIPTION
 *
 *       This file is Nucleus Implementation of environment layer.
 *
 *
 **************************************************************************/

#include "env.h"
#include "../config/config.h"

#if (ENV == NU_ENV)

#include <stdlib.h>
#include <string.h>
#include "nucleus.h"
#include "kernel/nu_kernel.h"
#include "os/kernel/plus/core/inc/thread_control.h"

/* Configurable parameters */

/* Main task's stack size */
#define NU_ENV_STACK_SIZE      (NU_MIN_STACK_SIZE * 64)

/* Main task's priority */
#define NU_ENV_TASK_PRIOIRTY   26

/* Main task's time slice */
#define NU_ENV_TASK_SLICE      20


/* Queue message size */
#define MSG_SIZE               sizeof(struct isr_info)/sizeof(unsigned)

/* Internal functions */
static VOID NU_Env_Task_Entry(UNSIGNED argc, VOID *argv);
static void NU_Env_HISR_Entry();
static VOID NU_Env_LISR(int vector);

/* Globals */
static NU_HISR NU_Env_HISR;
static NU_TASK NU_Env_Task;
static NU_QUEUE NU_Env_Queue;
static NU_SEMAPHORE NU_Env_Sem;
static VOID *Queue_Mem = NU_NULL;
static VOID *Task_Mem = NU_NULL;
static VOID *Hisr_Mem = NU_NULL;
static INT Intr_Count = 0;
static INT ENV_INIT_COUNT = 0;
static struct isr_info isr_table[ISR_COUNT];
static INT Old_Level = 0;

/**
 * env_init
 *
 * Initializes OS/BM environment.
 *
 */
int env_init(){
    STATUS status = NU_SUCCESS;
    NU_MEMORY_POOL *pool_ptr = NU_NULL;

    if(!ENV_INIT_COUNT)
    {
        status = NU_System_Memory_Get(&pool_ptr, NU_NULL);
        if(status == NU_SUCCESS)
        {
            status = NU_Allocate_Memory(pool_ptr, &Hisr_Mem, NU_ENV_STACK_SIZE,
                                        NU_NO_SUSPEND);
            if (status == NU_SUCCESS)
            {
                memset(Hisr_Mem , 0x00 , NU_ENV_STACK_SIZE);
                status = NU_Create_HISR(&NU_Env_HISR, "NU_Env_Q", NU_Env_HISR_Entry, 2, Hisr_Mem,
                                        NU_ENV_STACK_SIZE);
                if (status == NU_SUCCESS)
                {
                    status = NU_Create_HISR_Queue(&NU_Env_HISR,8,NU_TRUE);
                    if(status == NU_SUCCESS)
                    {
                        status = NU_Allocate_Memory(pool_ptr, &Queue_Mem,
                                8*sizeof(struct isr_info), NU_NO_SUSPEND);
                        memset(Queue_Mem , 0x00 , 8*sizeof(struct isr_info));
                        if(status == NU_SUCCESS)
                        {
                            status = NU_Create_Queue(&NU_Env_Queue, "NU_Env_Q", Queue_Mem,
                                            (8*MSG_SIZE), NU_FIXED_SIZE,
                                            MSG_SIZE, NU_FIFO);
                            if (status == NU_SUCCESS)
                            {
                                status = NU_Create_Semaphore(&NU_Env_Sem, "ENV_MSEM", 1 , NU_FIFO);
                                if(status == NU_SUCCESS)
                                {
                                    status = NU_Allocate_Memory(pool_ptr, &Task_Mem,
                                                                NU_ENV_STACK_SIZE, NU_NO_SUSPEND);
                                    memset(Task_Mem , 0x00 , NU_ENV_STACK_SIZE);
                                    if (status == NU_SUCCESS)
                                    {
                                        /* Create task 0.  */
                                        status = NU_Create_Task(&NU_Env_Task, "NU_Env_T", NU_Env_Task_Entry, 0,
                                                        pool_ptr, Task_Mem, NU_ENV_STACK_SIZE, NU_ENV_TASK_PRIOIRTY,
                                                        NU_ENV_TASK_SLICE, NU_PREEMPT, NU_START);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if(status == NU_SUCCESS){
        NU_Obtain_Semaphore(&NU_Env_Sem, NU_SUSPEND);
        ENV_INIT_COUNT++;
        NU_Release_Semaphore(&NU_Env_Sem);
    }

    return status;
}

/**
 * env_deinit
 *
 * Uninitializes OS/BM environment.
 *
 * @returns - execution status
 */
int env_deinit() {
    int status = NU_SUCCESS;

    NU_Obtain_Semaphore(&NU_Env_Sem, NU_SUSPEND);
    ENV_INIT_COUNT--;
    NU_Release_Semaphore(&NU_Env_Sem);

    if (!ENV_INIT_COUNT) {
        if (Task_Mem) {
            status |= NU_Terminate_Task(&NU_Env_Task);
            status |= NU_Delete_Task(&NU_Env_Task);
            status |= NU_Deallocate_Memory(Task_Mem);
        }

        if (Queue_Mem) {
            NU_Delete_Queue(&NU_Env_Queue);
            status |= NU_Deallocate_Memory(Queue_Mem);
        }

        if (Hisr_Mem) {
            status |= NU_Delete_HISR(&NU_Env_HISR);
            status |= NU_Deallocate_Memory(Hisr_Mem);
        }

        status |= NU_Delete_Semaphore(&NU_Env_Sem);
        Intr_Count = 0;

    }
    return status;
}

/**
 * env_allocate_memory - implementation
 *
 * @param size
 */
void *env_allocate_memory(unsigned int size)
{
    STATUS status = NU_SUCCESS;
    VOID *mem_ptr = NU_NULL;
    NU_MEMORY_POOL *pool_ptr = NU_NULL;

    /* Ensure valid allocation size. */
    if (size != 0)
    {
        /* Get system cached memory pools pointer */
        status = NU_System_Memory_Get(&pool_ptr, NU_NULL);
        if (status == NU_SUCCESS)
        {
            /* Allocate requested memory from Nucleus memory system. */
            status = NU_Allocate_Memory(pool_ptr, &mem_ptr,
                                         (UNSIGNED)size, NU_NO_SUSPEND);
            if(status == NU_SUCCESS)
            {
                memset(mem_ptr, 0x00 ,size);
            }
        }
    }

    return (mem_ptr);
}

/**
 * env_free_memory - implementation
 *
 * @param ptr
 */
void env_free_memory(void *ptr)
{
    if (ptr != NU_NULL)
    {
        (VOID) NU_Deallocate_Memory(ptr);
    }
}


/**
 *
 * env_memset - implementation
 *
 * @param ptr
 * @param value
 * @param size
 */
void env_memset(void *ptr, int value, unsigned long size)
{
    memset(ptr, value, size);
}

void env_memcpy(void *dst, void const * src, unsigned long len){
    memcpy(dst,src,len);
}
/**
 *
 * env_strncpy - implementation
 *
 * @param dest
 * @param src
 * @param len
 */
void env_strncpy(char * dst, const char *src, unsigned long len)
{
    strncpy(dst, src, len);
}

int env_strncmp(char * dst, const char *src, unsigned long len)
{
    return (strncmp(dst, src, len));
}

/**
 *
 * env_strcmp - implementation
 *
 * @param dst
 * @param src
 */

int env_strcmp(const char * dst, const char *src) {
    return (strcmp(dst, src));
}
/**
 *
 * env_mb - implementation
 *
 */
void env_mb()
{
    ESAL_TS_RTE_COMPILE_MEM_BARRIER();
}

/**
 * osalr_mb - implementation
 */
void env_rmb()
{
    ESAL_TS_RTE_COMPILE_MEM_BARRIER();
}

/**
 * env_wmb - implementation
 */
void env_wmb()
{
    ESAL_TS_RTE_COMPILE_MEM_BARRIER();
}

/**
 * env_map_vatopa - implementation
 *
 * @param address
 */
unsigned long env_map_vatopa(void *address)
{
    return ((((unsigned long)address & (~( 0x0fff << 20))) | (0x08 << 24)));
}

/**
 * env_map_patova - implementation
 *
 * @param address
 */
void *env_map_patova(unsigned long address)
{
    return ((void *)address);
}

/**
 * env_create_mutex
 *
 * Creates a mutex with initial state being unlocked.
 *
 */
int env_create_mutex(void **lock, int count)
{

    NU_SEMAPHORE *sem = NU_NULL;
    NU_MEMORY_POOL *pool_ptr = NU_NULL;
    STATUS status;

    /* Get system cached memory pools pointer */
    status = NU_System_Memory_Get(&pool_ptr, NU_NULL);
    if (status == NU_SUCCESS)
    {
        /* Allocate requested memory from Nucleus semaphore object. */
        status = NU_Allocate_Memory(pool_ptr, (VOID**)&sem,
                                     sizeof(NU_SEMAPHORE), NU_NO_SUSPEND);
        if (status == NU_SUCCESS){
            memset(sem , 0x00 , sizeof(NU_SEMAPHORE));
            status = NU_Create_Semaphore(sem, "Env_Sem", count, NU_PRIORITY);
            if ( status == NU_SUCCESS){
                *lock = sem;
            }
        }
    }

    return status;
}

/**
 * env_delete_mutex
 *
 * Deletes the given lock
 *
 */
void env_delete_mutex(void *lock)
{
    if (lock)
    {
        NU_Delete_Semaphore((NU_SEMAPHORE *)lock);
        NU_Deallocate_Memory(lock);
    }
}

/**
 * env_lock_mutex
 *
 * Tries to acquire the lock, if lock is not available then call to
 * this function will suspend.
 */
void env_lock_mutex(void *lock)
{
    NU_Obtain_Semaphore((NU_SEMAPHORE *)lock, NU_SUSPEND);
}

/**
 * env_unlock_mutex
 *
 * Releases the given lock.
 */

void env_unlock_mutex(void *lock)
{
    NU_Release_Semaphore((NU_SEMAPHORE*)lock);
}

/**
 * env_create_sync_lock
 *
 * Creates a synchronization lock primitive. It is required
 * when signal is has to be sent from the interrupt context
 * to main thread context.
 *
 */
int env_create_sync_lock(void **lock , int state){
    NU_SEMAPHORE *sem = NU_NULL;
    NU_MEMORY_POOL *pool_ptr = NU_NULL;
    STATUS status;

    /* Get system cached memory pools pointer */
    status = NU_System_Memory_Get(&pool_ptr, NU_NULL);
    if (status == NU_SUCCESS)
    {
        /* Allocate requested memory from Nucleus semaphore object. */
        status = NU_Allocate_Memory(pool_ptr, (VOID**)&sem,
                                     sizeof(NU_SEMAPHORE), NU_NO_SUSPEND);
        if (status == NU_SUCCESS){
            memset(sem , 0x00 , sizeof(NU_SEMAPHORE));
            status = NU_Create_Semaphore(sem, "Env_Sem", state, NU_PRIORITY);
            if ( status == NU_SUCCESS){
                *lock = sem;
            }
        }
    }

    return status;
}

/**
 * env_delete_sync_lock
 *
 * Deletes the given lock
 *
 */
void env_delete_sync_lock(void *lock)
{
    if (lock)
    {
        NU_Delete_Semaphore((NU_SEMAPHORE *)lock);
        NU_Deallocate_Memory(lock);
    }
}

/**
 * env_acquire_sync_lock
 *
 * Tries to acquire the lock, if lock is not available then call to
 * this function will suspend.
 */
void env_acquire_sync_lock(void *lock)
{
    NU_Obtain_Semaphore((NU_SEMAPHORE *)lock, NU_SUSPEND);
}

/**
 * env_release_sync_lock
 *
 * Releases the given lock.
 */
void env_release_sync_lock(void *lock)
{
    NU_Release_Semaphore((NU_SEMAPHORE*)lock);
}
/**
 * env_sleep_msec
 *
 * Suspends the calling thread for given time , in msecs.
 */

void env_sleep_msec(int num_msec)
{
    NU_Sleep(num_msec/10);
}

/**
 * env_disable_interrupts
 *
 * Disables system interrupts
 *
 */
void env_disable_interrupts()
{
    Old_Level = NU_Local_Control_Interrupts(NU_DISABLE_INTERRUPTS);
}

/**
 * env_restore_interrupts
 *
 * Enables system interrupts
 *
 */
void env_restore_interrupts()
{
    NU_Local_Control_Interrupts(Old_Level);
}

/**
 * env_register_isr
 *
 * Registers interrupt handler for the given interrupt vector.
 *
 */
void env_register_isr(int vector , void *data ,
                      void (*isr)(int vector , void *data))
{
    VOID (*old_lisr)(INT);

    env_disable_interrupts();

    if(Intr_Count < ISR_COUNT)
    {
        /* Save interrupt data */
        isr_table[Intr_Count].vector = vector + ESAL_AR_INT_VECTOR_ID_DELIMITER;
        isr_table[Intr_Count].data = data;
        isr_table[Intr_Count++].isr = isr;

        /* Register LISR */
        NU_Register_LISR(vector + ESAL_AR_INT_VECTOR_ID_DELIMITER, NU_Env_LISR, &old_lisr);
    }

    env_restore_interrupts();

}

/**
 * env_enable_interrupt
 *
 * Enables the given interrupt
 *
 * @param vector   - interrupt vector number
 * @param priority - interrupt priority
 * @param polarity - interrupt polarity
 */

void env_enable_interrupt(unsigned int vector , unsigned int priority ,
                unsigned int polarity)
{
    int idx;

    env_disable_interrupts();

    for(idx = 0 ; idx < ISR_COUNT ; idx++)
    {
        if(isr_table[idx].vector == vector + ESAL_AR_INT_VECTOR_ID_DELIMITER)
        {
            isr_table[idx].priority = priority;
            isr_table[idx].type = polarity;
            (VOID) ESAL_GE_INT_Enable(vector + ESAL_AR_INT_VECTOR_ID_DELIMITER, polarity, priority );
            break;
        }
    }

    env_restore_interrupts();
}

/**
 * env_disable_interrupt
 *
 * Disables the given interrupt
 *
 * @param vector   - interrupt vector number
 */

void env_disable_interrupt(unsigned int vector)
{
    ESAL_GE_INT_Disable(vector);
}

/**
 * env_map_memory
 *
 * Enables memory mapping for given memory region.
 *
 * @param pa   - physical address of memory
 * @param va   - logical address of memory
 * @param size - memory size
 * param flags - flags for cache/uncached  and access type
 */

void env_map_memory(unsigned int pa, unsigned int va, unsigned int size,
                unsigned int flags)
{
   int mem_type = ESAL_RAM;
   int cache_type = ESAL_NOCACHE;

   if ((flags & (0x0f << 4 )) == MEM_MAPPED)
   {
       mem_type = ESAL_MEM_MAPPED;
   }

    if ((flags & 0x0f) == WB_CACHE) {
        cache_type = ESAL_WRITEBACK;
    } else if ((flags & 0x0f) == WT_CACHE) {
        cache_type = ESAL_WRITETHROUGH;
    } else {
        cache_type = ESAL_NOCACHE;
    }

   ESAL_CO_MEM_Region_Setup(0, va, pa, size, mem_type, cache_type , 0);
}

/**
 * 
 * env_get_timestamp
 *
 * Returns a 64 bit time stamp.
 *
 *
 */
unsigned long long env_get_timestamp(void) {

    return NU_Get_Time_Stamp();
}
/**
 * env_disable_cache
 * 
 * Disables system caches.
 *
 */

void env_disable_cache() {
    ESAL_CO_MEM_DCACHE_ALL_FLUSH_INVAL();
    ESAL_CO_MEM_ICACHE_ALL_INVALIDATE();
    ESAL_CO_MEM_CACHE_DISABLE();
}

/***********************************************************************
 * *
 * *   FUNCTION
 * *
 * *       Env_Task_Entry
 * *
 * *   DESCRIPTION
 * *
 * *       Entry function for the Nucleus Environment task.
 *
 * ***********************************************************************/
static VOID NU_Env_Task_Entry(UNSIGNED argc, VOID *argv)
{
    STATUS status;
    UNSIGNED size;
    struct isr_info intr;

    while (1)
    {
        /* Wait for the interrupt */
        status =  NU_Receive_From_Queue(&NU_Env_Queue, &intr,
                        MSG_SIZE, &size,NU_SUSPEND);
        if(status == NU_SUCCESS)
        {
            intr.isr(intr.vector - ESAL_AR_INT_VECTOR_ID_DELIMITER, intr.data);
        }
    }
}

static void NU_Env_HISR_Entry()
{
    struct isr_info *info;
    NU_Receive_From_HISR_Queue(&NU_Env_HISR, (VOID **)&info);
    NU_Send_To_Queue(&NU_Env_Queue,info,MSG_SIZE,NU_NO_SUSPEND);
    env_enable_interrupt(info->vector , info->priority, info->type);
}

static VOID NU_Env_LISR(int vector)
{
    int idx;
    env_disable_interrupt(vector);

    for(idx = 0 ; idx < ISR_COUNT ; idx++)
    {
        if(isr_table[idx].vector == vector)
        {
            NU_Send_To_HISR_Queue(&NU_Env_HISR , &isr_table[idx], NU_ACTIVATE_HISR);
            break;
        }
    }
}

#endif /* (ENV == NU_ENV) */
