/*
 * pwm_driver.c
 *
 *  Created on: May 16, 2023
 *      Author: silas
 */

#include "pwm_driver.h"
#include "timer_a.h"
#include "gpio.h"

// Configures the timer A given the timer_aX_base_address reference.
// When the timer is configured, its added to an internal list so 
// that successive calls on the same timer will not alter 
// configuration.
static void ensure_configure_timer_a(pwm_driver_t* driver) {
    static uint16_t timer_aX_configured_base_addresses[8] = {0};
    int16_t first_empty_index = -1;
    uint16_t i;
    for(i = 0; i < 8; i++) {
        if(timer_aX_configured_base_addresses[i] == driver->timer_aX_base_address) {
           return;
        }
        if((first_empty_index != -1) && (!timer_aX_configured_base_addresses[i])) {
            first_empty_index = i;
        }
    }

    Timer_A_initUpModeParam timer_a_up_mode_param;
    timer_a_up_mode_param.clockSource = driver->timer_aX_clock_source;
    timer_a_up_mode_param.clockSourceDivider = driver->timer_aX_clock_source_divider;
    timer_a_up_mode_param.timerPeriod = driver->timer_aX_period;
    timer_a_up_mode_param.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    timer_a_up_mode_param.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
    timer_a_up_mode_param.timerClear = TIMER_A_SKIP_CLEAR;
    timer_a_up_mode_param.startTimer = true;
    Timer_A_initUpMode(driver->timer_aX_base_address, &timer_a_up_mode_param);

    timer_aX_configured_base_addresses[first_empty_index] = driver->timer_aX_base_address;
}

inline static void configure_gpio_for_timer_ax(pwm_driver_t* driver) {
    GPIO_setAsPeripheralModuleFunctionOutputPin(
            driver->gpio_port,
            driver->gpio_pin,
            driver->gpio_timer_aX_channel_module_function
            );
}


void pwm_driver_open(pwm_driver_t* driver) {
    configure_gpio_for_timer_ax(driver);

    ensure_configure_timer_a(driver);

    // Configure PWM
    Timer_A_initCompareModeParam timer_a_compare_mode_param;
    timer_a_compare_mode_param.compareRegister = driver->timer_aX_channel_compare_register;
    timer_a_compare_mode_param.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    timer_a_compare_mode_param.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
    timer_a_compare_mode_param.compareValue = 0;        // set to 0 during initialization
    Timer_A_initCompareMode(
            driver->timer_aX_base_address,
            &timer_a_compare_mode_param
            );
}

inline static void configure_and_set_gpio_low(pwm_driver_t* driver) {
    GPIO_setAsOutputPin(
            driver->gpio_port,
            driver->gpio_pin
           );
    GPIO_setOutputLowOnPin(
            driver->gpio_port,
            driver->gpio_pin
           );
}

void pwm_driver_set_duty_cycle(pwm_driver_t* driver, uint16_t duty_cycle_0_to_1_times_1024) {
    uint32_t magnitude_32 = (((uint32_t)duty_cycle_0_to_1_times_1024) * ((uint32_t)driver->timer_aX_period)) /
                    ((uint32_t)1024);
    uint16_t magnitude = ((uint16_t)magnitude_32);

    configure_and_set_gpio_low(driver);

    // NOTE: user's guide states pwm should be zeroed before writing new value.
    //       driverlib implementation does not zero.
//    Timer_A_setCompareValue(
//            driver->timer_aX_base_address,
//            driver->timer_aX_channel_compare_register,
//            0
//            );

    Timer_A_setCompareValue(
            driver->timer_aX_base_address,
            driver->timer_aX_channel_compare_register,
            magnitude
            );

    configure_gpio_for_timer_ax(driver);
}
