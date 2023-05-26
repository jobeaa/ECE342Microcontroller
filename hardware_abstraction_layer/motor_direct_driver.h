/*
 * motor_direct_driver.h
 *
 *  Created on: May 16, 2023
 *      Author: silas
 */

#ifndef MOTOR_DIRECT_DRIVER_H_
#define MOTOR_DIRECT_DRIVER_H_

#include <stdint.h>

// To "emulate" a DAC, time-domain is partitioned.
// Larger partitions enable finer control, but increase latency and lower
// the frequency. High frequency is desirable because a moving motor's
// kinetic energy will smooth the signal, but lower frequencies may produce
// notable motor oscillations.
#define PWM_COMPARATOR_MAX (UINT16_MAX)    // "emulated dac" resolution

typedef struct {
    uint16_t motor_magnitude_gpio_port;
    uint16_t motor_magnitude_gpio_pin;
    uint16_t timer_aX_base_address;                                   // eg TIMER_A0_BASE
    uint16_t timer_aX_channel_compare_register;                       // eg TIMER_A_CAPTURECOMPARE_REGISTER_1
    uint16_t motor_magnitude_gpio_timer_ax_channel_module_function;   // eg GPIO_PRIMARY_MODULE_FUNCTION
    uint16_t timer_aX_period;
    uint16_t motor_direction_gpio_port;
    uint16_t motor_direction_gpio_pin;
} motor_direct_driver_t;

// Initialize driver and underlying peripherals.
// EXPECTATIONS:
//      - driver's relevant members are set
void motor_direct_driver_open(motor_direct_driver_t* driver);

// -PWM_COMPARATOR_MAX <= power <= PWM_COMPARATOR_MAX
inline void motor_direct_driver_set_output(motor_direct_driver_t* driver, int16_t power);

#endif /* MOTOR_DIRECT_DRIVER_H_ */
