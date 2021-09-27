// Copyright (c) Acconeer AB, 2019-2021
// All rights reserved

#ifndef ACC_INTEGRATION_H_
#define ACC_INTEGRATION_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


typedef uint32_t acc_integration_thread_id_t;

struct acc_integration_thread_handle;

typedef struct acc_integration_thread_handle *acc_integration_thread_handle_t;

struct acc_integration_mutex;

typedef struct acc_integration_mutex *acc_integration_mutex_t;

struct acc_integration_semaphore;

typedef struct acc_integration_semaphore *acc_integration_semaphore_t;

typedef void (*acc_integration_uart_read_func_t)(uint8_t data, uint32_t status);


/**
 * @brief Sleep for a specified number of microseconds
 *
 * @param time_usec Time in microseconds to sleep
 */
void acc_integration_sleep_us(uint32_t time_usec);


/**
 * @brief Sleep for a specified number of milliseconds
 *
 * @param time_msec Time in milliseconds to sleep
 */
void acc_integration_sleep_ms(uint32_t time_msec);


/**
 * @brief Allocate dynamic memory
 *
 * @param[in]  size The bytesize of the reuested memory block
 * @return Returns either NULL or a unique pointer to a memory block
 */
void *acc_integration_mem_alloc(size_t size);


/**
 * @brief Free dynamic memory
 *
 * @param[in]  ptr A pointer to the memory space to be freed
 */
void acc_integration_mem_free(void *ptr);


/**
 * @brief Get current time
 *
 * It is important that this value wraps correctly and uses all bits. I.e. it should count
 * upwards to 2^32 - 1 and then 0 again.
 *
 * @returns Current time as milliseconds
 */
uint32_t acc_integration_get_time(void);


#endif
