/*
 * Copyright (c) 2017-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#include <stdint.h>

/* Include NS RTOS specific mutex declarations */
#include "cmsis_os2.h"
#include "tfm_ns_interface.h"

/* Static ns lock mutex id */
static osMutexId_t ns_lock_mutex_id = NULL;

/* Initialize the ns lock */
int32_t ns_interface_lock_init(void)
{
    /* NS RTOS specific mutex creation/initialization */
    ns_lock_mutex_id = osMutexNew(NULL);
    if (ns_lock_mutex_id != NULL) {
        return 0;
    }

    return -1;
}

int32_t tfm_ns_interface_dispatch(veneer_fn fn,
                                  uint32_t arg0, uint32_t arg1,
                                  uint32_t arg2, uint32_t arg3)
{
    int32_t result;

    /* TF-M request protected by NS lock. */
    osMutexAcquire(ns_lock_mutex_id, osWaitForever);

    result = fn(arg0, arg1, arg2, arg3);

    osMutexRelease(ns_lock_mutex_id);

    return result;
}
