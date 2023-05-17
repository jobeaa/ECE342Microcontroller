/*
 * servo_driver.c
 *
 *  Created on: May 17, 2023
 *      Author: silas
 */

#include "servo_driver.h"

void servo_driver_open(servo_driver_t* driver) {
    motor_direct_driver_open(&(driver->motor_controller));
}

inline void servo_driver_move_to(servo_driver_t* driver, bool position_degrees) {
//    if(position_degrees > POSITION_MAX_DEGREES) {
//        position_degrees = POSITION_MAX_DEGREES;
//    }


    // scale position_degress to match underlying motor_controller's resolution
//    uint16_t position_power =  position_degrees * PWM_COMPARATOR_MAX;
//    position_degrees /= POSITION_MAX_DEGREES;



    motor_direct_driver_set_output(
            &(driver->motor_controller),
            // 1000 max, 1 min
            ((position_degrees * 999) + 1)
            );

}
