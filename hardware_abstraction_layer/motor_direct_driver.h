/*
 * motor_direct_driver.h
 *
 *  Created on: May 16, 2023
 *      Author: silas
 */

#ifndef MOTOR_DIRECT_DRIVER_H_
#define MOTOR_DIRECT_DRIVER_H_

#include <stdint.h>

#include "pwm_driver.h"

// To "emulate" a DAC, time-domain is partitioned.
// Larger partitions enable finer control, but increase latency and lower
// the frequency. High frequency is desirable because a moving motor's
// kinetic energy will smooth the signal, but lower frequencies may produce
// notable motor oscillations.
#define PWM_COMPARATOR_MAX (UINT16_MAX / 64);    // "emulated dac" resolution

typedef struct {
    pwm_driver_t motor_magnitude_pwm_driver;
    uint16_t motor_direction_enable_1_gpio_port;
    uint16_t motor_direction_enable_1_gpio_pin;
    uint16_t motor_direction_enable_2_gpio_port;
    uint16_t motor_direction_enable_2_gpio_pin;
} motor_direct_driver_t;

// Initialize driver and underlying peri//    }pherals.
// EXPECTATIONS:
//      - driver's relevant members are set
//      - pwm_driver is configured and opened
void motor_direct_driver_open(motor_direct_driver_t* driver);

// -PWM_COMPARATOR_MAX <= power <= PWM_COMPARATOR_MAX
inline void motor_direct_driver_set_output(motor_direct_driver_t* driver, int16_t power_within_plus_minus_1024);

#endif /* MOTOR_DIRECT_DRIVER_H_ */
