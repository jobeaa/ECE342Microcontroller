/*
 * servo_driver.c
 *
 *  Created on: May 17, 2023
 *      Author: silas
 */

#include "servo_driver.h"

#define POWER_SIGNAL_POSITION_MAX 10
#define POWER_SIGNAL_POSITION_MIN 100


void servo_driver_open(servo_driver_t* driver) {
    motor_direct_driver_open(&(driver->motor_controller));
}

inline void servo_driver_move_to(servo_driver_t* driver, uint16_t position_degrees) {
    if(position_degrees > POSITION_MAX_DEGREES) {
        position_degrees = POSITION_MAX_DEGREES;
    }

    // linear mapping between angular position and pwm control signal
    int16_t slope_times_10 = 10*(POWER_SIGNAL_POSITION_MIN - POWER_SIGNAL_POSITION_MAX)/(0 - POSITION_MAX_DEGREES);
    int16_t y_intercept = POWER_SIGNAL_POSITION_MIN;
    int16_t position_power = ((int16_t)(slope_times_10*position_degrees))/10 + y_intercept;

    // servo pwm signal period cannot be too large. tested with ~12 ms.
    motor_direct_driver_set_output(
            &(driver->motor_controller),
            position_power);
}
