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

static const uint16_t timer_a_instance_base = (TIMER_A0_BASE);

static bool was_timer_a_configured = false;

static void configure_timer_a() {
    if(!was_timer_a_configured) {
        Timer_A_initContinuousModeParam param;
        param.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
        param.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
        param.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
        param.timerClear = TIMER_A_SKIP_CLEAR;
        param.startTimer = true;
        Timer_A_initContinuousMode(timer_a_instance_base, &param);
    }
}

void motor_direct_driver_open(motor_direct_driver_t* driver) {
    // Ensure TimerA is configured
    configure_timer_a();

    // Configure GPIO Pins
    GPIO_setAsPeripheralModuleFunctionOutputPin(
            driver->motor_preamp_signal_gpio_port,
            driver->motor_preamp_signal_gpio_pin,
            driver->motor_preamp_signal_gpio_timer_a0_channel_module_function
            );
    GPIO_setAsOutputPin(
            driver->motor_direction_gpio_port,
            driver->motor_direction_gpio_pin
            );

    uint16_t compare_register;
    switch(driver->motor_preamp_signal_gpio_timer_a0_channel) {
        case 0:
            compare_register = TIMER_A_CAPTURECOMPARE_REGISTER_0;
            break;
        case 1:
            compare_register = TIMER_A_CAPTURECOMPARE_REGISTER_1;
            break;
        case 2:
            compare_register = TIMER_A_CAPTURECOMPARE_REGISTER_2;
            break;
        default:
            break;
    }

    // Configure PWM for preamp
    Timer_A_initCompareModeParam compare_mode_param;
    compare_mode_param.compareRegister = compare_register;
    compare_mode_param.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    compare_mode_param.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
    compare_mode_param.compareValue = (0xffff/3);
    Timer_A_initCompareMode(
            timer_a_instance_base,
            &compare_mode_param
            );

    Timer_A_setCompareValue(
            timer_a_instance_base,
            compare_register,
            (0xffff/2)
            );
}

inline void motor_direct_driver_set_output(float power) {
    // set preamp to 0

    // update direction gpio state based on sign of `power`.

    // turn on preamp proportional to power
}
