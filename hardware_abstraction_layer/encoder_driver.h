/*
 * encoder_driver.h
 *
 *  Created on: May 8, 2023
 *      Author: silas
 */
#ifndef ENCODER_DRIVER_H_
#define ENCODER_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>
#include "ringbuffer.h"
#include "clock_driver.h"

#define DATA_BUFFER_SIZE_MIN 32

typedef struct {
    uint16_t signal_a_gpio_port;
    uint16_t signal_a_gpio_pin;
    uint16_t signal_b_gpio_port;
    uint16_t signal_b_gpio_pin;
    ring_buffer_t* data_buffer;
    bool has_processed_a_reading;
    int16_t pulse_count;
    float instantaneous_velocity_pulse_per_ms;
    clock_driver_time_t delta_t;
} encoder_driver_t;

// Initialize driver and underlying peripherals. Returns true on success, false on failure.
// EXPECTATIONS:
//  - passed driver is populated:
//      - GPIO pins and ports are set
//      - data buffer is initialized
//  - RTC is enabled
bool encoder_driver_open(encoder_driver_t* driver);

// Deinitialize driver and underlying peripherals.
void encoder_driver_close(encoder_driver_t* driver);

void encoder_driver_calculate_kinematics(encoder_driver_t* driver);

inline float encoder_driver_get_position_degrees(encoder_driver_t* driver);

// IMPORTANT: The user (YOU) must register this function within the correct ISR.
//            Call this function in signal_a's GPIO interrupt.
inline void encoder_driver_signal_a_rising_edge_event(encoder_driver_t* driver);

#endif /* HARDWARE_ABSTRACTION_LAYER_ENCODER_DRIVER_H_ */
