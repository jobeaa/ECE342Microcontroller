/*
 * pwm_driver.h
 *
 *  Created on: May 25, 2023
 *      Author: silas
 */

#ifndef PWM_DRIVER_H_
#define PWM_DRIVER_H_

#include <stdint.h>

typedef struct {
    uint16_t gpio_port;
    uint16_t gpio_pin;
    uint16_t gpio_timer_aX_channel_module_function;     // eg GPIO_PRIMARY_MODULE_FUNCTION
    uint16_t timer_aX_base_address;                     // eg TIMER_A0_BASE
    uint16_t timer_aX_channel_compare_register;         // eg TIMER_A_CAPTURECOMPARE_REGISTER_1
    uint16_t timer_aX_clock_source;                     // eg TIMER_A_CLOCKSOURCE_ACLK
    uint16_t timer_aX_clock_source_divider;             // eg TIMER_A_CLOCKSOURCE_DIVIDER_1
    uint16_t timer_aX_period;                           // period between pwm pulses in terms of timer aX's divided clock source count
} pwm_driver_t;

// Initialize driver and underlying peripherals.
// EXPECTATIONS:
//      - driver's relevant members are set
void pwm_driver_open(pwm_driver_t* driver);

// Sets the duty cycle of the pwm signal.
// 
// duty_cycle_percent_times_1024:   the duty cycle percentage (0 to 1) * 1024
void pwm_driver_set_duty_cycle(pwm_driver_t* driver, uint16_t duty_cycle_0_to_1_times_1024);

#endif // PWM_DRIVER_H_
