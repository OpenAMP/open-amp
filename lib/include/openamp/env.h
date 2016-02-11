/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 * Copyright (c) 2015 Xilinx, Inc. All rights reserved.
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
 *       env.h
 *
 * COMPONENT
 *
 *         OpenAMP stack.
 *
 * DESCRIPTION
 *
 *       This file defines abstraction layer for OpenAMP stack. The implementor
 *       must provide definition of all the functions.
 *
 * DATA STRUCTURES
 *
 *        none
 *
 * FUNCTIONS
 *
 *       env_allocate_memory
 *       env_free_memory
 *       env_memset
 *       env_memcpy
 *       env_strlen
 *       env_strcpy
 *       env_strncpy
 *       env_print
 *       env_map_vatopa
 *       env_map_patova
 *       env_mb
 *       env_rmb
 *       env_wmb
 *       env_create_mutex
 *       env_delete_mutex
 *       env_lock_mutex
 *       env_unlock_mutex
 *       env_sleep_msec
 *       env_disable_interrupts
 *       env_restore_interrupts
 *
 **************************************************************************/
#ifndef _ENV_H_
#define _ENV_H_

#include <stdio.h>

/**
 * env_init
 *
 * Initializes OS/BM environment.
 *
 * @returns - execution status
 */

int env_init();

/**
 * env_deinit
 *
 * Uninitializes OS/BM environment.
 *
 * @returns - execution status
 */

int env_deinit();
/**
 * -------------------------------------------------------------------------
 *
 * Dynamic memory management functions. The parameters
 * are similar to standard c functions.
 *
 *-------------------------------------------------------------------------
 **/

/**
 * env_allocate_memory
 *
 * Allocates memory with the given size.
 *
 * @param size - size of memory to allocate
 *
 * @return - pointer to allocated memory
 */
void *env_allocate_memory(unsigned int size);

/**
 * env_free_memory
 *
 * Frees memory pointed by the given parameter.
 *
 * @param ptr - pointer to memory to free
 */
void env_free_memory(void *ptr);

/**
 * -------------------------------------------------------------------------
 *
 * RTL Functions
 *
 *-------------------------------------------------------------------------
 */

void env_memset(void *, int, unsigned long);
void env_memcpy(void *, void const *, unsigned long);
size_t env_strlen(const char *);
void env_strcpy(char *, const char *);
int env_strcmp(const char *, const char *);
void env_strncpy(char *, const char *, unsigned long);
int env_strncmp(char *, const char *, unsigned long);
#define env_print(...)  printf(__VA_ARGS__)

/**
 *-----------------------------------------------------------------------------
 *
 *  Functions to convert physical address to virtual address and vice versa.
 *
 *-----------------------------------------------------------------------------
 */

/**
 * env_map_vatopa
 *
 * Converts logical address to physical address
 *
 * @param address - pointer to logical address
 *
 * @return  - physical address
 */
unsigned long env_map_vatopa(void *address);

/**
 * env_map_patova
 *
 * Converts physical address to logical address
 *
 * @param address - pointer to physical address
 *
 * @return  - logical address
 *
 */
void *env_map_patova(unsigned long address);

/**
 *-----------------------------------------------------------------------------
 *
 *  Abstractions for memory barrier instructions.
 *
 *-----------------------------------------------------------------------------
 */

/**
 * env_mb
 *
 * Inserts memory barrier.
 */

void env_mb();

/**
 * env_rmb
 *
 * Inserts read memory barrier
 */

void env_rmb();

/**
 * env_wmb
 *
 * Inserts write memory barrier
 */

void env_wmb();

/**
 *-----------------------------------------------------------------------------
 *
 *  Abstractions for OS lock primitives.
 *
 *-----------------------------------------------------------------------------
 */

/**
 * env_create_mutex
 *
 * Creates a mutex with given initial count.
 *
 * @param lock -  pointer to created mutex
 * @param count - initial count 0 or 1
 *
 * @return - status of function execution
 */
int env_create_mutex(void **lock, int count);

/**
 * env_delete_mutex
 *
 * Deletes the given lock.
 *
 * @param lock - mutex to delete
 */

void env_delete_mutex(void *lock);

/**
 * env_lock_mutex
 *
 * Tries to acquire the lock, if lock is not available then call to
 * this function will suspend.
 *
 * @param lock - mutex to lock
 *
 */

void env_lock_mutex(void *lock);

/**
 * env_unlock_mutex
 *
 * Releases the given lock.
 *
 * @param lock - mutex to unlock
 */

