/*
 * servo_driver.c
 *
 *  Created on: May 17, 2023
 *      Author: silas
 */

#include "servo_driver.h"

#define DUTY_CYCLE_0_to_1_TIMES_1024_POSITION_MAX 27
#define DUTY_CYCLE_0_to_1_TIMES_1024_POSITION_MIN 256


void servo_driver_open(servo_driver_t* driver) {
    pwm_driver_open(&(driver->pwm_driver));
}

void servo_driver_move_to(servo_driver_t* driver, uint16_t position_degrees) {
    if(position_degrees > POSITION_MAX_DEGREES) {
        position_degrees = POSITION_MAX_DEGREES;
    }

    // linear mapping between angular position and pwm control signal
    int16_t slope_times_10 = 10*(DUTY_CYCLE_0_to_1_TIMES_1024_POSITION_MIN - DUTY_CYCLE_0_to_1_TIMES_1024_POSITION_MAX) /
            (0 - POSITION_MAX_DEGREES);
    int16_t y_intercept = DUTY_CYCLE_0_to_1_TIMES_1024_POSITION_MIN;
    int16_t position_duty_cycle_0_to_1_times_1024 = ((int16_t)(slope_times_10*position_degrees))/10 + y_intercept;

    // servo pwm signal period cannot be too large. tested with ~12 ms.
    pwm_driver_set_duty_cycle(
            &(driver->pwm_driver),
            position_duty_cycle_0_to_1_times_1024);
}
