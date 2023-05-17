/*
 * motor_direct_driver.c
 *
 *  Created on: May 16, 2023
 *      Author: silas
 */

#include "motor_direct_driver.h"
#include "timer_a.h"
#include "gpio.h"

#include <stdbool.h>

static void configure_timer_a(uint16_t timer_aX_base_address) {
    static uint16_t timer_aX_configured_base_addresses[8] = {0};
    int16_t first_empty_index = -1;
    uint16_t i;
    for(i = 0; i < 8; i++) {
        if(timer_aX_configured_base_addresses[i] == timer_aX_base_address) {
           return;
        }
        if((first_empty_index != -1) && (!timer_aX_configured_base_addresses[i])) {
            first_empty_index = i;
        }
    }

    Timer_A_initUpModeParam param;
    param.clockSource = TIMER_A_CLOCKSOURCE_ACLK;
    param.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    param.timerPeriod = PWM_COMPARATOR_MAX;
    param.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    param.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
    param.timerClear = TIMER_A_SKIP_CLEAR;
    param.startTimer = true;
    Timer_A_initUpMode(timer_aX_base_address, &param);

    timer_aX_configured_base_addresses[first_empty_index] = timer_aX_base_address;
}

void motor_direct_driver_open(motor_direct_driver_t* driver) {
    // Ensure TimerA is configured
    configure_timer_a(driver->timer_aX_base_address);

    // Configure GPIO Pins
    GPIO_setAsPeripheralModuleFunctionOutputPin(
            driver->motor_magnitude_gpio_port,
            driver->motor_magnitude_gpio_pin,
            driver->motor_magnitude_gpio_timer_ax_channel_module_function
            );
    GPIO_setAsOutputPin(
            driver->motor_direction_gpio_port,
            driver->motor_direction_gpio_pin
            );

    // Configure PWM for magnitude
    Timer_A_initCompareModeParam compare_mode_param;
    compare_mode_param.compareRegister = driver->timer_aX_channel_compare_register;
    compare_mode_param.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    compare_mode_param.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
    compare_mode_param.compareValue = 0;
    Timer_A_initCompareMode(
            driver->timer_aX_base_address,
            &compare_mode_param
            );
}

inline void motor_direct_driver_set_output(motor_direct_driver_t* driver, int16_t power) {
    bool is_power_positive = (power > 0);
    uint16_t magnitude = abs(power);

    // set magnitude to 0
    // change output mode to reset so that pin is low while changing duty cycle
    GPIO_setAsOutputPin(
            driver->motor_magnitude_gpio_port,
            driver->motor_magnitude_gpio_pin
           );
    GPIO_setOutputLowOnPin(
            driver->motor_magnitude_gpio_port,
            driver->motor_magnitude_gpio_pin
           );
    // NOTE: user's guide states pwm should be zeroed before writing new value.
    //       driverlib implementation does not zero.
    Timer_A_setCompareValue(
            driver->timer_aX_base_address,
            driver->timer_aX_channel_compare_register,
            0
            );

    // update direction gpio state based on sign of `power`
    if(is_power_positive) {
        GPIO_setOutputHighOnPin(
                driver->motor_direction_gpio_port,
                driver->motor_direction_gpio_pin
                );
    }
    else {
        GPIO_setOutputLowOnPin(
                driver->motor_direction_gpio_port,
                driver->motor_direction_gpio_pin
                );
    }

    // set magnitude proportional to power
    Timer_A_setCompareValue(
            driver->timer_aX_base_address,
            driver->timer_aX_channel_compare_register,
            magnitude
            );
    // restore output mode
    GPIO_setAsPeripheralModuleFunctionOutputPin(
            driver->motor_magnitude_gpio_port,
            driver->motor_magnitude_gpio_pin,
            driver->motor_magnitude_gpio_timer_ax_channel_module_function
           );
}
