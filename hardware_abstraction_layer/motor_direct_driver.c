/*
 * motor_direct_driver.c
 *
 *  Created on: May 16, 2023
 *      Author: silas
 */

#include "motor_direct_driver.h"
#include "gpio.h"

#include <stdbool.h>

void motor_direct_driver_open(motor_direct_driver_t* driver) {
    GPIO_setAsOutputPin(
            driver->motor_direction_enable_1_gpio_port,
            driver->motor_direction_enable_1_gpio_pin
            );
    GPIO_setAsOutputPin(
            driver->motor_direction_enable_2_gpio_port,
            driver->motor_direction_enable_2_gpio_pin
            );
}

inline void set_direction_negative(motor_direct_driver_t* driver) {
    GPIO_setOutputHighOnPin(driver->motor_direction_enable_1_gpio_port,
                            driver->motor_direction_enable_1_gpio_pin);
    GPIO_setOutputLowOnPin(driver->motor_direction_enable_2_gpio_port,
                            driver->motor_direction_enable_2_gpio_pin);
}

inline void set_direction_positive(motor_direct_driver_t* driver) {
    GPIO_setOutputLowOnPin(driver->motor_direction_enable_1_gpio_port,
                            driver->motor_direction_enable_1_gpio_pin);
    GPIO_setOutputHighOnPin(driver->motor_direction_enable_2_gpio_port,
                            driver->motor_direction_enable_2_gpio_pin);
}

inline void motor_direct_driver_set_output(motor_direct_driver_t* driver, int16_t power_within_plus_minus_1024) {
    bool is_power_positive = (power_within_plus_minus_1024 > 0);
    uint16_t magnitude = abs(power_within_plus_minus_1024);

    pwm_driver_set_duty_cycle(&(driver->motor_magnitude_pwm_driver), magnitude);

    //output
    if(is_power_positive) {
        set_direction_positive(driver);
    } else {
        set_direction_negative(driver);
    }
}
