/*
 * motor_direct_driver.h
 *
 *  Created on: May 16, 2023
 *      Author: silas
 */

#ifndef MOTOR_DIRECT_DRIVER_H_
#define MOTOR_DIRECT_DRIVER_H_

#include <stdint.h>

typedef struct {
    uint16_t motor_preamp_signal_gpio_port;
    uint16_t motor_preamp_signal_gpio_pin;
    uint16_t motor_preamp_signal_gpio_timer_a0_channel;                 // X in TA0.X
    uint16_t motor_preamp_signal_gpio_timer_a0_channel_module_function; // eg GPIO_PRIMARY_MODULE_FUNCTION
    uint16_t motor_direction_gpio_port;
    uint16_t motor_direction_gpio_pin;
    // To "emulate" a DAC, time-domain is partitioned. Larger partitions
    // enable finer control, but increase maximum_latency between changes in value.
    uint16_t pwm_comparator_max;    // "emulated dac" resolution
} motor_direct_driver_t;

// Initialize driver and underlying peripherals.
// EXPECTATIONS:
//  - passed driver is populated:
//      - GPIO pins and ports are set
//      - pwm_resolution is set
void motor_direct_driver_open(motor_direct_driver_t* driver);

inline void motor_direct_driver_set_output(float power);

#endif /* MOTOR_DIRECT_DRIVER_H_ */