void env_unlock_mutex(void *lock);

/**
 * env_create_sync_lock
 *
 * Creates a synchronization lock primitive. It is used
 * when signal has to be sent from the interrupt context to main
 * thread context.
 *
 * @param lock  - pointer to created sync lock object
 * @param state - initial state , lock or unlocked
 *
 * @returns - status of function execution
 */
#define LOCKED			0
#define UNLOCKED		1

int env_create_sync_lock(void **lock, int state);

/**
 * env_create_sync_lock
 *
 * Deletes given sync lock object.
 *
 * @param lock  - sync lock to delete.
 *
 */

void env_delete_sync_lock(void *lock);

/**
 * env_acquire_sync_lock
 *
 * Tries to acquire the sync lock.
 *
 * @param lock  - sync lock to acquire.
 */
void env_acquire_sync_lock(void *lock);

/**
 * env_release_sync_lock
 *
 * Releases synchronization lock.
 *
 * @param lock  - sync lock to release.
 */
void env_release_sync_lock(void *lock);

/**
 * env_sleep_msec
 *
 * Suspends the calling thread for given time in msecs.
 *
 * @param num_msec -  delay in msecs
 */
void env_sleep_msec(int num_msec);

/**
 * env_disable_interrupts
 *
 * Disables system interrupts
 *
 */
void env_disable_interrupts();

/**
 * env_restore_interrupts
 *
 * Enables system interrupts
 *
 */
void env_restore_interrupts();

/**
 * env_register_isr
 *
 * Registers interrupt handler for the given interrupt vector.
 *
 * @param vector - interrupt vector number
 * @param data   - private data
 * @param isr    - interrupt handler
 */

void env_register_isr(int vector, void *data,
		      void (*isr) (int vector, void *data));
/**
 * env_register_isr_shared
 *
 * Registers interrupt handler for the given shared interrupt vector.
 *
 * @param vector - interrupt vector number
 * @param data   - private data
 * @param isr    - interrupt handler
 * @oaram name   - interrup handler name
 * @param shared - if the interrupt is a shared interrupt
 */

void env_register_isr_shared(int vector, void *data,
		      void (*isr) (int vector, void *data),
		      char *name,
		      int shared);

void env_update_isr(int vector, void *data,
		    void (*isr) (int vector, void *data),
		    char *name,
		    int shared);

/**
 * env_enable_interrupt
 *
 * Enables the given interrupt.
 *
 * @param vector   - interrupt vector number
 * @param priority - interrupt priority
 * @param polarity - interrupt polarity
 */

void env_enable_interrupt(unsigned int vector, unsigned int priority,
			  unsigned int polarity);

/**
 * env_disable_interrupt
 *
 * Disables the given interrupt.
 *
 * @param vector   - interrupt vector number
 */

void env_disable_interrupt(unsigned int vector);

/**
 * env_map_memory
 *
 * Enables memory mapping for given memory region.
 *
 * @param pa   - physical address of memory
 * @param va   - logical address of memory
 * @param size - memory size
 * param flags - flags for cache/uncached  and access type
 *
 * Currently only first byte of flag parameter is used and bits mapping is defined as follow;
 *
 * Cache bits
 * 0x0000_0001 = No cache
 * 0x0000_0010 = Write back
 * 0x0000_0100 = Write through
 * 0x0000_x000 = Not used
 *
 * Memory types
 *
 * 0x0001_xxxx = Memory Mapped
 * 0x0010_xxxx = IO Mapped
 * 0x0100_xxxx = Shared
 * 0x1000_xxxx = TLB
 */

/* Macros for caching scheme used by the shared memory */
#define UNCACHED                            (1 << 0)
#define WB_CACHE                            (1 << 1)
#define WT_CACHE                            (1 << 2)

/* Memory Types */
#define MEM_MAPPED                          (1 << 4)
#define IO_MAPPED                           (1 << 5)
#define SHARED_MEM                          (1 << 6)
#define TLB_MEM                             (1 << 7)

void env_map_memory(unsigned int pa, unsigned int va, unsigned int size,
		    unsigned int flags);

/**
 * env_get_timestamp
 *
 * Returns a 64 bit time stamp.
 *
 *
 */
unsigned long long env_get_timestamp(void);

/**
 * env_disable_cache
 * 
 * Disables system caches.
 *
 */

void env_disable_cache();

typedef void LOCK;

#endif				/* _ENV_H_ */
