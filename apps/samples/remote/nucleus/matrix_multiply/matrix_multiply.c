/* This is a sample demonstration application that showcases usage of remoteproc
 and rpmsg APIs on the remote core. This application is meant to run on the remote CPU
 running Nucleus. This applicationr receives two matrices from the master, multiplies
 them and returns the result to the master core. */

/* Including required headers */
#include  <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "open_amp.h"
#include "rsc_table.h"
#include "nucleus.h"
#include "kernel/nu_kernel.h"

/* Define the main task's stack size */
#define STACK_SIZE              (NU_MIN_STACK_SIZE * 64)

/* Define the main task's priority */
#define TASK_PRIORITY           26

/* Define the main task's time slice */
#define TASK_SLICE              20

/* Application */
#define MAX_SIZE                6
#define NUM_MATRIX              2

#define SHUTDOWN_MSG            0xEF56A55A

typedef struct _matrix {
    unsigned long size;
    unsigned long elements[MAX_SIZE][MAX_SIZE];
} matrix;

/* Prototype for the main task's entry function */
static void Main_Task_Entry(UNSIGNED argc, VOID *argv);

static NU_TASK Task_Control_Block;
static NU_SEMAPHORE Remote_Sem;
static struct rpmsg_endpoint *rp_ept;
static struct rpmsg_channel *app_rp_chnl;
static matrix matrix_array[NUM_MATRIX];
static matrix matrix_result;
static struct remote_proc *proc;
static struct rsc_table_info rsc_info;
extern const struct remote_resource_table resources;
static int shutdown_flag = 0;

void rpmsg_channel_created(struct rpmsg_channel *rp_chnl);
void rpmsg_channel_deleted(struct rpmsg_channel *rp_chnl);
void rpmsg_read_default_cb(struct rpmsg_channel *rp_chnl, void *data, int len,
                void * pric, unsigned long src);

/***********************************************************************
 * *
 * *   FUNCTION
 * *
 * *       Application_Initialize
 * *
 * *   DESCRIPTION
 * *
 * *       Demo application entry point
 *
 * ***********************************************************************/
VOID Application_Initialize(NU_MEMORY_POOL* mem_pool,
                NU_MEMORY_POOL* uncached_mem_pool) {
    VOID *pointer;
    STATUS status;

    /* Reference unused parameters to avoid toolset warnings */
    NU_UNUSED_PARAM(uncached_mem_pool);

    status = NU_Create_Semaphore(&Remote_Sem, "r_sem", 0, NU_FIFO);

    if (status == NU_SUCCESS) {
        status = NU_Allocate_Memory(mem_pool, &pointer,
                        STACK_SIZE, NU_NO_SUSPEND);
        if (status == NU_SUCCESS) {
            /* Create task 0.  */
            status = NU_Create_Task(&Task_Control_Block, "MAIN",
                            Main_Task_Entry, 0, uncached_mem_pool, pointer,
                            STACK_SIZE, TASK_PRIORITY, TASK_SLICE,
                            NU_PREEMPT, NU_START);
        }
    }

    /* Check to see if previous operations were successful */
    if (status != NU_SUCCESS) {
        /* Loop forever */
        while (1);
    }
}

static void Matrix_Multiply(const matrix *m, const matrix *n, matrix *r) {
    int i, j, k;

    r->size = m->size;

    for (i = 0; i < m->size; ++i) {
        for (j = 0; j < n->size; ++j) {
            r->elements[i][j] = 0;
        }
    }

    for (i = 0; i < m->size; ++i) {
        for (j = 0; j < n->size; ++j) {
            for (k = 0; k < r->size; ++k) {
                r->elements[i][j] += m->elements[i][k] * n->elements[k][j];
            }
        }
    }
}

void rpmsg_channel_created(struct rpmsg_channel *rp_chnl) {

    app_rp_chnl = rp_chnl;
    rp_ept = rpmsg_create_ept(rp_chnl, rpmsg_read_default_cb, RPMSG_NULL,
                    RPMSG_ADDR_ANY);
}

void rpmsg_channel_deleted(struct rpmsg_channel *rp_chnl) {
    rpmsg_destroy_ept(rp_ept);
}

void rpmsg_read_default_cb(struct rpmsg_channel *rp_chnl, void *data, int len,
                void * priv, unsigned long src) {

    if ((*(int *) data) == SHUTDOWN_MSG) {
        shutdown_flag = 1;
        NU_Release_Semaphore(&Remote_Sem);
    } else {
        env_memcpy(matrix_array, data, len);
        NU_Release_Semaphore(&Remote_Sem);
    }
}

/***********************************************************************
 * *
 * *   FUNCTION
 * *
 * *       Main_Task_Entry
 * *
 * *   DESCRIPTION
 * *
 * *       Entry function for the main task. This task prints a hello world
 * *       message.
 * *
 * ***********************************************************************/
static VOID Main_Task_Entry(UNSIGNED argc, VOID *argv) {
    rsc_info.rsc_tab = (struct resource_table *) &resources;
    rsc_info.size = sizeof(resources);

    remoteproc_resource_init(&rsc_info, rpmsg_channel_created,
                    rpmsg_channel_deleted, rpmsg_read_default_cb, &proc);

    for (;;) {

        /* Wait for incoming matrix data. */
        NU_Obtain_Semaphore(&Remote_Sem, NU_SUSPEND);

        if (shutdown_flag) {
            remoteproc_resource_deinit(proc);
            break;
        }

        /* Process received data and multiple matrices. */
        Matrix_Multiply(&matrix_array[0], &matrix_array[1], &matrix_result);

        /* Send the result of matrix multiplication back to master. */
        rpmsg_send(app_rp_chnl, &matrix_result, sizeof(matrix));

        NU_Sleep(5);
    }
}
